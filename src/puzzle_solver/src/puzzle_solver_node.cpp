#include <iostream>
#include <chrono>

#include "puzzle_utils.h"
#include "puzzle_processing.h"
#include "puzzle_matching.h"

#include <rclcpp/rclcpp.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp> // For getting package paths
#include <sensor_msgs/msg/image.hpp> // ROS 2 Image message type
#include <std_msgs/msg/bool.hpp>
#include <cv_bridge/cv_bridge.hpp> // Bridge between ROS 2 and OpenCV

#include "scara_msgs/msg/piece_pose.hpp"

std::map<std::pair<int,int>, std::vector<MatchInfo>>  puzzleMatchInfo;
std::map<std::pair<int,int>, MatchInfo> puzzleBestMatches;

std::string package_path = ament_index_cpp::get_package_share_directory("puzzle_solver");

class PuzzleSolverNode : public rclcpp::Node{
public:
        PuzzleSolverNode(size_t puzzle_size) : Node("puzzle_solver_node"){
            RCLCPP_INFO(this->get_logger(), "Puzzle solver node started.");
            // Subscribe to the topic publishing puzzle piece images
            image_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
                "/camera/image_raw",
                1,
                std::bind(&PuzzleSolverNode::imageCallback, this, std::placeholders::_1)
            );
            robot_sub_ = this->create_subscription<std_msgs::msg::Bool>(
                "/capture/trigger",
                10,
                std::bind(&PuzzleSolverNode::triggerCallback, this, std::placeholders::_1)
            );
            confirm_pub_ = this->create_publisher<std_msgs::msg::Bool> (
                "/capture/confirm",
                10
            );
            assemble_pub_ = this->create_publisher<scara_msgs::msg::PiecePose>(
                "/assembly/solution",
                10
            );
            loadProcessingParameters();
            PUZZLE_SIZE = puzzle_size;
            RCLCPP_INFO(this->get_logger(), "Solver waiting for trigger...");
        }
        ~PuzzleSolverNode(){
            RCLCPP_INFO(this->get_logger(), "Puzzle solver node destroyed.");
        }

private:

    void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg){
        try {
            // Convert a sensor_msgs::Image message to an OpenCV-compatible CvImage
            latest_image_ = cv_bridge::toCvShare(msg, "bgr8")->image.clone();
        }
        catch (cv_bridge::Exception &e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
            return;
        }
    }

    void triggerCallback(const std_msgs::msg::Bool::SharedPtr msg){
        if (!msg->data)
            return;

        if (latest_image_.empty()) {
            RCLCPP_ERROR(this->get_logger(), "No images received yet, cannot save!");
            return;
        }

        rclcpp::sleep_for(std::chrono::milliseconds(500));
        puzzle_images_.push_back(latest_image_.clone());
        RCLCPP_INFO(this->get_logger(), "Captured image.");
        auto confirm_msg = std_msgs::msg::Bool();
        confirm_msg.data = true;
        confirm_pub_->publish(confirm_msg);

        if (initialPuzzleImages.size() == PUZZLE_SIZE){
            RCLCPP_INFO(this->get_logger(), "All images received. Starting processing...");
            processPuzzlePieces(true);
            RCLCPP_INFO(this->get_logger(), "Succesfully processed all images. Preparing assembly...");
            assembly();
            RCLCPP_INFO(this->get_logger(), "Puzzle assembly finished.");
        }
    }

    int processPuzzlePieces(bool showImages = false){
        for (int i = 0; i < PUZZLE_SIZE; i++) {
            RCLCPP_INFO_STREAM(this->get_logger(), "Proccesing element " << i+1 << " of " << PUZZLE_SIZE << ".");

            Element elem = elementPipeline(initialPuzzleImages.at(i), i);
            if (!elem.edges.empty()){
                processedPuzzlePieces.push_back(elem); //i acts as unique ID for each element
            }
            else {
                RCLCPP_ERROR(this->get_logger(), "Pre-processing could not finish for element %d!", i+1);
                return 1;
            }
            if (showImages){
                plotEdges(processedPuzzlePieces.at(i).normalizedEdges, "edges", PUZZLE_IMAGES_SIZE);
                waitKey(0);
            }
        }
        if (showImages) destroyAllWindows();
        return 0;
    }

    void assembly(){
        matchingPipeline(processedPuzzlePieces);

        /*
            scara_msgs::msg::PiecePose msg;
            msg.piece_id = id;
            msg.start_pose.position.x = start_x;
            msg.start_pose.position.y = start_y;
            pose.start_pose.orientation.w = start_orientation; //QUATERNION!
            msg.goal_pose.position.x = goal_x;
            msg.goal_pose.position.y = goal_y;
            pose.goal_pose.orientation.w = goal_orientation; //QUATERNION!
        */
    }

    int loadProcessingParameters(){
        if(loadParameters(configFile)){
            RCLCPP_ERROR(this->get_logger(), "Could not read processing parameters for puzzle solver!");
            return 1;
        }
        return 0;
    }    

    std::string configFile = package_path + "/config/solverConfig.txt";
    std::string imagesDirectory = package_path + "/images/*.jpg";
    std::vector<Element> processedPuzzlePieces;
    cv::Mat latest_image_;
    std::vector<cv::Mat> puzzle_images_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr robot_sub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr confirm_pub_;
    rclcpp::Publisher<scara_msgs::msg::PiecePose>::SharedPtr assemble_pub_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PuzzleSolverNode>(9);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
