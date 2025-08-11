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
    TunerNode() : rclcpp::Node("tuner_node") {

        client_ = rclcpp_action::create_client<scara_msgs::action::ScaraTask>(this, "scara_task");
        timer_ = create_wall_timer(1s, std::bind(&TunerNode::timer_callback, this));

        camera_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/image_raw",
            1,
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
    bool captured_ = false;
    bool robot_in_pos_ = false;
    cv::Mat src_, temp_, processed_;
    std::mutex image_mutex;
    
    std::string config_filepath_;
    double scale_down = 0.4;
    int blurKernelSize{1};
    int thresholdValue{128};
    int structElementSize{1};
    int cornersBlockSize{7}; // corner neighbourhood
    int cornersKSize{1}; // higher = edge is more blurry
    double cornersK{0.04};
    int cornersTreshVal{190};

    const int maxBlur = 10;
    const int maxThreshold = 255;
    const int maxStructElementSize = 21;
    const int maxCornersBlockSize = 12;
    const int maxCornersKSize = 5;
    const int maxCornersTreshVal{255};

    const char* preprocWindowName = "Adjusting preprocessing";
    const char* cornersWindowName = "";
    
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
                break;
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
        preprocImage();
    }

    void camera_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        try {
            if(!captured_)
            {
                src_ = cv_bridge::toCvCopy(msg, "bgr8")->image.clone();
                captured_ = true;
                adjust_proc_params();
            }
        } 
        catch (cv_bridge::Exception &e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
            return;
        }
    }

    void adjust_proc_params() {
        temp_ = src_.clone();

        preprocImage();
        while (true){
            char key = (char)cv::waitKey(0);
            if (key == 's') {
                saveParameters();
                break;
            }
            if (key == 27) break;
        }
        adjustCorners();
        while (true){
            char key = (char)cv::waitKey(0);
            if (key == 's') {
                saveParameters();
                break;
            }
            if (key == 27) break;
        }


        destroyAllWindows();
    }

    void load_parameters(const std::string& path) {
        std::ifstream file(path);
        int tempBlurKernelSize;
        int tempThresholdValue;
        int tempStructElementSize;
        int tempcornersBlockSize;
        int tempcornersKSize;
        int tempcornersTreshVal;
        if (file.is_open()) {
            std::string line;
            if (std::getline(file, line)){
                std::stringstream ss(line);
                char delim;
                if (!(ss >> tempBlurKernelSize >> delim >> tempThresholdValue >> delim >> tempStructElementSize
                         >> delim  >> tempcornersBlockSize >> delim >>tempcornersKSize >> delim >> tempcornersTreshVal
                         >> delim >> scale_down)) {
                    RCLCPP_WARN(this->get_logger(), "Could not parse parameters, using initial values");
                    return;
                }
                else {
                    RCLCPP_INFO(this->get_logger(), "Succesfully read solver parameters from config file.");
                    blurKernelSize = (tempBlurKernelSize-1)/2;
                    thresholdValue = tempThresholdValue;
                    structElementSize = (tempStructElementSize-1)/2;
                    cornersBlockSize = tempcornersBlockSize/2;
                    cornersKSize = (tempcornersKSize-1)/2;
                    cornersTreshVal = tempcornersTreshVal;
                }
            }
            file.close();
        } else {
            RCLCPP_WARN(this->get_logger(), "No config file found. Using default parameters!");
        }
    }

    void saveParameters() {
    std::ofstream file("src/configFile");
    if(file.is_open()){
        file << (blurKernelSize * 2 + 1) << "," << thresholdValue << "," << (structElementSize  * 2 + 1) << ","
             << cornersBlockSize * 2 << "," << cornersKSize * 2 +1 << "," << cornersTreshVal << ","
             << scale_down << std::endl;
        file.close();
        RCLCPP_INFO(this->get_logger(), "Parameters saved to configFile");
    } else {
        RCLCPP_ERROR(this->get_logger(), "@@@ Error saving parameters!");
    }
    }

    static void preprocOnTrackbar(int, void* userdata) {
        auto *self = static_cast<TunerNode*>(userdata);
        self->updateImage();
    }

    void preprocImage() {
        cvtColor(temp_,temp_,COLOR_BGR2GRAY);
        namedWindow(preprocWindowName, WINDOW_AUTOSIZE);
        createTrackbar("Blur", preprocWindowName, &blurKernelSize, maxBlur, preprocOnTrackbar);
        createTrackbar("Binary thresh.", preprocWindowName, &thresholdValue, maxThreshold, preprocOnTrackbar);
        createTrackbar("Struct elem.", preprocWindowName, &structElementSize, maxStructElementSize, preprocOnTrackbar);
        preprocOnTrackbar(0,0);
    }

    static void cornersOnTrackbar(int, void* userdata) {
        auto *self = static_cast<TunerNode*>(userdata);
        self->getCorners();
    }

    void adjustCorners() {

        namedWindow(cornersWindowName, WINDOW_AUTOSIZE);
        createTrackbar("Block Size", cornersWindowName, &cornersBlockSize, maxCornersBlockSize, cornersOnTrackbar);
        createTrackbar("KSize", cornersWindowName, &cornersKSize, maxCornersKSize, cornersOnTrackbar);
        createTrackbar("Treshold", cornersWindowName, &cornersTreshVal, maxCornersTreshVal, cornersOnTrackbar);
        cornersOnTrackbar(0,0);
    }

    void updateImage() {
        int blurKernel = blurKernelSize * 2 + 1;
        int elemKernel = structElementSize * 2 + 1;
        cv::Mat blured = cv::Mat::zeros(temp_.size(), CV_8UC1);
        cv::GaussianBlur(temp_, blured, cv::Size(blurKernel, blurKernel), 0);
        cv::threshold(blured, blured, thresholdValue, maxThreshold, cv::THRESH_BINARY_INV);
        cv::morphologyEx(blured, blured,cv::MORPH_CLOSE, getStructuringElement(cv::MORPH_RECT, cv::Size(elemKernel,elemKernel)));
        cv::Mat processed = blured.clone();
        cv::imshow(preprocWindowName, processed);
    }    

    void getCorners() {

        int blockSize = cornersBlockSize * 2;
        int kSize = cornersKSize * 2 + 1;

        Mat corners = Mat::zeros( processed_.size(), CV_32FC1 );
        Mat cornersTresholded = Mat::zeros( processed_.size(), CV_32FC1 );

        cornerHarris( processed_, corners, blockSize, kSize, cornersK );
        Mat corners_norm, corners_scaled;
        normalize( corners, corners_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
        convertScaleAbs( corners_norm, corners_scaled );

        // apply tresholding
        for (int i = 0; i < corners_norm.rows; i++) {
            for (int j = 0; j < corners_norm.cols; j++) {
                if (static_cast<int>(corners_norm.at<float>(i, j)) > cornersTreshVal) {
                    cornersTresholded.at<float>(i, j) = static_cast<int>(corners_norm.at<float>(i, j));
                }
            }
        }

        Mat extractedCorners = processed_.clone();
        cvtColor(extractedCorners, extractedCorners, COLOR_GRAY2BGR);

        for (int i = 0; i < cornersTresholded.rows; i++){
            for (int j = 0; j < cornersTresholded.cols; j++){
                if (cornersTresholded.at<float>(i,j) != 0){
                    circle(extractedCorners, cv::Point(j,i), 1, cv::Scalar(0, 0, 255),2);
                }
            }
        }
        imshow(cornersWindowName,extractedCorners);
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