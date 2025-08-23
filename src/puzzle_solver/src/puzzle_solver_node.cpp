#include <iostream>
#include <chrono>
#include <memory>

#include "puzzle_solver/puzzle_utils.h"
#include "puzzle_solver/puzzle_processing.h"
#include "puzzle_solver/puzzle_matching.h"
#include "puzzle_solver/scara_positions.h"

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <std_msgs/msg/bool.hpp>
#include <cv_bridge/cv_bridge.hpp>

#include "scara_msgs/msg/piece_pose.hpp"
#include "scara_msgs/action/scara_task.hpp"

std::map<std::pair<int,int>, std::vector<MatchInfo>>  puzzleMatchInfo;
std::map<std::pair<int,int>, MatchInfo> puzzleBestMatches;

std::string package_path = ament_index_cpp::get_package_share_directory("puzzle_tuner");

using namespace std::chrono_literals;
using ClientGoalHandle = rclcpp_action::ClientGoalHandle<scara_msgs::action::ScaraTask>;

class PuzzleSolverNode : public rclcpp::Node{
public:
        PuzzleSolverNode() 
            :   Node("puzzle_solver_node"),
                config_file_(package_path + "/config/solver_config.txt"),
                images_directory_(package_path + "/images/*.jpg"),
                IMAGE_WIDTH_MM_(58.4),
                IMAGE_HEIGHT_MM_(43.9),
                IMAGE_WIDTH_PX_(800.0),
                IMAGE_HEIGHT_PX_(600.0),
                CORNERS_REINF_BOX_SIZE_{10},
                NORMALIZE_SAMPLES_VAL_{300},
                FLAT_TRESHOLD_{4.0},
                LOCAL_MAXIMA_NEIGHBORHOOD_{6} // neighborhood size of the pixel for finding local maximas
                {
            
            this->declare_parameter<int>("puzzle_size", 4);
            PUZZLE_SIZE = this->get_parameter("puzzle_size").as_int();

            client_ = rclcpp_action::create_client<scara_msgs::action::ScaraTask>(this, "scara_task");
            timer_ = create_wall_timer(1s, std::bind(&PuzzleSolverNode::timerCallback, this));

            camera_sub_ = this->create_subscription<sensor_msgs::msg::Image> (
                "/camera/image_raw",
                10,
                std::bind(&PuzzleSolverNode::camera_callback, this, std::placeholders::_1)
            );

            joint_states_sub_ = this->create_subscription<sensor_msgs::msg::JointState> (
                "/joint_states",
                10,
                std::bind(&PuzzleSolverNode::jointStatesCallback, this, std::placeholders::_1)
            );

            confirm_pub_ = this->create_publisher<std_msgs::msg::Bool> (
                "/capture/confirm",
                10
            );

            assemble_pub_ = this->create_publisher<scara_msgs::msg::PiecePose>(
                "/assembly/solution",
                10
            );

            load_processing_parameters(config_file_);
            RCLCPP_INFO(this->get_logger(), "Ready for processing.");
        }
        ~PuzzleSolverNode(){
            RCLCPP_INFO(this->get_logger(), "Puzzle solver node destroyed.");
        }

private:
    rclcpp_action::Client<scara_msgs::action::ScaraTask>::SharedPtr client_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr camera_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr capture_trig_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_states_sub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr confirm_pub_;
    rclcpp::Publisher<scara_msgs::msg::PiecePose>::SharedPtr assemble_pub_;
    std::vector<cv::Mat> puzzle_images_;
    std::vector<Element> processed_puzzle_images_;
    std::vector<Element> assembly_;
    std::vector<scara_msgs::msg::PiecePose> robot_positions_;
    std::vector<double> image_angles_;
    cv::Mat camera_frame_;
    double latest_joint_3_angle_;
    const std::string config_file_;
    const std::string images_directory_;
    const double IMAGE_WIDTH_MM_, IMAGE_HEIGHT_MM_, IMAGE_WIDTH_PX_, IMAGE_HEIGHT_PX_, FLAT_TRESHOLD_;
    const int CORNERS_REINF_BOX_SIZE_, NORMALIZE_SAMPLES_VAL_, LOCAL_MAXIMA_NEIGHBORHOOD_; // neighborhood size of the pixel for finding local maximas

private:
    void timerCallback() {
        timer_->cancel();

        if (!client_->wait_for_action_server(10s)) {
            RCLCPP_ERROR(this->get_logger(), "Action server not available after waiting!");
            rclcpp::shutdown();
        }

        auto goal_msg = scara_msgs::action::ScaraTask::Goal();
        goal_msg.command = "capture";
        RCLCPP_INFO(this->get_logger(), "Solver client sending capture request");

        auto send_goal_options = rclcpp_action::Client<scara_msgs::action::ScaraTask>::SendGoalOptions();
        send_goal_options.goal_response_callback = std::bind(&PuzzleSolverNode::goalCallback, this, std::placeholders::_1);
        send_goal_options.feedback_callback = std::bind(&PuzzleSolverNode::feedbackCallback, this, std::placeholders::_1, std::placeholders::_2);
        send_goal_options.result_callback = std::bind(&PuzzleSolverNode::resultCallback, this, std::placeholders::_1);

        client_->async_send_goal(goal_msg, send_goal_options);
    }

