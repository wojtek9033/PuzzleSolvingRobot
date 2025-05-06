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
        if (cmd == "test") {
            std::vector<double> arm_joint_goal = {1.2, 1.2, 1.2, 0.0};
            move_arm_->setPoseReferenceFrame("base_link");
            move_arm_->setStartStateToCurrentState();

            bool arm_within_bounds = move_arm_->setJointValueTarget(arm_joint_goal);
            if (!arm_within_bounds)
            {
              RCLCPP_WARN(get_logger(), "Target joint position(s) were outside of limits, but we will plan and clamp to the limits ");
              return;
            }

            moveit::planning_interface::MoveGroupInterface::Plan arm_plan;
            bool arm_plan_success = (move_arm_->plan(arm_plan) == moveit::core::MoveItErrorCode::SUCCESS);
            
            if(arm_plan_success)
            {
              RCLCPP_INFO(get_logger(), "Planner SUCCEED, moving the arm");
              move_arm_->move();
            }
            else
            {
              RCLCPP_ERROR(get_logger(), "One or more planners failed!");
              return;
            }
          
            result->success = true;
            goal_handle->succeed(result);
            RCLCPP_INFO(get_logger(), "Goal succeeded");

        } else if (cmd == "capture") {
            for (size_t i = 0; i < 1; i++) {
                if (goal_handle->is_canceling()) {
                    result->success = false;
                    goal_handle->canceled(result);
                    RCLCPP_WARN(this->get_logger(), "Capture task canceled.");
                    return;
                }
    
                feedback->current_step = "Moving  to capture pose " + std::to_string(i + 1);
                goal_handle->publish_feedback(feedback);

                move_arm_->setPoseReferenceFrame("base_link");
                move_arm_->setStartStateToCurrentState();
                //move_arm_->setGoalTolerance(0.01);
                //move_arm_->setGoalPositionTolerance(0.01);   // 1 cm tolerance in position
                //move_arm_->setGoalOrientationTolerance(0.05);
                bool arm_within_bounds = move_arm_->setPoseTarget(capture_poses_.at(i));
                //move_arm_->setRandomTarget();
                
                if (!arm_within_bounds)
                {
                  RCLCPP_ERROR(get_logger(), "Target joint position(s) were outside of limits!");
                  return;
                }
                
                moveit::planning_interface::MoveGroupInterface::Plan arm_plan;
                bool arm_plan_success = (move_arm_->plan(arm_plan) == moveit::core::MoveItErrorCode::SUCCESS);
                
                if(arm_plan_success)
                {
                  RCLCPP_INFO(get_logger(), "Planner SUCCEED for pose %ld, moving the arm", i + 1);
                  move_arm_->move();
                }
                else
                {
                  RCLCPP_ERROR(get_logger(), "One or more planners FAILED for pose %ld!", i + 1);
                  return;
                }
                //geometry_msgs::msg::PoseStamped TestPose = move_arm_->getCurrentPose();
                //RCLCPP_INFO(this->get_logger(), "Current pose is x: %lf y: %lf z: %lf w: %lf", TestPose.pose.position.x, TestPose.pose.position.y, TestPose.pose.position.z, TestPose.pose.orientation.w);
                // rclcpp::sleep_for(std::chrono::milliseconds(250));
                std_msgs::msg::Bool msg;
                msg.data = true;
                ready_pub_->publish(msg);
    
                RCLCPP_INFO(this->get_logger(), "At pose %ld, notified camera", i + 1);
            }
            result->success = true;
            goal_handle->succeed(result);
            RCLCPP_INFO(this->get_logger(), "Goal capture succeed");
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
            // x: 2.002987 y: 0.247675 z: 1.340460 w: 0.612770 - from setRandomPose()
            pose.orientation.x = 1e-6;
            pose.orientation.y = 1e-6;
            pose.orientation.z = 1e-6;
            pose.orientation.w = 0.612770;
            pose.position.x = 2.002987;
            pose.position.y = 0.247675;
            pose.position.z = 1.340460;
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