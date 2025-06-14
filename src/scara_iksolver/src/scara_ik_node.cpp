#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <std_msgs/msg/string.hpp>

#include <urdf/model.h>
#include <kdl_parser/kdl_parser.hpp>
#include <kdl/tree.hpp>
#include <kdl/chain.hpp>
#include <kdl/chainfksolverpos_recursive.hpp>
#include <kdl/chainiksolverpos_lma.hpp>
#include <kdl/jntarray.hpp>

class ScaraIKNode : public rclcpp::Node {
public:
    ScaraIKNode() : Node("scara_ik_node") {
        using std::placeholders::_1;

        this->declare_parameter<std::string>("base_link", "base_link");
        this->declare_parameter<std::string>("tip_link", "tool0");

        base_link_ = this->get_parameter("base_link").as_string();
        tip_link_ = this->get_parameter("tip_link").as_string();

        rclcpp::QoS qos_profile(rclcpp::KeepLast(1));
        qos_profile.transient_local();

        desc_sub_ = this->create_subscription<std_msgs::msg::String>(
            "/robot_description",
            qos_profile,
            std::bind(&ScaraIKNode::urdf_callback, this, _1)
        );
        
        goal_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
            "ik_goal",
            10,
            std::bind(&ScaraIKNode::goal_callback, this, _1)
        );

        joint_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/arm_controller/commands",
            10
        );

        
        RCLCPP_INFO(this->get_logger(), "SCARA IK Node initialized.");
    }

private:

    void urdf_callback (const std_msgs::msg::String::SharedPtr msg) {
        urdf::Model model;
        if(!model.initString(msg->data)) {
            RCLCPP_ERROR(this->get_logger(), "Failed to parse robot URDF!");
            return;
        }

        // Create a KDL tree based on URDF
        if (!kdl_parser::treeFromUrdfModel(model, kdl_tree_)) {
            RCLCPP_ERROR(this->get_logger(), "Failed to construct KDL tree.");
            return;
        }

        // Create kinematic chain from base_link to tip_link
        if (!kdl_tree_.getChain(base_link_, tip_link_, kdl_chain_)) {
            RCLCPP_ERROR(this->get_logger(), "Failed to extract KDL chain from %s to %s.", base_link_.c_str(), tip_link_.c_str());
            return;
        }

        // Read joint limits from URDF
        for (const auto &segment : kdl_chain_.segments) {
            const auto &joint_name = segment.getJoint().getName();
            auto joint = model.getJoint(joint_name);
            if (joint && joint->limits) {
                lower_limits_.push_back(joint->limits->lower);
                upper_limits_.push_back(joint->limits->upper);
            } else {
                RCLCPP_WARN(this->get_logger(), "Warning! Could not read limits for joint %s!", joint_name.c_str());
                lower_limits_.push_back(-1e9);
                upper_limits_.push_back(1e9);
            }
        }

        fk_solver_ = std::make_shared<KDL::ChainFkSolverPos_recursive>(kdl_chain_);
        ik_solver_ = std::make_shared<KDL::ChainIkSolverPos_LMA>(kdl_chain_);

        q_init_ = KDL::JntArray(kdl_chain_.getNrOfJoints()); // initial configuration buffer
        q_sol_ = KDL::JntArray(kdl_chain_.getNrOfJoints()); // IK solution buffer

        RCLCPP_INFO(this->get_logger(), "KDL chain succesfully initialized.");
        kdl_ready_ = true;

        RCLCPP_INFO(this->get_logger(), "Succesfully received robot description.");
    }

    void goal_callback (const geometry_msgs::msg::Pose::SharedPtr msg) {

        if(!kdl_ready_) {
            RCLCPP_WARN(this->get_logger(), "KDL not ready yet. Waiting for URDF...");
            return; 
        }

        // Create a KDL::Frame from msg
        KDL::Frame target;
        target.p = KDL::Vector(
            msg->position.x,
            msg->position.y,
            msg->position.z
        );
        
        target.M = KDL::Rotation::Quaternion(
            msg->orientation.x,
            msg->orientation.y,
            msg->orientation.z,
            msg->orientation.w
        );

        // Solve IK - positions of joints for target pose
        if(ik_solver_->CartToJnt(q_init_, target, q_sol_) < 0) {
            RCLCPP_ERROR(this->get_logger(), "IK solution failed.");
            return;
        }

        for (size_t i = 0; i < q_sol_.rows(); i++) {
            if (q_sol_(i) < lower_limits_.at(i) || q_sol_(i) > upper_limits_.at(i)) {
                RCLCPP_WARN(this->get_logger(), "Joint %zu out of bounds: %f", i, q_sol_(i));
                return;
            }
        }

        std_msgs::msg::Float64MultiArray joint_msg;
        for (size_t i = 0; i < q_sol_.rows(); i++) {
            joint_msg.data.push_back(q_sol_(i));
        }

        joint_pub_->publish(joint_msg);
        RCLCPP_INFO(this->get_logger(), "Published joint targers.");
    }

    std::string base_link_, tip_link_;
    KDL::Tree kdl_tree_;
    KDL::Chain kdl_chain_;
    KDL::JntArray q_init_, q_sol_;
    std::vector<double> lower_limits_, upper_limits_;

    std::shared_ptr<KDL::ChainFkSolverPos_recursive> fk_solver_;
    std::shared_ptr<KDL::ChainIkSolverPos_LMA> ik_solver_;

    bool kdl_ready_ = false;

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr desc_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr goal_sub_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr joint_pub_;

};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ScaraIKNode>());
    rclcpp::shutdown();
    return 0;
}