    void goalCallback(const ClientGoalHandle::SharedPtr &goal_handle) {
        if (!goal_handle)
            RCLCPP_ERROR(this->get_logger(), "Goal was rejected by server");
        else
            RCLCPP_INFO(this->get_logger(), "Goal accepted by server");
    }

    void feedbackCallback(ClientGoalHandle::SharedPtr, const std::shared_ptr<const scara_msgs::action::ScaraTask::Feedback> feedback) {
        if (camera_frame_.empty()) {
            RCLCPP_ERROR(this->get_logger(), "No images received yet, cannot save!");
            return;
        }

        if (feedback->current_step <= 9) {
            // lower than 9 means when pictures are taken 
            puzzle_images_.push_back(camera_frame_.clone());
            image_angles_.push_back(latest_joint_3_angle_);
            RCLCPP_INFO(this->get_logger(), "Solver captured image %ld", puzzle_images_.size());

            if (puzzle_images_.size() == PUZZLE_SIZE){
                // START ASSEMBLING IN SEPARATE THREAD
                RCLCPP_INFO(this->get_logger(), "Starting thread for preprocessing and assembly.");
                std::thread{std::bind(&PuzzleSolverNode::processing_pipeline, this)}.detach();
            }
        } else if (feedback->current_step > 9) {
            // higher than 9 means when puzzle elements are placed
        }
        

    }

    void resultCallback(const ClientGoalHandle::WrappedResult &result) {
        switch (result.code) {
            case rclcpp_action::ResultCode::SUCCEEDED:
                return;
            case rclcpp_action::ResultCode::ABORTED:
                RCLCPP_ERROR(this->get_logger(), "Goal was aborted");
                return;
            case rclcpp_action::ResultCode::CANCELED:
                RCLCPP_ERROR(this->get_logger(), "Goal was canceled");
                return;
            default:
                RCLCPP_ERROR(get_logger(), "Unknown result code");
                return;
        }
    }

    void camera_callback(const sensor_msgs::msg::Image::SharedPtr msg){
        try {
            camera_frame_ = cv_bridge::toCvShare(msg, "bgr8")->image.clone();
        }
        catch (cv_bridge::Exception &e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
            return;
        }
    }

    void jointStatesCallback(const sensor_msgs::msg::JointState::SharedPtr msg) {
        latest_joint_3_angle_ = msg->position.at(2);
    }

