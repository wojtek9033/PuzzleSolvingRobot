#include <iostream>
#include "puzzle_utils.h"
#include "puzzle_processing.h"
#include "puzzle_matching.h"

#include <rclcpp/rclcpp.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.hpp>

std::map<std::pair<int,int>, std::vector<MatchInfo>>  puzzleMatchInfo;
std::map<std::pair<int,int>, MatchInfo> puzzleBestMatches;

std::string package_path = ament_index_cpp::get_package_share_directory("puzzle_solver");

class PuzzleSolverNode : public rclcpp::Node{
    public:
        PuzzleSolverNode() : Node("puzzle_solver_node"){
            RCLCPP_INFO(this->get_logger(), "Puzzle solver node started.");
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
        int loadPuzzleImages(bool simulation = false){
            if (simulation){
                // simulation without the use of camera. Sample images provided.
                if(loadImages(imagesDirectory)){
                    RCLCPP_ERROR(this->get_logger(), "Could not read puzzle images!");
                    return 1;
                }
            } else {
                // for camera integration
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
