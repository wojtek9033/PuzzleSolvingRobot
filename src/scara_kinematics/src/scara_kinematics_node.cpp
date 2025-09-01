#include <cmath>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <std_msgs/msg/bool.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <urdf/model.h>
#include <tf2/LinearMath/Quaternion.hpp>
#include <tf2/LinearMath/Matrix3x3.hpp>
#include <yaml-cpp/yaml.h>
#include <ament_index_cpp/get_package_share_directory.hpp>


class ScaraKinematics : public rclcpp::Node {
public:
    ScaraKinematics() 
        :   Node("scara_kinematics_node"),
            shoulder_gear_no_(33.0),
            elbow_gear_no_(62.0)  
            {
                
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

        gripper_on_goal_sub_ = this->create_subscription<geometry_msgs::msg::Pose> (
            "/scara/gripper_on/ik_goal",
            10,
            std::bind(&ScaraKinematics::gripper_on_goal_callback, this, _1)
        );

        gripper_off_goal_sub_ = this->create_subscription<geometry_msgs::msg::Pose> (
            "/scara/gripper_off/ik_goal",
            10,
            std::bind(&ScaraKinematics::gripper_off_goal_callback, this, _1)
        );

        camera_goal_sub_ = this->create_subscription<geometry_msgs::msg::Pose> (
            "/scara/camera/ik_goal",
            10,
            std::bind(&ScaraKinematics::camera_goal_callback, this, _1)
        );

        joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState> (
            "/joint_states",
            10,
            std::bind(&ScaraKinematics::joint_states_callback, this, _1)
        );

        joint_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray> (
            "/arm_controller/commands",
            10
        );

        arm_in_pos_pub_ = this->create_publisher<std_msgs::msg::Bool> (
            "/scara/in_pos",
            10
        );

        config_filepath_ = ament_index_cpp::get_package_share_directory("scara_kinematics") + "/config/initial_positions.yaml";
        load_initial_positions(config_filepath_);

        RCLCPP_INFO(this->get_logger(), "SCARA IK Node initialized");
    }


private:

    double shoulder_gear_no_, elbow_gear_no_, last_joint_1, last_joint_2, last_joint_3, last_joint_4;
    bool is_sim_;

    std::string config_filepath_;
    std::vector<double> initial_joint_positions_, joint_length_, lower_limits_, upper_limits_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr arm_in_pos_pub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr gripper_on_goal_sub_, gripper_off_goal_sub_, camera_goal_sub_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr desc_sub_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr  joint_pub_;
    enum class LimitError : std::uint8_t { None=255, Joint1=0, Joint2=1, Joint3=2, Joint4=3 };
    enum class Effector : std::uint8_t { Gripper=0, Camera=1 };
    enum class Gripper : std::uint8_t { Release=0, Pickup=1, Hold =2};
    static constexpr double RAD2DEG = 180.0 / M_PI;
    
    struct IKResult {
        std::array<double,4> joint_angle;
        std::uint8_t gripper_state;
        bool success;
    };
    IKResult result_;

private:

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

    IKResult compute_ik_from_pose(const geometry_msgs::msg::Pose &goal_pose, Effector effector, Gripper gripper_state = Gripper::Release) {

        double j2_offset = joint_length_.at(1);
        double j3_length = joint_length_.at(2);
        double j4_length = joint_length_.at(3);

        double camera_offset = (effector == Effector::Camera) ? (joint_length_.at(4) - joint_length_.at(3)) : 0.0;
        j4_length += camera_offset;

        double x = goal_pose.position.x;
        double y = goal_pose.position.y;
        double z = goal_pose.position.z;

        double j1_goal = lower_limits_[0] + z;

        double dx = x - j2_offset;
        double r2 = dx * dx + y * y;
        double cos_theta3 = (r2 - j3_length * j3_length - j4_length * j4_length) / (2 * j3_length * j4_length);
        if (std::abs(cos_theta3) > 1.0) 
            return {0.0, 0.0, 0.0, 0.0, 0, false};

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

        IKResult res;
        res.joint_angle[0] = j1_goal;
        res.joint_angle[1] = theta2;
        res.joint_angle[2] = theta3;
        res.joint_angle[3] = theta4;
        res.gripper_state = static_cast<uint8_t>(gripper_state);
        res.success = true;

        return res;
    }