    void processing_pipeline(){
        RCLCPP_INFO(this->get_logger(), "Images pre-processing in progress...");
        PUZZLE_IMAGES_SIZE = puzzle_images_.at(0).size();
        if (process_puzzle_images()) {
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

        robot_positions_ = convert_to_msg(assembly_ );
    }

    void load_processing_parameters(const std::string& path){
        if(loadParameters(path)){
            RCLCPP_ERROR(this->get_logger(), "Could not read solver parameters!");
            rclcpp::shutdown();
        }
        RCLCPP_INFO(this->get_logger(), "Succesfully read solver parameters.");
    }    

    std::vector<scara_msgs::msg::PiecePose> convert_to_msg(std::vector<Element> &assembly) {

        const double y_mm_per_px = IMAGE_HEIGHT_MM_/IMAGE_HEIGHT_PX_;
        const double x_mm_per_px = IMAGE_WIDTH_MM_/IMAGE_WIDTH_PX_;

        std::vector<scara_msgs::msg::PiecePose> robot_poses;
        //for (size_t i = 1; i < assembly.size(); i++) {
            //robot_poses.at(i).start_pose
        //}

        std::vector<std::array<double,3>> elements_placed = placeElementsIn2D(assembly);
        for (std::array<double,3>& pos : elements_placed) {
            pos.at(0) *= y_mm_per_px;
            pos.at(1) *= x_mm_per_px;
        }
        

        return robot_poses;
    }

Element element_pipeline(cv::Mat image, int id){
    Element piece_data;
    piece_data.id = id;
    cv::imshow("Pre-processed image", image);
    cv::waitKey(0);
    
    // find puzzle centroid
    Mat inverted,dist;
    cv::bitwise_not(image,inverted);
    cv::distanceTransform(inverted,dist, DIST_L1, DIST_MASK_PRECISE, CV_8U);
    double minVal, maxVal;
    cv::Point maxLoc;
    cv::minMaxLoc(dist, &minVal, &maxVal, nullptr, &maxLoc);
    cv::cvtColor(dist, dist, COLOR_GRAY2BGR);
    cv::circle(dist,maxLoc,5,cv::Scalar(255, 0, 0),2);
    piece_data.centroid = maxLoc;
    cv::imshow("Centroid", dist);
    cv::waitKey(0); 

    vector<Point> candidate_corners = getCorners(image,LOCAL_MAXIMA_NEIGHBORHOOD_);
    if (candidate_corners.size() < 4) {
        RCLCPP_ERROR(this->get_logger(), "Critical Error! Could not find at least 4 corners for piece %d.", id + 1);
        return {};
    } else if (candidate_corners.size() >= 40) {
        RCLCPP_ERROR(this->get_logger(), "Error! Too many potential corners! Run Tuner to adjust parameters");
        return {};
    }
    RCLCPP_INFO(this->get_logger(), "Successfully extracted piece %d corners", id + 1);

    vector<Point> puzzle_contour = getContour(image);
    if (puzzle_contour.empty())
        RCLCPP_ERROR(this->get_logger(), "Could not get element %d contour!", id + 1);
    RCLCPP_INFO(this->get_logger(), "Successfully extracted piece %d contour", id + 1);

    vector<Point> reinforced_corners = reinforceCorners(candidate_corners, puzzle_contour, CORNERS_REINF_BOX_SIZE_);
    if (reinforced_corners.size() != 4) {
        RCLCPP_ERROR(this->get_logger(), "Something went wrong when reinforcing the corners for element %d.", id + 1);
    }
    RCLCPP_INFO(this->get_logger(), "Successfully reinforced piece %d corners", id + 1);

    piece_data.edges = getPuzzleEdges(puzzle_contour, reinforced_corners);
    RCLCPP_INFO(this->get_logger(), "Successfully extracted piece %d edges", id + 1);

    piece_data.edgeType = determineInOut(piece_data.edges, FLAT_TRESHOLD_);
    // normalize element edges for matching algorithm and get orientation of each edge
    normalizeEdges(piece_data, NORMALIZE_SAMPLES_VAL_);
    
    int numFlats = std::count(piece_data.edgeType.begin(), piece_data.edgeType.end(),0);
    if (numFlats > 2){
        RCLCPP_ERROR(this->get_logger(), "Error! Piece %d was assigned more than two flat edges!", id + 1);
        return {};
    } else if(numFlats == 1){
        piece_data.isEdgePiece = true;
    } else if(numFlats == 2){
        piece_data.isCornerPiece = true;
    }
    RCLCPP_INFO_STREAM(this->get_logger(), "Element " << id + 1 << " edge types: [" << piece_data.edgeType[0] << "," << piece_data.edgeType[1] << "," << piece_data.edgeType[2] << "," << piece_data.edgeType[3] << "]");
    return piece_data;
}

    int process_puzzle_images(){
        for (size_t i = 0; i < PUZZLE_SIZE; i++) {
            RCLCPP_INFO_STREAM(this->get_logger(), "Proccesing element " << i+1 << " of " << PUZZLE_SIZE << ".");
            cv::rotate(puzzle_images_.at(i), puzzle_images_.at(i), cv::ROTATE_180);
            cv::imshow("Initial image", puzzle_images_.at(i));
            cv::waitKey(0);
            preprocImage(puzzle_images_.at(i));
            Element elem = element_pipeline(puzzle_images_.at(i), i); //i acts as unique ID for each element
            if (!elem.edges.empty()){
                processed_puzzle_images_.push_back(elem); 
            }
            else {
                RCLCPP_ERROR(this->get_logger(), "Pre-processing could not finish for element %ld! Adjust processing parameters.", i+1);
                destroyAllWindows();
                return 1;
            }
            //plotEdges(processed_puzzle_images_.at(i).normalizedEdges, "edges", PUZZLE_IMAGES_SIZE);
            destroyAllWindows();
        }
        
        return 0;
    }

    void print_matches(const std::map<std::pair<int,int>, std::vector<MatchInfo>> matchMap){
        RCLCPP_INFO(this->get_logger(), "All matches:");
        for (const auto & [key, matches] : matchMap){
            RCLCPP_INFO_STREAM(this->get_logger(), "Element " << key.first + 1  << " edge " << key.second << ", matches:");
            for (const auto & match : matches){
                RCLCPP_INFO_STREAM(this->get_logger(), "Element " << match.idB + 1 << " edge " << match.edgeB << ", with score: " << match.similarityScore);
            }
        }
    }

    void print_matches(const std::map<std::pair<int,int>, MatchInfo> matchMap){
        RCLCPP_INFO(this->get_logger(), "Best matches:");
        for (const auto& [key, match] : matchMap) {
            RCLCPP_INFO_STREAM(this->get_logger(), "Element " << key.first + 1 << " (Edge " << key.second << ") -> " 
            << "Element " << match.idB + 1<< " (Edge " << match.edgeB << ") " << "Score: " << match.similarityScore);
        }
    }
    
    std::vector<Element> matchingPipeline(std::vector<Element> &processed_elements){
        std::map<std::pair<int,int>, std::vector<MatchInfo>>  puzzle_match_info;
        std::map<std::pair<int,int>, MatchInfo> puzzle_best_matches;

        puzzle_match_info = findMatches(processed_elements);
        RCLCPP_INFO(this->get_logger(), "Found initial matches.");
        print_matches(puzzle_match_info);
        puzzle_best_matches = extractBestMatches(puzzle_match_info);
        RCLCPP_INFO(this->get_logger(), "Extracted best matches.");
        print_matches(puzzle_best_matches);
        assignMatches(processed_elements, puzzle_best_matches);
        RCLCPP_INFO(this->get_logger(), "Assigned matches.");
        std::vector<Element> assembly = puzzleAssembly(processed_elements);
        drawAssembly(assembly, puzzle_images_);
        return assembly;
    }
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PuzzleSolverNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
