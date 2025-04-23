#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/bool.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.hpp>

#include <libcamera/stream.h>
#include "core/rpicam_app.hpp"
#include "core/still_options.hpp"
#include "image/image.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <ctime>
#include <opencv2/opencv.hpp>

using std::placeholders::_1;

class CameraNode : public rclcpp::Node {
public:
    CameraNode() : Node("camera_node") {
        RCLCPP_INFO(this->get_logger(), "Camera node started.");
        capture_sub_ = this->create_subscription<std_msgs::msg::Bool>(
            "/camera/capture",
             10,
            std::bind(&CameraNode::capture_callback, this, _1)
        );

        image_pub_ = this->create_publisher<sensor_msgs::msg::Image>("/camera/image", 10);
        app_ = std::make_unique<RPiCamApp>(std::make_unique<StillOptions>());
    }
private:
    void capture_callback(const std_msgs::msg::Bool::SharedPtr msg){
        if (!msg->data)
            return;
        RCLCPP_INFO(this->get_logger(), "Capture command received.");
        bool camera_started = false;

        try{
            StillOptions *options = static_cast<StillOptions *>(app_->GetOptions());
            options->timeout.value = std::chrono::milliseconds(1);
            options->nopreview = true;
            options->output.clear(); // not write to a file
            options->denoise = "off"; // or try "cdn_off"

            app_->OpenCamera();
            app_->ConfigureStill();
            app_->StartCamera();
            camera_started = true;

            while(true){
                RPiCamApp::Msg msg = app_->Wait();
                if (msg.type == RPiCamApp::MsgType::RequestComplete){
                    RCLCPP_INFO(this->get_logger(), "Image captured. ");

                    libcamera::Stream *stream = app_->StillStream();
                    StreamInfo info = app_->GetStreamInfo(stream);
                    CompletedRequestPtr &payload = std::get<CompletedRequestPtr>(msg.payload);
                    BufferReadSync r(app_.get(), payload->buffers[stream]);
                    const auto mem = r.Get();

                    // Convert raw image buffer to OpenCV Mat (RGB)
                    // cv::Mat image(info.height, info.width, CV_8UC3, mem[0].data());
                    
                    // Assuming libcamera gives YUV420 and we want RGB
                    cv::Mat yuv_image(info.height * 3 / 2, info.width, CV_8UC1, mem[0].data());
                    cv::Mat rgb_image;
                    cv::cvtColor(yuv_image, rgb_image, cv::COLOR_YUV2RGB_I420);

                    
                    std_msgs::msg::Header header;  
                    header.stamp = this->now();
                    header.frame_id = "camera_frame";

                    // sensor_msgs::msg::Image::SharedPtr msg = cv_bridge::CvImage(header, "rgb8", image).toImageMsg();
                    // image_pub_->publish(*msg);
                    auto image_msg = cv_bridge::CvImage(header, "rgb8", rgb_image).toImageMsg();
                    image_pub_->publish(*image_msg);
                    
                    RCLCPP_INFO(this->get_logger(), "Image published. %d x %d", image_msg->width, image_msg->height);
                    break;
                }
            }
        }
        catch (const std::exception &e)
        {
            RCLCPP_ERROR(this->get_logger(), "Capture failed: %s", e.what());
        }

            // Always clean up the camera to avoid stuck state
        if (camera_started) {
            try {
                app_->StopCamera();
                app_->Teardown();
            } catch (const std::exception &e) {
                RCLCPP_WARN(this->get_logger(), "Cleanup failed: %s", e.what());
            }
        }
    }

    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr capture_sub_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_pub_;
    std::unique_ptr<RPiCamApp> app_;
};

int main(int argc, char** argv){
    rclcpp::init(argc,argv);
    auto node = std::make_shared<CameraNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}