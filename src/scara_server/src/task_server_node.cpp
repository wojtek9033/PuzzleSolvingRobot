#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <scara_msgs/action/scara_task.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/bool.hpp>
#include <moveit/move_group_interface/move_group_interface.hpp>

class ScaraTaskServer : public rclcpp::Node {
public:
    using ScaraTask = scara_msgs::action::ScaraTask;
    using GoalHandle = rclcpp_action::ServerGoalHandle<ScaraTask>;

    ScaraTaskServer() : Node("Scara_task_server") {
            task_action_server_ = rclcpp_action::create_server<ScaraTask>(
                this,
                "scara_task",
                std::bind(&ScaraTaskServer::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&ScaraTaskServer::handle_cancel, this, std::placeholders::_1),
                std::bind(&ScaraTaskServer::handle_accepted, this, std::placeholders::_1)
            );

            ready_pub_ = this->create_publisher<std_msgs::msg::Bool>("/robot_is_pos", 10);

            generate_capture_waypoints();
            RCLCPP_INFO(this->get_logger(), "Scara Task Server started.");
        }
        

private:
    rclcpp_action::Server<ScaraTask>::SharedPtr task_action_server_;
    std::shared_ptr<moveit::planning_interface::MoveGroupInterface> move_arm_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ready_pub_;
    std::vector<geometry_msgs::msg::Pose> capture_poses_;

    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID &uuid,
        std::shared_ptr<const ScaraTask::Goal> goal) {
            (void)uuid;
            RCLCPP_INFO(this->get_logger(), "Received goal: %s", goal->command.c_str());
            return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandle> goal_handle) {
        (void)goal_handle;
        RCLCPP_INFO(this->get_logger(), "Received cancel goal request");
        auto move_arm_ = moveit::planning_interface::MoveGroupInterface(shared_from_this(), "arm");
        move_arm_.stop();
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    void handle_accepted(const std::shared_ptr<GoalHandle> goal_handle) {
        std::thread{std::bind(&ScaraTaskServer::execute, this, goal_handle)}.detach();
    }

    void execute(const std::shared_ptr<GoalHandle> goal_handle){
        RCLCPP_INFO(get_logger(), "Executing goal...");
        auto result = std::make_shared<ScaraTask::Result>();
        auto feedback = std::make_shared<ScaraTask::Feedback>();

        if(!move_arm_){
            move_arm_ = std::make_shared<moveit::planning_interface::MoveGroupInterface>(shared_from_this(), "arm");
        }

        const auto &cmd = goal_handle->get_goal()->command;
        if (cmd == "capture") {
            for (size_t i = 0; i < 1; i++) {
                if (goal_handle->is_canceling()) {
                    result->success = false;
                    goal_handle->canceled(result);
                    RCLCPP_WARN(this->get_logger(), "Capture task canceled.");
                    return;
                }
    
                feedback->current_step = "Moving  to capture pose " + std::to_string(i + 1);
                goal_handle->publish_feedback(feedback);

                //move_arm_->setStartState(*move_arm_->getCurrentState());
                move_arm_->setPoseReferenceFrame("base_link");
                move_arm_->setStartStateToCurrentState();

                moveit::planning_interface::MoveGroupInterface::Plan arm_plan;
                geometry_msgs::msg::Pose pose;
                pose.position.x = 1.6;
                pose.position.y = 0.8;
                pose.position.z = 0.3;
                pose.orientation.w = 1.0;

                
                move_arm_->setPoseTarget(pose);
                bool arm_plan_success = (move_arm_->plan(arm_plan) == moveit::core::MoveItErrorCode::SUCCESS);
                
                if(arm_plan_success){
                    RCLCPP_INFO(this->get_logger(), "Planning SUCCEED for pose %ld!", i);
                    move_arm_->move();
                } else {
                    RCLCPP_WARN(this->get_logger(), "Planning FAILED for pose %ld!", i);
                    result->success = false;
                    goal_handle->abort(result);
                    return;
                }
    
                // rclcpp::sleep_for(std::chrono::milliseconds(250));
                std_msgs::msg::Bool msg;
                msg.data = true;
                ready_pub_->publish(msg);
    
                RCLCPP_INFO(this->get_logger(), "At pose %ld, notified camera", i + 1);
            }
            result->success = true;
            goal_handle->succeed(result);
        } else {
            RCLCPP_ERROR(this->get_logger(), "Unknown command '%s'", cmd.c_str());
            auto result = std::make_shared<ScaraTask::Result>();
            result->success = false;
            goal_handle->abort(result);
        }
        
    }

    void generate_capture_waypoints() {
        geometry_msgs::msg::Pose pose;
        for (int i = 0; i < 9; i ++) {
            pose.position.x = 1.6;
            pose.position.y = 0.8;
            pose.position.z = 0.3;
            pose.orientation.w = 0.5;
            // pose.position.x = 0.5 + 0.5*(i/3);
            // pose.position.y = -0.8 - 0.5*(i%3);
            // pose.position.z = 0.7;
            // pose.orientation.w = 1.0;
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