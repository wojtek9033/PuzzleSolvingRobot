#include <cmath>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <urdf/model.h>
#include <tf2/LinearMath/Quaternion.hpp>
#include <tf2/LinearMath/Matrix3x3.hpp>
#include <yaml-cpp/yaml.h>
#include <ament_index_cpp/get_package_share_directory.hpp>


class ScaraKinematics : public rclcpp::Node {
public:
    ScaraKinematics() : Node("scara_kinematics_node") {
        using std::placeholders::_1;

        this->declare_parameter<bool>("is_sim", false);
        is_sim_ = this->get_parameter("is_sim").as_bool();

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
        filepath_ = ament_index_cpp::get_package_share_directory("scara_kinematics") + "/config/initial_positions.yaml";
        load_initial_positions(filepath_);

        if (is_sim_) {
            init_pos_timer_ = this->create_wall_timer(
                std::chrono::seconds(5),
                std::bind(&ScaraKinematics::timer_callback, this)
            );
        }

        RCLCPP_INFO(this->get_logger(), "SCARA IK Node initialized");
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
    
    void timer_callback() {
        std_msgs::msg::Float64MultiArray msg;
        msg.data = {
            initial_joint_positions_.at(0),
            initial_joint_positions_.at(1),
            initial_joint_positions_.at(2),
            initial_joint_positions_.at(3)};
        init_pos_timer_->cancel();
        RCLCPP_INFO(this->get_logger(), "Sent initial joint positions. (simulation)");
    }

    void load_initial_positions(const std::string &filepath) {

        YAML::Node config;
        try {
            config = YAML::LoadFile(filepath);
        } catch (const YAML::BadFile& e) {
            RCLCPP_ERROR(this->get_logger(), "Could not open file YAML: %s", filepath.c_str());
            throw;
        }

        if(!config["initial_positions"]) {
            RCLCPP_ERROR(this->get_logger(), "initial_positions not found in YAML file.");
            rclcpp::shutdown();
        }
        
        for (const auto &it : config["initial_positions"]) {
            initial_joint_positions_.push_back(it.second.as<double>());
        }
        RCLCPP_INFO(this->get_logger(), "Succesfully read robot initial positions.");
    }

    IKResult compute_ik_from_pose(const geometry_msgs::msg::Pose &goal_pose) {

        double j2_offset = joint_length_.at(1);
        double j3_length = joint_length_.at(2);
        double j4_length = joint_length_.at(3);

        double x = goal_pose.position.x;
        double y = goal_pose.position.y;
        double z = goal_pose.position.z;

        double j1_goal = z;

        double dx = x - j2_offset;
        double r2 = dx * dx + y * y;
        double cos_theta3 = (r2 - j3_length * j3_length - j4_length * j4_length) / (2 * j3_length * j4_length);
        if (std::abs(cos_theta3) > 1.0) 
            return {0.0, 0.0, 0.0, 0.0, false};

        double sin_theta3 = std::sqrt(1.0 - cos_theta3 * cos_theta3);
        if (y < 0.0)
            sin_theta3 = -sin_theta3;  // elbow-down

        double theta2 = std::atan2(y, dx) - std::atan2(j4_length * sin_theta3, j3_length + j4_length * cos_theta3);
        double theta3 = std::atan2(sin_theta3, cos_theta3);

        tf2::Quaternion q(
            goal_pose.orientation.x,
            goal_pose.orientation.y,
            goal_pose.orientation.z,
            goal_pose.orientation.w);
        double roll, pitch, yaw;
        tf2::Matrix3x3(q).getRPY(roll, pitch, yaw);
        //double theta4 = yaw - (theta2 + theta3);
        double theta4 = yaw;
        // In real robot, elbow and shoulder engines are mounted in the base
        // so in the theta3 we compensate theta2 movement by the elbow reductions mounted in shoulder
        if (!is_sim_) 
            theta3 += (shoulder_gear_no_/elbow_gear_no_) * theta2;

        return IKResult(j1_goal, theta2, theta3 , theta4, true);
    }

    void urdf_callback(const std_msgs::msg::String::SharedPtr msg) {
        urdf::Model model;

        if(!model.initString(msg->data)) {
            RCLCPP_ERROR(this->get_logger(), "Failed to parse robot URDF!");
            return;
        }

        int counter{0};
        for(auto const& [joint_name, joint_ptr] : model.joints_) {
            if (!joint_ptr.get()->parent_link_name.compare("world"))
                continue;
            
            if (joint_ptr->limits) {
                lower_limits_.push_back(joint_ptr->limits->lower);
                upper_limits_.push_back(joint_ptr->limits->upper);
            } else {
                RCLCPP_WARN(this->get_logger(), "Could not read limits for joint %s! Solver will continue with large limits.", joint_name.c_str());
                lower_limits_.push_back(-1e9);
                upper_limits_.push_back(1e9);
            }

            // Read length of each joint
            double len = joint_ptr->parent_to_joint_origin_transform.position.x;
            if (len == 0.0)
                counter++;
            joint_length_.push_back(len);
        }

        if(joint_length_.empty()) {
            RCLCPP_ERROR(this->get_logger(), "Something went wrong while tryign to extract joint lengths from URDF!");
            return;
        }

        if(counter == joint_length_.size()) {
            RCLCPP_ERROR(this->get_logger(), "All joint lengths are 0! IK Solver will fail.");
            return;
        }

        for(size_t i = 0; i < joint_length_.size(); i++)
            RCLCPP_INFO(this->get_logger(), "Joint %li len: %f lower: %f upper:%f ", i + 1, joint_length_.at(i), lower_limits_.at(i), upper_limits_.at(i));

        RCLCPP_INFO(this->get_logger(), "Succesfully received robot description.");
    }

    void goal_callback(const geometry_msgs::msg::Pose::SharedPtr msg) {
        IKResult res = compute_ik_from_pose(*msg);

        if(!res.success) {
            RCLCPP_ERROR(this->get_logger(), "IK Solver failed!");
            return;
        }

        if (res.joint1 < lower_limits_.at(0) || res.joint1 > upper_limits_.at(0)) {
            RCLCPP_ERROR(this->get_logger(), "Joint 1 out of bounds! %f < %f < %f", lower_limits_.at(0) * (180/M_PI), res.joint1 * (180/M_PI), upper_limits_.at(0) * (180/M_PI));
            return;
        }
        if (res.joint2 < lower_limits_.at(1) || res.joint2 > upper_limits_.at(1)) {
            RCLCPP_ERROR(this->get_logger(), "Joint 2 out of bounds! %f < %f < %f", lower_limits_.at(1) * (180/M_PI), res.joint2 * (180/M_PI), upper_limits_.at(1) * (180/M_PI));
            return;
        }
        if (res.joint3 < lower_limits_.at(2) || res.joint3 > upper_limits_.at(2)) {
            RCLCPP_ERROR(this->get_logger(), "Joint 3 out of bounds! %f < %f < %f", lower_limits_.at(2) * (180/M_PI), res.joint3 * (180/M_PI), upper_limits_.at(2) * (180/M_PI));
            return;
        }
        if (res.joint4 < lower_limits_.at(3) || res.joint4 > upper_limits_.at(3)) {
            RCLCPP_ERROR(this->get_logger(), "Joint 4 out of bounds! %f < %f < %f", lower_limits_.at(3) * (180/M_PI), res.joint4 * (180/M_PI), upper_limits_.at(3) * (180/M_PI));
            return;
        }
                

        std_msgs::msg::Float64MultiArray out;
        if (is_sim_) {
            out.data = {
                res.joint1,
                res.joint2,
                res.joint3,
                res.joint4};
        }
        else {
            out.data = {
                res.joint1 - initial_joint_positions_.at(0),
                res.joint2 - initial_joint_positions_.at(1),
                res.joint3 - ((shoulder_gear_no_/elbow_gear_no_) * initial_joint_positions_.at(2)),
                res.joint4 - initial_joint_positions_.at(3)};
        }

        joint_pub_->publish(out);
        RCLCPP_INFO(this->get_logger(), "Published joint targers.");
    }
    bool is_sim_;
    double shoulder_gear_no_{33.0};
    double elbow_gear_no_{62.0};
    std::string filepath_;
    std::vector<double> initial_joint_positions_, joint_length_, lower_limits_, upper_limits_;
    rclcpp::TimerBase::SharedPtr init_pos_timer_;
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