    void urdf_callback(const std_msgs::msg::String::SharedPtr msg) {
        urdf::Model model;

        if(!model.initString(msg->data)) {
            RCLCPP_ERROR(this->get_logger(), "Failed to parse robot URDF!");
            return;
        }

        size_t counter{0};
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

    LimitError check_joint_limits(const IKResult &res, const std::vector<double>& ll, const std::vector<double>& ul) {

        for (size_t i = 0; i < res.joint_angle.size(); i++) {
            if (res.joint_angle[i] < ll.at(i) || res.joint_angle[i] > ul.at(i)) {
                if (i == 0) {
                    RCLCPP_ERROR(this->get_logger(), "Joint %ld out of bounds! %f <= %f <= %f", i, ll.at(i), res.joint_angle[i], ul.at(i));
                } else {
                    RCLCPP_ERROR(this->get_logger(), "Joint %ld out of bounds! %f <= %f <= %f", i, ll.at(i) * RAD2DEG, res.joint_angle[i] * RAD2DEG, ul.at(i) * RAD2DEG);
                }
                return static_cast<LimitError>(i);
            }
        }
        return LimitError::None;
    }

    void publish_joint_angles(const IKResult &res, const std::vector<double>& joint_init_pos) {
        std_msgs::msg::Float64MultiArray out;
            out.data = {
                res.joint_angle[0] - joint_init_pos.at(0),
                res.joint_angle[1] - joint_init_pos.at(1),
                res.joint_angle[2] - (shoulder_gear_no_/elbow_gear_no_) * joint_init_pos.at(2),
                res.joint_angle[3] - joint_init_pos.at(3),
                static_cast<double>(res.gripper_state)};

        joint_pub_->publish(out);
        RCLCPP_INFO(this->get_logger(), "Published joint targers.");
    }

    void gripper_on_goal_callback(const geometry_msgs::msg::Pose::SharedPtr msg) {
        IKResult result_ = compute_ik_from_pose(*msg, Effector::Gripper, Gripper::Pickup);

        if(!result_.success) {
            RCLCPP_ERROR(this->get_logger(), "IK Solver failed!");
            return;
        }

        if(check_joint_limits(result_, lower_limits_, upper_limits_) != LimitError::None) {
            return;
        }

        publish_joint_angles(result_, initial_joint_positions_);
    }

    void gripper_off_goal_callback(const geometry_msgs::msg::Pose::SharedPtr msg) {
        IKResult result_ = compute_ik_from_pose(*msg, Effector::Gripper, Gripper::Release);

        if(!result_.success) {
            RCLCPP_ERROR(this->get_logger(), "IK Solver failed!");
            return;
        }

        if(check_joint_limits(result_, lower_limits_, upper_limits_) != LimitError::None) {
            return;
        }

        publish_joint_angles(result_, initial_joint_positions_);
    }

    void camera_goal_callback(const geometry_msgs::msg::Pose::SharedPtr msg) {
        IKResult result_ = compute_ik_from_pose(*msg, Effector::Camera);

        if(!result_.success) {
            RCLCPP_ERROR(this->get_logger(), "IK Solver failed!");
            return;
        }

        if(check_joint_limits(result_, lower_limits_, upper_limits_) != LimitError::None) {
            return;
        }

        publish_joint_angles(result_, initial_joint_positions_);
    }

    void joint_states_callback(const sensor_msgs::msg::JointState::SharedPtr msg) {

        if (msg->position.at(0) != last_joint_1 ||
            msg->position.at(1) != last_joint_2 ||
            msg->position.at(2) != last_joint_3 ||
            msg->position.at(3) != last_joint_4){
            std_msgs::msg::Bool status;
            status.data = true;
            arm_in_pos_pub_->publish(status);
        }
        last_joint_1 = msg->position.at(0);
        last_joint_2 = msg->position.at(1);
        last_joint_3 = msg->position.at(2);
        last_joint_4 = msg->position.at(3);
    }
};

int main (int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ScaraKinematics>());
    rclcpp::shutdown();
    return 0;
}