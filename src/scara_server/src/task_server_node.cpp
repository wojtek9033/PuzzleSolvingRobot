#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <scara_msgs/action/scara_task.hpp>
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

            arm_cmd_pub_ = this -> create_publisher<geometry_msgs::msg::Pose>(
                "/scara/ik_goal",
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

            generate_capture_waypoints();
            RCLCPP_INFO(this->get_logger(), "Scara Task Server started.");
        }
        

private:
    rclcpp_action::Server<ScaraTask>::SharedPtr task_action_server_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ready_pub_;
    rclcpp::Publisher<geometry_msgs::msg::Pose>::SharedPtr arm_cmd_pub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr confirm_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr arm_in_pos_sub_;
    std::vector<geometry_msgs::msg::Pose> capture_poses_;
    std::mutex solver_confirm_mutex_;
    std::mutex robot_confirm_mutex_;
    std::condition_variable solver_confirm_;
    std::condition_variable robot_confirm_; 
    bool solver_confirm_received_ = false;
    bool robot_confirm_received_ = false;
    bool canceled_ = false;
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

    void execute(const std::shared_ptr<GoalHandle> goal_handle){
        RCLCPP_INFO(get_logger(), "Initializing arm...");
        auto result = std::make_shared<ScaraTask::Result>();
        auto feedback = std::make_shared<ScaraTask::Feedback>();
 
        arm_cmd_pub_->publish(scara_positions::arm_middle_pose.start_pose);
        if (wait_for_arm_confirm()) {
            RCLCPP_INFO(this->get_logger(), "Arm initialized.");
        } else {
            RCLCPP_ERROR(this->get_logger(), "Timeout: Did not receive arm movement confirmation.");
            goal_handle->abort(result);
        }
        RCLCPP_INFO(get_logger(), "Executing goal...");

        const auto &cmd = goal_handle->get_goal()->command;
        if (cmd == "capture") {
            // rozmiar pola widzenia 49mm x 36mm w:1600 h:1200
            for(size_t i = 0; i < PUZZLE_SIZE; i++) {
                if(goal_handle->is_canceling()) {
                    RCLCPP_INFO(this->get_logger(), "Capture goal canceled.");
                    result->success = false;
                    goal_handle->canceled(result);
                    break;
                }
                arm_cmd_pub_->publish(scara_positions::robot_poses.at(i).start_pose);
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

        } else if (cmd == "calibrate") {

            arm_cmd_pub_ -> publish(scara_positions::first_piece_pose.start_pose);

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

    void generate_capture_waypoints() {
        geometry_msgs::msg::Pose pose;
        int puzzleSize{9};
        for (int i = 0; i < puzzleSize; i ++) {
            double theta = M_PI;
            pose.orientation.x = 0.0;
            pose.orientation.y = 0.0;
            pose.orientation.z = sin(theta/ 2.0);
            pose.orientation.w = cos(theta/ 2.0);
            pose.position.x = 0.07 + 0.04*(i / (int)sqrt(puzzleSize));
            pose.position.y = -0.18 + 0.04*(i % (int)sqrt(puzzleSize));
            pose.position.z = 0.13;
            capture_poses_.push_back(pose);
        }
    }
};

int main (int argc, char** argv) {
    rclcpp::init(argc,argv);
    auto node = std::make_shared<ScaraTaskServer>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}