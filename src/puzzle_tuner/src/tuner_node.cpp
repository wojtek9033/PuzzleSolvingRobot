#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <std_msgs/msg/bool.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>

#include <chrono>

#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>

#include "scara_msgs/action/scara_task.hpp"

using namespace std::chrono_literals;
using namespace cv;
using ClientGoalHandle = rclcpp_action::ClientGoalHandle<scara_msgs::action::ScaraTask>;

class TunerNode : public rclcpp::Node {

public:
    TunerNode()
        :   rclcpp::Node("tuner_node"),
            MAX_BLUR_(10),
            MAX_THRESHOLD_(255),
            MAX_STRUT_ELEM_SIZE_(21),
            MAX_CORNERS_BLOCK_SIZE_(12),
            MAX_CORNERS_KSIZE_(5),
            MAX_CORNERS_TRESH_VAL_(255),
            scale_down_(0.4),
            cornersK_(0.04),
            blur_kernel_size_(1),
            threshold_value_(128),
            struct_element_size_(1),
            corners_block_size_(7), // corner neighbourhood
            corners_ksize_(1), // higher = edge is more blurry
            corners_tresh_val_(190),
            trigger_capture_(false),
            robot_in_pos_(false),
            preproc_window_name_("Adjusting preprocessing."),
            corners_window_name_("Adjusting corners detection.")
            {

        client_ = rclcpp_action::create_client<scara_msgs::action::ScaraTask>(this, "scara_task");
        timer_ = create_wall_timer(5s, std::bind(&TunerNode::timer_callback, this));

        camera_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/image_raw",
            10,
            std::bind(&TunerNode::camera_callback, this, std::placeholders::_1)
        );

        config_filepath_ = ament_index_cpp::get_package_share_directory("puzzle_tuner") + "/config/solver_config.txt";

        load_parameters(config_filepath_);
        RCLCPP_INFO(this->get_logger(), "Puzzle tuner node initialized. Waiting for task server to become available...");
    }

