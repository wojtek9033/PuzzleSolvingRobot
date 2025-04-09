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
            if(loadImages(imagesDirectory)){
                return 1;
            }
            return 0;
        }

        int processPuzzlePieces(bool showImages = false){
            for (int i = 0; i < PUZZLE_SIZE; i++) {
                RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Proccesing element " << i+1 << " of " << PUZZLE_SIZE << ".");
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
            return 0;
        }

        void assembly(){
            matchingPipeline(processedPuzzlePieces);
        }

        std::string imagesDirectory;
    private:
        const char* configFile =  "config.txt";
        std::vector<Element> processedPuzzlePieces;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PuzzleSolverNode>();
    node->imagesDirectory = package_path + "/images/*.jpg";
    if(node->loadProcessingParameters()){
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Could not read processing parameters for puzzle solver!");
    }
    node->processPuzzlePieces(true);
    
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
