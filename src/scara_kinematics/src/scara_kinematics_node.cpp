#include <cmath>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <urdf/model.h>
#include <Eigen/Dense>

class ScaraKinematics : public rclcpp::Node {
public:
    ScaraKinematics() : Node("scara_kinematics_node") {
        using std::placeholders::_1;

        rclcpp::QoS qos_profile(rclcpp::KeepLast(1));
        qos_profile.transient_local();
        desc_sub_ = this->create_subscription<std_msgs::msg::String>(
            "/robot_description",
            qos_profile,
            std::bind(&ScaraKinematics::urdf_callback, this, _1)
        );

        goal_sub_ = this->create_subscription<geometry_msgs::msg::Pose> (
            "/scara_ik_goal",
            10,
            std::bind(&ScaraKinematics::goal_callback, this, _1)
        );

        joint_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray> (
            "/arm_controller/commands",
            10
        );
        RCLCPP_INFO(this->get_logger(), "SCARA IK Node initialized.");
    }


private:
    struct IKResult {
        double joint1;
        double joint2;
        double joint3;
        double joint4;
        bool success;
        IKResult(double j1, double j2, double j3, double j4, bool result) 
        : joint1(j1), joint2(j2), joint3(j3), joint4(j4), success(result) {};
    };
    
    IKResult compute_ik_from_pose(const geometry_msgs::msg::Pose &goal_pose) {

        double x = goal_pose.position.x;
        double y = goal_pose.position.y;
        double z = goal_pose.position.z;
        double joint1 = lower_limits_.at(0) + z;

        double r2 = x * x + y * y;
        double cos_theta3 = (r2 - j2_length * j2_length - j3_length * j3_length) / (2 * j2_length * j3_length);
        if (std::abs(cos_theta3) > 1.0) return {0, 0, 0, 0, false};

        double theta3 = std::acos(cos_theta3);
        double theta2 = std::atan2(y, x) - std::atan2(j3_length * std::sin(theta3), j2_length + j3_length * std::cos(theta3));

        Eigen::Quaterniond q(goal_pose.orientation.w, goal_pose.orientation.x,
                             goal_pose.orientation.y, goal_pose.orientation.z);
        Eigen::Matrix3d rot = q.normalized().toRotationMatrix();

        //double roll = std::atan2(rot(2, 1), rot(2, 2));
        //double pitch = std::atan2(-rot(2, 0), std::sqrt(rot(2, 1) * rot(2, 1) + rot(2, 2) * rot(2, 2)));
        double yaw = std::atan2(rot(1, 0), rot(0, 0));
        double joint4 = yaw;

        return IKResult(joint1, theta2, theta3, joint4, true);
    }

    void urdf_callback(const std_msgs::msg::String::SharedPtr msg) {
        urdf::Model model;

        if(!model.initString(msg->data)) {
            RCLCPP_ERROR(this->get_logger(), "Failed to parse robot URDF!");
            return;
        }
        //TODO: Do I need p.y & p.z?
        if(auto joint2 = model.getJoint("joint_2")) {
            auto p = joint2->parent_to_joint_origin_transform.position;
            //j2_length = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
            j2_length = p.x;
        }

        if(auto joint3 = model.getJoint("joint_3")) {
            auto p = joint3->parent_to_joint_origin_transform.position;
            //j3_length = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
            j3_length = p.x;
        }
        
        if(j2_length == 0.0 || j3_length == 0.0){
            RCLCPP_ERROR(this->get_logger(), "Something went wrong while tryign to extract joint lengths from URDF!");
            return;
        }

        // Read joint limits
        for(const auto &joint : model.joints_) {
            if (!joint.second->name.compare("virtual_joint") || !joint.second->name.compare("joint_5"))
                continue;
            RCLCPP_INFO(this->get_logger(), "%s", joint.second->name.c_str());
            if (joint.second->limits) {
                lower_limits_.push_back(joint.second->limits->lower);
                upper_limits_.push_back(joint.second->limits->upper);
            } else {
                RCLCPP_WARN(this->get_logger(), "Warning! Could not read limits for joint %s!", joint.second->name.c_str());
                lower_limits_.push_back(-1e9);
                upper_limits_.push_back(1e9);
            }
        }

        RCLCPP_INFO(this->get_logger(), "Succesfully received robot description.");
    }

    void goal_callback(const geometry_msgs::msg::Pose::SharedPtr msg) {
        IKResult res = compute_ik_from_pose(*msg);

        if(!res.success) {
            RCLCPP_ERROR(this->get_logger(), "IK Solver failed!");
            return;
        }

        if (res.joint1 < lower_limits_.at(0) || res.joint1 > upper_limits_.at(0)) {
            RCLCPP_ERROR(this->get_logger(), "Joint 1 out of bounds! %f < %f < %f", lower_limits_.at(0), res.joint1, upper_limits_.at(0));
            return;
        }
        if (res.joint2 < lower_limits_.at(1) || res.joint2 > upper_limits_.at(1)) {
            RCLCPP_ERROR(this->get_logger(), "Joint 2 out of bounds! %f < %f < %f", lower_limits_.at(1), res.joint2, upper_limits_.at(1));
            return;
        }
        if (res.joint3 < lower_limits_.at(2) || res.joint3 > upper_limits_.at(2)) {
            RCLCPP_ERROR(this->get_logger(), "Joint 3 out of bounds! %f < %f < %f", lower_limits_.at(2), res.joint3, upper_limits_.at(2));
            return;
        }
        if (res.joint4 < lower_limits_.at(3) || res.joint4 > upper_limits_.at(3)) {
            RCLCPP_ERROR(this->get_logger(), "Joint 4 out of bounds! %f < %f < %f", lower_limits_.at(3), res.joint4, upper_limits_.at(3));
            return;
        }
                

        std_msgs::msg::Float64MultiArray out;
        out.data = {res.joint1, res.joint2, res.joint3, res.joint4};
        joint_pub_->publish(out);
        RCLCPP_INFO(this->get_logger(), "Published joint targers.");
    }

    double j2_length, j3_length;
    std::vector<double> lower_limits_, upper_limits_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr goal_sub_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr desc_sub_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr  joint_pub_;
};

int main (int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ScaraKinematics>());
    rclcpp::shutdown();
    return 0;
}