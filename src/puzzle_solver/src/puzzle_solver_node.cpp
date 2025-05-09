#include <iostream>
#include <chrono>
#include <memory>

#include "puzzle_utils.h"
#include "puzzle_processing.h"
#include "puzzle_matching.h"

#include <rclcpp/rclcpp.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <sensor_msgs/msg/image.hpp>
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
            RCLCPP_INFO(this->get_logger(), "Solver waiting for capture trigger...");
        }
        ~PuzzleSolverNode(){
            RCLCPP_INFO(this->get_logger(), "Puzzle solver node destroyed.");
        }

private:
 
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr robot_sub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr confirm_pub_;
    rclcpp::Publisher<scara_msgs::msg::PiecePose>::SharedPtr assemble_pub_;
    std::vector<cv::Mat> puzzle_images_;
    std::vector<Element> processed_puzzle_images_;
    std::vector<Element> assembly_;
    std::vector<scara_msgs::msg::PiecePose> robot_positions_;
    cv::Mat latest_image_;
    std::string configFile = package_path + "/config/solverConfig.txt";
    std::string imagesDirectory = package_path + "/images/*.jpg";
    const double milimeters_per_pixel_x = 0.125;
    const double milimeters_per_pixel_y = 0.125;

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
        rclcpp::sleep_for(std::chrono::milliseconds(250)); // wait for Server node to release a thread
        if (puzzle_images_.size() == PUZZLE_SIZE){
            // START ASSEMBLING IN SEPARATE THREAD
            std::thread{std::bind(&PuzzleSolverNode::assembly, this)}.detach();
        }
    }

    void assembly(){
        RCLCPP_INFO(this->get_logger(), "All images received. Starting processing...");
        if (processPuzzlePieces(true)) {
            RCLCPP_ERROR(this->get_logger(), "Proccesing puzzle images did not succeed!");
            return;
        }
        RCLCPP_INFO(this->get_logger(), "Succesfully processed all images. Preparing assembly...");
        
        assembly_ = matchingPipeline(processed_puzzle_images_);
        if (assembly_.empty()) {
            RCLCPP_ERROR(this->get_logger(), "Puzzle assembly did not succeed!");
            return;
        }
        RCLCPP_INFO(this->get_logger(), "Puzzle assembly finished. Publishing data to server...");

        robot_positions_ = convert_to_msg(assembly_);
    }

    int loadProcessingParameters(){
        if(loadParameters(configFile)){
            RCLCPP_ERROR(this->get_logger(), "Could not read processing parameters for puzzle solver!");
            return 1;
        }
        return 0;
    }    

    int processPuzzlePieces(bool showImages = false){
        for (int i = 0; i < PUZZLE_SIZE; i++) {
            RCLCPP_INFO_STREAM(this->get_logger(), "Proccesing element " << i+1 << " of " << PUZZLE_SIZE << ".");

            Element elem = elementPipeline(puzzle_images_.at(i), i); //i acts as unique ID for each element
            if (!elem.edges.empty()){
                processed_puzzle_images_.push_back(elem); 
            }
            else {
                RCLCPP_ERROR(this->get_logger(), "Pre-processing could not finish for element %d! Adjust processing parameters.", i+1);
                return 1;
            }
            if (showImages){
                plotEdges(processed_puzzle_images_.at(i).normalizedEdges, "edges", PUZZLE_IMAGES_SIZE);
                waitKey(0);
            }
        }
        if (showImages) destroyAllWindows();
        return 0;
    }

    std::vector<scara_msgs::msg::PiecePose> convert_to_msg(const std::vector<Element> &assembly) {

        std::vector<scara_msgs::msg::PiecePose> robot_poses;
        scara_msgs::msg::PiecePose pose;
        for (size_t i = 0; i < assembly.size(); i++) {
            double theta = CV_PI;
            pose.start_pose.orientation.x = 0.0;
            pose.start_pose.orientation.y = 0.0;
            pose.start_pose.orientation.z = sin(theta/ 2.0);
            pose.start_pose.orientation.w = cos(theta/ 2.0);
            pose.start_pose.position.x = assembly.at(i).centroid.x * milimeters_per_pixel_x;
            pose.start_pose.position.y = assembly.at(i).centroid.y * milimeters_per_pixel_y; 
            pose.start_pose.position.z = 0; // fixed for every piece

            theta = assembly.at(i).rotationAngle;
            pose.goal_pose.orientation.x = 0.0;
            pose.goal_pose.orientation.y = 0.0;
            pose.goal_pose.orientation.z = sin(theta/ 2.0);
            pose.goal_pose.orientation.w = cos(theta/ 2.0);
            if (i == 0) {
                // First element has allways position
                pose.goal_pose.position.x = 0.7;
                pose.goal_pose.position.y = 0.10;
                pose.goal_pose.position.z = 0.50; // fixed for every piece
            } else {
                pose.goal_pose.position.x = 0;
                pose.goal_pose.position.y = 0;
                pose.goal_pose.position.z = 0; // fixed for every piece
            }

            robot_poses.push_back(pose);
        }

        return robot_poses;
    }
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PuzzleSolverNode>(9);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