private:
    rclcpp_action::Client<scara_msgs::action::ScaraTask>::SharedPtr client_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr pub_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr camera_sub_;
    cv::Mat src_, temp_, processed_;
    std::string config_filepath_;
    
    const int MAX_BLUR_, MAX_THRESHOLD_, MAX_STRUT_ELEM_SIZE_, MAX_CORNERS_BLOCK_SIZE_, MAX_CORNERS_KSIZE_, MAX_CORNERS_TRESH_VAL_;
    double scale_down_, cornersK_;
    int blur_kernel_size_, threshold_value_, struct_element_size_, corners_block_size_, corners_ksize_, corners_tresh_val_;
    bool trigger_capture_, robot_in_pos_;
    const char* preproc_window_name_, *corners_window_name_;
    
    void timer_callback() {
        timer_->cancel();

        if (!client_->wait_for_action_server(10s)) {
            RCLCPP_ERROR(this->get_logger(), "Action server not available after waiting!");
            rclcpp::shutdown();
            return;
        }
        auto goal_msg = scara_msgs::action::ScaraTask::Goal();
        goal_msg.command = "calibrate";
        RCLCPP_INFO(this->get_logger(), "Tuner client sending calibration request");

        auto send_goal_options = rclcpp_action::Client<scara_msgs::action::ScaraTask>::SendGoalOptions();
        send_goal_options.goal_response_callback = std::bind(&TunerNode::goal_callback, this, std::placeholders::_1);
        send_goal_options.result_callback = std::bind(&TunerNode::result_callback, this, std::placeholders::_1);

        client_->async_send_goal(goal_msg, send_goal_options);
    }

    void goal_callback (const ClientGoalHandle::SharedPtr &goal_handle) {
        if (!goal_handle)
            RCLCPP_ERROR(this->get_logger(), "Goal was rejected by server");
        else
            RCLCPP_INFO(this->get_logger(), "Calibration goal accepted by server, waiting for robot...");
    }

    void result_callback (const ClientGoalHandle::WrappedResult &result) {
        switch (result.code) {
            case rclcpp_action::ResultCode::SUCCEEDED:
                RCLCPP_INFO(this->get_logger(), "Request succeeded. Attempting to capture image...");
                trigger_capture_ = true;
                return;
            case rclcpp_action::ResultCode::ABORTED:
                RCLCPP_ERROR(get_logger(), "Calibration was aborted");
                return;
            case rclcpp_action::ResultCode::CANCELED:
                RCLCPP_ERROR(get_logger(), "Calibration was canceled");
                return;
            default:
                RCLCPP_ERROR(get_logger(), "Unknown result code");
                return;
            }
        
        robot_in_pos_ = result.result->success;
    }

    void camera_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        try {
            if(trigger_capture_)
            {
                src_ = cv_bridge::toCvCopy(msg, "bgr8")->image.clone();
                trigger_capture_ = false;
                RCLCPP_INFO(this->get_logger(), "Tuner succesfully captured camera image.");

                std::thread gui_thread([this]() {
                    this->adjust_proc_params();
                });
                gui_thread.detach();
            }
        } 
        catch (cv_bridge::Exception &e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
            return;
        }
    }

    void adjust_proc_params() {
        if (src_.empty()) {
            RCLCPP_ERROR(this->get_logger(), "Captured image is empty! Cannot adjust processing parameters.");
            return;
        }
        RCLCPP_INFO(this->get_logger(), "Parameters adjusting procedure started.");
        temp_ = src_.clone();

        preproc_image();
        while (true){
            char key = (char)cv::waitKey(0);
            if (key == 's') {
                save_parameters(config_filepath_);
                break;
            }
            if (key == 27) break; // ESC key
        }
        destroyAllWindows();
        adjust_corners();
        while (true){
            char key = (char)cv::waitKey(0);
            if (key == 's') {
                save_parameters(config_filepath_);
                break;
            }
            if (key == 27) break; // ESC key
        }
        destroyAllWindows();
        RCLCPP_INFO(this->get_logger(), "Parameters adjusting procedure ended.");
    }

    void load_parameters(const std::string& path) {
        std::ifstream file(path);
        int temp_blur_kernel_size, temp_threshold_value, temp_struct_element_size, temp_corners_block_size, temp_corners_K_size, temp_corners_tresh_val;
        if (file.is_open()) {
            std::string line;
            if (std::getline(file, line)){
                std::stringstream ss(line);
                char delim;
                if (!(ss >> temp_blur_kernel_size >> delim >> temp_threshold_value >> delim >> temp_struct_element_size
                         >> delim  >> temp_corners_block_size >> delim >>temp_corners_K_size >> delim >> temp_corners_tresh_val
                         >> delim >> scale_down_)) {
                    RCLCPP_WARN(this->get_logger(), "Could not parse parameters, using initial values.");
                    return;
                }
                else {
                    RCLCPP_INFO(this->get_logger(), "Succesfully read solver parameters from config file.");
                    blur_kernel_size_ = (temp_blur_kernel_size-1)/2;
                    threshold_value_ = temp_threshold_value;
                    struct_element_size_ = (temp_struct_element_size-1)/2;
                    corners_block_size_ = temp_corners_block_size/2;
                    corners_ksize_ = (temp_corners_K_size-1)/2;
                    corners_tresh_val_ = temp_corners_tresh_val;
                }
            }
            file.close();
        } else {
            RCLCPP_WARN(this->get_logger(), "No config file found. Using default parameters!");
        }
    }

    void save_parameters(const std::string& path) {
    std::ofstream file(path);
        if(file.is_open()){
            file << (blur_kernel_size_ * 2 + 1) << "," << threshold_value_ << "," << (struct_element_size_  * 2 + 1) << ","
                << corners_block_size_ * 2 << "," << corners_ksize_ * 2 +1 << "," << corners_tresh_val_ << ","
                << scale_down_ << std::endl;
            file.close();
            RCLCPP_INFO(this->get_logger(), "Parameters saved to configFile.");
        } else {
            RCLCPP_ERROR(this->get_logger(), "Error saving parameters!");
        }
    }

    static void preproc_on_trackbar(int, void* userdata) {
        auto *self = static_cast<TunerNode*>(userdata);
        self->update_image();
    }

    void preproc_image() {
        cvtColor(temp_,temp_,COLOR_BGR2GRAY);
        namedWindow(preproc_window_name_, WINDOW_AUTOSIZE);
        createTrackbar("Blur", preproc_window_name_, &blur_kernel_size_, MAX_BLUR_, preproc_on_trackbar, this);
        createTrackbar("Binary thresh.", preproc_window_name_, &threshold_value_, MAX_THRESHOLD_, preproc_on_trackbar, this);
        createTrackbar("Struct elem.", preproc_window_name_, &struct_element_size_, MAX_STRUT_ELEM_SIZE_, preproc_on_trackbar, this);
        preproc_on_trackbar(0, this);
    }

    static void corners_on_trackbar(int, void* userdata) {
        auto *self = static_cast<TunerNode*>(userdata);
        self->get_corners();
    }

    void adjust_corners() {

        namedWindow(corners_window_name_, WINDOW_AUTOSIZE);
        createTrackbar("Block Size", corners_window_name_, &corners_block_size_, MAX_CORNERS_BLOCK_SIZE_, corners_on_trackbar, this);
        createTrackbar("KSize", corners_window_name_, &corners_ksize_, MAX_CORNERS_KSIZE_, corners_on_trackbar, this);
        createTrackbar("Treshold", corners_window_name_, &corners_tresh_val_, MAX_CORNERS_TRESH_VAL_, corners_on_trackbar, this);
        corners_on_trackbar(0, this);
    }

    void update_image() {
        int blurKernel = blur_kernel_size_ * 2 + 1;
        int elemKernel = struct_element_size_ * 2 + 1;
        cv::Mat blured = cv::Mat::zeros(temp_.size(), CV_8UC1);
        cv::GaussianBlur(temp_, blured, cv::Size(blurKernel, blurKernel), 0);
        cv::threshold(blured, blured, threshold_value_, MAX_THRESHOLD_, cv::THRESH_BINARY_INV);
        cv::morphologyEx(blured, blured,cv::MORPH_CLOSE, getStructuringElement(cv::MORPH_RECT, cv::Size(elemKernel,elemKernel)));
        processed_ = blured.clone();
        cv::imshow(preproc_window_name_, processed_);
    }    

    void get_corners() {

        int block_size = corners_block_size_ * 2;
        int k_size = corners_ksize_ * 2 + 1;

        Mat corners = Mat::zeros( processed_.size(), CV_32FC1 );
        Mat corners_tresholded = Mat::zeros( processed_.size(), CV_32FC1 );

        cornerHarris( processed_, corners, block_size, k_size, cornersK_ );
        Mat corners_norm, corners_scaled;
        normalize( corners, corners_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
        convertScaleAbs( corners_norm, corners_scaled );

        // apply tresholding
        for (int i = 0; i < corners_norm.rows; i++) {
            for (int j = 0; j < corners_norm.cols; j++) {
                if (static_cast<int>(corners_norm.at<float>(i, j)) > corners_tresh_val_) {
                    corners_tresholded.at<float>(i, j) = static_cast<int>(corners_norm.at<float>(i, j));
                }
            }
        }

        Mat extracted_corners = processed_.clone();
        cvtColor(extracted_corners, extracted_corners, COLOR_GRAY2BGR);

        for (int i = 0; i < corners_tresholded.rows; i++){
            for (int j = 0; j < corners_tresholded.cols; j++){
                if (corners_tresholded.at<float>(i,j) != 0){
                    circle(extracted_corners, cv::Point(j,i), 1, cv::Scalar(0, 0, 255),2);
                }
            }
        }
        imshow(corners_window_name_,extracted_corners);
        waitKey(0);
    }
};


int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TunerNode>();
    rclcpp::spin(node); 
    rclcpp::shutdown();
    return 0;
}