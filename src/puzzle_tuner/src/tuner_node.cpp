#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/bool.hpp>
#include <sensor_msgs/msg/image.hpp>

#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <fstream>
#include <ament_index_cpp/get_package_share_directory.hpp> // For getting package paths

class TunerNode : public rclcpp::Node {

public:
    TunerNode() : rclcpp::Node("tuner_node") {
        camera_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/image_raw",
            1,
            std::bind(&TunerNode::camera_callback, this, std::placeholders::_1)
        );
    }

private:
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr pub_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr camera_sub_;
    bool captured_ = false;
    cv::Mat image_;
    std::mutex image_mutex;

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
    const char* cornersWindowName = "Adjusting corner detection";

    void camera_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        try {
            if(!captured_)
            {
                image_ = cv_bridge::toCvCopy(msg, "bgr8")->image.clone();
                captured_ = true;
                std::thread(std::bind(&adjust_proc_params, this)).detach();
            }
        } 
        catch (cv_bridge::Exception &e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
            return;
        }
    }

    void adjust_proc_params() {
        loadParameters()
    }

    void loadParameters() {
        std::ifstream file(configFile);
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
                    std::cout << "Error parsing parameters, using initial values" << std::endl;
                    return;
                }
                else {
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
            std::cerr << "No config file found. Using default parameters." << std::endl;
        }
    }

    void updateImage(int, void*){
        int blurKernel = blurKernelSize * 2 + 1;
        int elemKernel = structElementSize * 2 + 1;
        cv::Mat blured = cv::Mat::zeros(image_.size(), CV_8UC1);
        cv::GaussianBlur(image_, blured, cv::Size(blurKernel, blurKernel), 0);
        cv::threshold(blured, blured, thresholdValue, maxThreshold, cv::THRESH_BINARY_INV);
    
        //Mat element = getStructuringElement(MORPH_RECT, Size(7,7)); // fill small gaps
        cv::morphologyEx(blured, blured,cv::MORPH_CLOSE, getStructuringElement(cv::MORPH_RECT, cv::Size(elemKernel,elemKernel)));
        // processed = blured.clone();
        //cv::imshow(preprocWindowName, processed);
    }    

    void preprocImage(){
        cv::cvtColor(image_, image_, cv::COLOR_BGR2GRAY);
        cv::namedWindow(preprocWindowName, cv::WindowFlags::WINDOW_AUTOSIZE);
        cv::createTrackbar("Blur", preprocWindowName, &blurKernelSize, maxBlur, 
            [](int pos, void* userdata) {
                static_cast<TunerNode*>(userdata)->updateImage(pos, userdata);
            });
        cv::createTrackbar("Binary thresh.", preprocWindowName, &thresholdValue, maxThreshold, 
            [](int pos, void* userdata) {
                static_cast<TunerNode*>(userdata)->updateImage(pos, userdata);
            });
        cv::createTrackbar("Struct elem.", preprocWindowName, &structElementSize, maxStructElementSize,
            [](int pos, void* userdata) {
                static_cast<TunerNode*>(userdata)->updateImage(pos, userdata);
            });
        updateImage(0,0);
    }

};


int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TunerNode>();
    rclcpp::spin(node); 
    rclcpp::shutdown();
    return 0;
}