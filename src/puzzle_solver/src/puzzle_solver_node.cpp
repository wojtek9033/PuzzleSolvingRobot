#include <iostream>
#include "puzzle_utils.h"
#include "puzzle_processing.h"
#include "puzzle_matching.h"

#include <rclcpp/rclcpp.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp> // For getting package paths
#include <sensor_msgs/msg/image.hpp> // ROS 2 Image message type
#include <cv_bridge/cv_bridge.hpp> // Bridge between ROS 2 and OpenCV

std::map<std::pair<int,int>, std::vector<MatchInfo>>  puzzleMatchInfo;
std::map<std::pair<int,int>, MatchInfo> puzzleBestMatches;

std::string package_path = ament_index_cpp::get_package_share_directory("puzzle_solver");

class PuzzleSolverNode : public rclcpp::Node{
    public:
        PuzzleSolverNode() : Node("puzzle_solver_node"){
            RCLCPP_INFO(this->get_logger(), "Puzzle solver node started.");
            // Subscribe to the topic publishing puzzle piece images
            image_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
                "/puzzle_piece/image_raw",
                rclcpp::SensorDataQoS(),
                std::bind(&PuzzleSolverNode::imageCallback, this, std::placeholders::_1)
            );
        }
        ~PuzzleSolverNode(){
            RCLCPP_INFO(this->get_logger(), "Puzzle solver node destroyed.");
        }
        int loadProcessingParameters(){
            if(loadParameters(configFile)){
                RCLCPP_ERROR(this->get_logger(), "Could not read processing parameters for puzzle solver!");
                return 1;
            }
            return 0;
        }

        void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg){
            try{
                // Convert a sensor_msgs::Image message to an OpenCV-compatible CvImage
                Mat image = cv_bridge::toCvShare(msg, "bgr8")->image.clone();
                // Store the image in a thread-safe manner
                {
                    std::lock_guard<std::mutex> lock(image_mutex_);
                    puzzle_images_.push_back(image);
                    new_image_received_ = true;
                }
                image_cv_.notify_one(); // Notify waiting threads
                RCLCPP_INFO(this->get_logger(), "Image %ld received and stored.", puzzle_images_.size());

            } catch (cv_bridge::Exception &e) {
                RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
                return;
            }
        }

        int loadPuzzleImages(bool simulation = false){
            if (simulation){
                // simulation without the use of camera. Sample images provided.
                if(loadImages(imagesDirectory)){
                    RCLCPP_ERROR(this->get_logger(), "Could not read puzzle images!");
                    return 1;
                }
                return 0;
            } else {
                puzzle_images_.clear();
                for (int i = 0; i < PUZZLE_SIZE; i++){
                    RCLCPP_INFO_STREAM(this->get_logger(), "Triggering robot movement to position " << i);
                    // TODO: call to move robot

                    RCLCPP_INFO_STREAM(this->get_logger(), "Waiting for image" << i +1 << " from camera...");
                    // Wait for a new image to be received
                    std::unique_lock<std::mutex> lock(image_mutex_);
                    image_cv_.wait(lock, [&] { return new_image_received_; });
                    new_image_received_ = false;
                }
                
                initialPuzzleImages = puzzle_images_;
                PUZZLE_IMAGES_SIZE = initialPuzzleImages.at(0).size();


                return 0;
            }

        }

        int processPuzzlePieces(bool showImages = false){
            for (int i = 0; i < PUZZLE_SIZE; i++) {
                RCLCPP_INFO_STREAM(this->get_logger(), "Proccesing element " << i+1 << " of " << PUZZLE_SIZE << ".");
                Mat puzzleImage = initialPuzzleImages.at(i);
                Element elem = elementPipeline(puzzleImage, i);
                if (!elem.edges.empty()){
                    processedPuzzlePieces.push_back(elem); //i acts as unique ID for each element
                }
                else {
                    std::cerr << "@@@ Error! pre-processing could not finish for element " << i +1 << std::endl;
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
        }

    private:
        std::string configFile = package_path + "/config/solverConfig.txt";
        std::string imagesDirectory = package_path + "/images/*.jpg";
        std::vector<Element> processedPuzzlePieces;
        std::vector<cv::Mat> puzzle_images_;
        rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;
        std::mutex image_mutex_; // Mutex for thread-safe image storage
        std::condition_variable image_cv_; // Condition variable for image synchronization
        bool new_image_received_ = false;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PuzzleSolverNode>();
    node->loadProcessingParameters();
    node->loadPuzzleImages(true);
    node->processPuzzlePieces(true);
    node->assembly();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
