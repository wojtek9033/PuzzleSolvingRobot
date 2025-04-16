#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.hpp>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/bool.hpp>
#include <sensor_msgs/msg/image.hpp>

class CameraNode : public rclcpp::Node{
public:
    CameraNode() : Node("camera_node"){
        RCLCPP_INFO(this->get_logger(), "Camera node started.");
        camera_subscriber_ = this->create_subscription<std_msgs::msg::Bool>(
            "/robot_in_pos",
            10,
            std::bind(&CameraNode::camera_callback, this, std::placeholders::_1)
        );
        camera_publisher_ = this->create_publisher<sensor_msgs::msg::Image>(
            "/image_raw", 
            10
        );

        cap_.open(deviceID, apiID);
        if(!cap_.isOpened()){
            RCLCPP_FATAL(this->get_logger(), "Error! Could not open camera!");
            rclcpp::shutdown();
            return;
        } else {
            RCLCPP_INFO(this->get_logger(), "Successfully opened camera");
        }
    }

    // callback function to trigger camera when robot is in position
    void camera_callback(const std_msgs::msg::Bool::SharedPtr msg){
        if(msg->data){
            if(!cap_.isOpened()){
                RCLCPP_FATAL(this->get_logger(), "Error! Camera shutdown during operation!");
                rclcpp::shutdown();
                return;
            } else {
                cv::Mat frame;
                cap_ >> frame;
    
                if(frame.empty()){
                    RCLCPP_ERROR(this->get_logger(), "Error! Captured empty frame!");
                } else {
                    try {
                        auto img_msg = cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", frame).toImageMsg();
                        img_msg->header.stamp = this->now();
                        img_msg->header.frame_id = "camera_frame";
                        camera_publisher_->publish(*img_msg);
                    } catch (cv_bridge::Exception &e) {
                        RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
                        return;
                    }
                    RCLCPP_INFO(this->get_logger(), "Captured and published image.");
                }
            }
        }
    }
private:

    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr camera_subscriber_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr camera_publisher_;
    cv::VideoCapture cap_;
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
};


int main(int argc, char** argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CameraNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
