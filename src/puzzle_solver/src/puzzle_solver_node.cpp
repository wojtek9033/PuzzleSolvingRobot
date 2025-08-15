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
#include <cv_bridge/cv_bridge.hpp> // Bridge between ROS 2 and OpenCV

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
                config_file_(package_path + "/config/solverConfig.txt"),
                images_directory_(package_path + "/images/*.jpg"),
                image_width_mm_(58.4),
                image_height_mm_(43.9),
                image_width_px_(3200.0),
                image_height_px_(2400.0) {
            
            this->declare_parameter<int>("puzzle_size", 9);
            PUZZLE_SIZE = this->get_parameter("puzzle_size").as_int();

            client_ = rclcpp_action::create_client<scara_msgs::action::ScaraTask>(this, "scara_task");
            timer_ = create_wall_timer(1s, std::bind(&PuzzleSolverNode::timerCallback, this));

            camera_sub_ = this->create_subscription<sensor_msgs::msg::Image> (
                "/camera/image_raw",
                10,
                std::bind(&PuzzleSolverNode::camera_callback, this, std::placeholders::_1)
            );
            /*
            capture_trig_sub_ = this->create_subscription<std_msgs::msg::Bool> (
                "/capture/trigger",
                10,
                std::bind(&PuzzleSolverNode::trigger_callback, this, std::placeholders::_1)
            );
            */
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
    const double image_width_mm_, image_height_mm_, image_width_px_, image_height_px_;

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
    /*
    void trigger_callback(const std_msgs::msg::Bool::SharedPtr msg){
        if (!msg->data)
            return;

        if (camera_frame_.empty()) {
            RCLCPP_ERROR(this->get_logger(), "No images received yet, cannot save!");
            return;
        }

        rclcpp::sleep_for(std::chrono::milliseconds(500));
        puzzle_images_.push_back(camera_frame_.clone());
        image_angles_.push_back(latest_joint_3_angle_);
        RCLCPP_INFO(this->get_logger(), "Solver captured image %ld", puzzle_images_.size());
        auto confirm_msg = std_msgs::msg::Bool();
        confirm_msg.data = true;
        confirm_pub_->publish(confirm_msg);
        rclcpp::sleep_for(std::chrono::milliseconds(100)); // wait for Server node to release a thread
        if (puzzle_images_.size() == PUZZLE_SIZE){
            // START ASSEMBLING IN SEPARATE THREAD
            std::thread{std::bind(&PuzzleSolverNode::assembly, this)}.detach();
        }
    }
    */
    void jointStatesCallback(const sensor_msgs::msg::JointState::SharedPtr msg) {
        latest_joint_3_angle_ = msg->position.at(2);
    }

    void processing_pipeline(){
        RCLCPP_INFO(this->get_logger(), "Images pre-processing in progress...");
        if (process_puzzle_images(true)) {
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

    int process_puzzle_images(bool showImages = false){
        for (size_t i = 0; i < PUZZLE_SIZE; i++) {
            RCLCPP_INFO_STREAM(this->get_logger(), "Proccesing element " << i+1 << " of " << PUZZLE_SIZE << ".");

            Element elem = elementPipeline(puzzle_images_.at(i), i); //i acts as unique ID for each element
            if (!elem.edges.empty()){
                processed_puzzle_images_.push_back(elem); 
            }
            else {
                RCLCPP_ERROR(this->get_logger(), "Pre-processing could not finish for element %ld! Adjust processing parameters.", i+1);
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

    std::vector<scara_msgs::msg::PiecePose> convert_to_msg(std::vector<Element> &assembly) {

        const double y_mm_per_px = image_height_mm_/image_height_px_;
        const double x_mm_per_px = image_width_mm_/image_width_px_;

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
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PuzzleSolverNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
