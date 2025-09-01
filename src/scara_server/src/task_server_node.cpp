#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <scara_msgs/action/scara_task.hpp>
#include <scara_msgs/msg/puzzle_assembly.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/bool.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <memory>

#include "puzzle_solver/scara_positions.h"

using namespace std::chrono_literals;

class ScaraTaskServer : public rclcpp::Node {
public:
    using ScaraTask = scara_msgs::action::ScaraTask;
    using GoalHandle = rclcpp_action::ServerGoalHandle<ScaraTask>;

    ScaraTaskServer()
         :  Node("Scara_task_server"),
            solver_confirm_received_(false),
            robot_confirm_received_(false),
            canceled_(false),
            received_assembly_data_(false),
            PUZZLE_SIZE(4) {
            
            task_action_server_ = rclcpp_action::create_server<ScaraTask>(
                this,
                "scara_task",
                std::bind(&ScaraTaskServer::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&ScaraTaskServer::handle_cancel, this, std::placeholders::_1),
                std::bind(&ScaraTaskServer::handle_accepted, this, std::placeholders::_1)
            );

            ready_pub_ = this->create_publisher<std_msgs::msg::Bool>(
                "/capture/trigger",
                 10
            );

            gripper_on_goal_pub_ = this -> create_publisher<geometry_msgs::msg::Pose>(
                "/scara/gripper_on/ik_goal",
                10
            );

            gripper_off_goal_pub_ = this -> create_publisher<geometry_msgs::msg::Pose>(
                "/scara/gripper_off/ik_goal",
                10
            );

            camera_goal_pub_ = this -> create_publisher<geometry_msgs::msg::Pose>(
                "/scara/camera/ik_goal",
                10
            );

            confirm_sub_ = this->create_subscription<std_msgs::msg::Bool>(
                "/capture/confirm",
                10,
                std::bind(&ScaraTaskServer::confirm_callback, this, std::placeholders::_1)
            );

            arm_in_pos_sub_ = this->create_subscription<std_msgs::msg::Bool> (
                "/scara/in_pos",
                10,
                std::bind(&ScaraTaskServer::arm_in_pos_callback, this, std::placeholders::_1)
            );

            puzzle_assembly_sub_ = this->create_subscription<scara_msgs::msg::PuzzleAssembly> (
                "/puzzle/solution",
                10,
                std::bind(&ScaraTaskServer::assembly_data_callback, this, std::placeholders::_1)
            );
            RCLCPP_INFO(this->get_logger(), "Scara Task Server started.");
        }
        

private:
    rclcpp_action::Server<ScaraTask>::SharedPtr task_action_server_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ready_pub_;
    rclcpp::Publisher<geometry_msgs::msg::Pose>::SharedPtr gripper_on_goal_pub_, gripper_off_goal_pub_, camera_goal_pub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr confirm_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr arm_in_pos_sub_;
    rclcpp::Subscription<scara_msgs::msg::PuzzleAssembly>::SharedPtr puzzle_assembly_sub_;
    std::vector<geometry_msgs::msg::Pose> capture_poses_;
    std::vector<scara_msgs::msg::PiecePose> assembly_positions_;
    std::mutex solver_confirm_mutex_;
    std::mutex robot_confirm_mutex_;
    std::condition_variable solver_confirm_;
    std::condition_variable robot_confirm_; 
    bool solver_confirm_received_, robot_confirm_received_, canceled_, received_assembly_data_;
    size_t PUZZLE_SIZE;

    rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID &uuid, std::shared_ptr<const ScaraTask::Goal> goal) {
            (void)uuid;
            RCLCPP_INFO(this->get_logger(), "Received goal: %s", goal->command.c_str());
            return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandle> goal_handle) {
        (void)goal_handle;
        RCLCPP_INFO(this->get_logger(), "Received cancel goal request");
        {
            std::lock_guard<std::mutex> lock(solver_confirm_mutex_);
            canceled_ = true;
        }
        solver_confirm_.notify_all();
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    void handle_accepted(const std::shared_ptr<GoalHandle> goal_handle) {
        std::thread{std::bind(&ScaraTaskServer::execute, this, goal_handle)}.detach();
    }

    bool wait_for_arm_confirm() {
        {
            std::lock_guard<std::mutex> lock(robot_confirm_mutex_);
            robot_confirm_received_ = false;
        }

        std::unique_lock<std::mutex> lock(robot_confirm_mutex_);
        bool completed = robot_confirm_.wait_for(lock, std::chrono::seconds(10), [this]() {
            return robot_confirm_received_;
        });

        if (completed) {
            return 1;
        } else {
            RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation");
            return 0;
        }
    }

    bool initialize_arm() {
        RCLCPP_INFO(get_logger(), "Initializing arm...");
        gripper_off_goal_pub_->publish(scara_positions::arm_middle_pose.start_pose);
        if (wait_for_arm_confirm()) {
            RCLCPP_INFO(this->get_logger(), "Arm initialized.");
            return true;
        } else {
            RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
            return false;
        }
    }

    void execute(const std::shared_ptr<GoalHandle> goal_handle){
        auto result = std::make_shared<ScaraTask::Result>();
        auto feedback = std::make_shared<ScaraTask::Feedback>();
 
        const auto &cmd = goal_handle->get_goal()->command;
        if (cmd == "capture") {
            if (!initialize_arm()) {
                result->success = false;
                goal_handle->abort(result);
            }
            // rozmiar pola widzenia 49mm x 36mm w:1600 h:1200
            RCLCPP_INFO(get_logger(), "Executing goal: capture");
            for(size_t i = 0; i < PUZZLE_SIZE; i++) {
                if(goal_handle->is_canceling()) {
                    RCLCPP_INFO(this->get_logger(), "Capture goal canceled.");
                    result->success = false;
                    goal_handle->canceled(result);
                    break;
                }
                camera_goal_pub_->publish(scara_positions::robot_poses.at(i).start_pose);
                if (wait_for_arm_confirm()) {
                    feedback->current_step = i;
                    rclcpp::sleep_for(std::chrono::milliseconds(500));
                    goal_handle->publish_feedback(feedback);
                    RCLCPP_INFO(this->get_logger(), "Server waiting for solver to capture picture...");
                } else {
                    RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
                    result->success = false;
                    goal_handle->abort(result);
                }
            }
            result->success = true;
            goal_handle->succeed(result);
        
        } else if (cmd == "assemble") {
            if (!received_assembly_data_) {
                result->success = false;
                goal_handle->abort(result);
            }
            if (!initialize_arm()) {
                result->success = false;
                goal_handle->abort(result);
            }
            RCLCPP_INFO(get_logger(), "Executing goal: assemble");

            for (size_t i = 0; i < assembly_positions_.size(); i++) {
                if(goal_handle->is_canceling()) {
                    RCLCPP_INFO(this->get_logger(), "Assembly goal canceled.");
                    result->success = false;
                    goal_handle->canceled(result);
                    break;
                }

                scara_msgs::msg::PiecePose pose_above_table;
                pose_above_table.start_pose.position.x = assembly_positions_.at(i).start_pose.position.x;
                pose_above_table.start_pose.position.y = assembly_positions_.at(i).start_pose.position.y;
                pose_above_table.start_pose.position.z = 0.221;
                pose_above_table.start_pose.orientation.z = assembly_positions_.at(i).start_pose.orientation.z;
                
                pose_above_table.goal_pose.position.x = assembly_positions_.at(i).goal_pose.position.x;
                pose_above_table.goal_pose.position.y = assembly_positions_.at(i).goal_pose.position.y;
                pose_above_table.goal_pose.position.z = 0.221;
                pose_above_table.goal_pose.orientation.z = assembly_positions_.at(i).goal_pose.orientation.z;

                RCLCPP_INFO(this->get_logger(), "Moving arm to start position above pickup table for element %ld.", i);
                gripper_off_goal_pub_->publish(pose_above_table.start_pose);
                if (!wait_for_arm_confirm()) {
                    RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
                    result->success = false;
                    goal_handle->abort(result);
                    break;
                }
                RCLCPP_INFO(this->get_logger(), "Moving arm to pickup position for element %ld.", i);
                gripper_on_goal_pub_->publish(assembly_positions_.at(i).start_pose);
                if (!wait_for_arm_confirm()) {
                    RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
                    result->success = false;
                    goal_handle->abort(result);
                    break;
                }
                RCLCPP_INFO(this->get_logger(), "Moving arm with element %ld to position above pickup table.", i);
                gripper_on_goal_pub_->publish(pose_above_table.start_pose);
                if (!wait_for_arm_confirm()) {
                    RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
                    result->success = false;
                    goal_handle->abort(result);
                    break;
                }
                RCLCPP_INFO(this->get_logger(), "Moving arm with element %ld to position above assembly table.", i);
                gripper_on_goal_pub_->publish(scara_positions::arm_middle_pose.start_pose);
                if (!wait_for_arm_confirm()) {
                    RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
                    result->success = false;
                    goal_handle->abort(result);
                    break;
                }
                gripper_on_goal_pub_->publish(pose_above_table.goal_pose);
                if (!wait_for_arm_confirm()) {
                    RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
                    result->success = false;
                    goal_handle->abort(result);
                    break;
                }
                RCLCPP_INFO(this->get_logger(), "Placing element %ld on assembly table.", i);
                gripper_on_goal_pub_->publish(assembly_positions_.at(i).goal_pose);
                if (!wait_for_arm_confirm()) {
                    RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
                    result->success = false;
                    goal_handle->abort(result);
                    break;
                }
                RCLCPP_INFO(this->get_logger(), "Finishing procedure for element %ld.", i);
                gripper_off_goal_pub_->publish(pose_above_table.goal_pose);
                if (!wait_for_arm_confirm()) {
                    RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
                    result->success = false;
                    goal_handle->abort(result);
                    break;
                }
                gripper_off_goal_pub_->publish(scara_positions::arm_middle_pose.start_pose);
                if (!wait_for_arm_confirm()) {
                    RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
                    result->success = false;
                    goal_handle->abort(result);
                    break;
                }
                feedback->current_step = i;
                goal_handle->publish_feedback(feedback);
            }

        } else if (cmd == "calibrate") {
            RCLCPP_INFO(get_logger(), "Executing goal: calibrate");
            if (!initialize_arm()) {
                RCLCPP_INFO(get_logger(), "Failed to initialize arm.");
                result->success = false;
                goal_handle->abort(result);
            }
            camera_goal_pub_ -> publish(scara_positions::first_piece_pose.start_pose);

            if (wait_for_arm_confirm()) {
                result->success = true;
                goal_handle->succeed(result);
            } else {
                RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
                result->success = false;
                goal_handle->abort(result);
            }

        } else {
            RCLCPP_ERROR(this->get_logger(), "Unknown command '%s'", cmd.c_str());
            auto result = std::make_shared<ScaraTask::Result>();
            result->success = false;
            goal_handle->abort(result);
            return;
        }
         RCLCPP_INFO(this->get_logger(), "Server finished %s task.", cmd.c_str());
    }

    void confirm_callback(const std_msgs::msg::Bool::SharedPtr msg){
        if(!msg->data)
            return;
        std::lock_guard<std::mutex> lock(solver_confirm_mutex_); // temporary locking the solver_confirm_mutex_ to set the confirmation flag 
        solver_confirm_received_ = true;
        solver_confirm_.notify_one(); // waking up the waiting thread in execute()
    }

    void arm_in_pos_callback(const std_msgs::msg::Bool::SharedPtr msg) {   
        if (msg->data) {
            std::lock_guard<std::mutex> lock (robot_confirm_mutex_);
            robot_confirm_received_ = true;
            robot_confirm_.notify_one();
        }
    }

    void assembly_data_callback(scara_msgs::msg::PuzzleAssembly::SharedPtr msg) {
        assembly_positions_ = msg->puzzle_assembly;
        received_assembly_data_ = true;
    }

};

int main (int argc, char** argv) {
    rclcpp::init(argc,argv);
    auto node = std::make_shared<ScaraTaskServer>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}