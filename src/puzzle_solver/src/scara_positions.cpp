#include "puzzle_solver/scara_positions.h"

namespace scara_positions {

        const scara_msgs::msg::PiecePose arm_middle_pose = []{
        scara_msgs::msg::PiecePose pose;

        pose.start_pose.position.x = 0.26;
        pose.start_pose.position.y = -0.001;
        pose.start_pose.position.z = 0.221;
        pose.start_pose.orientation.x = 0.0;
        pose.start_pose.orientation.y = 0.0;
        pose.start_pose.orientation.z = 0.0;
        pose.start_pose.orientation.w = 1.0;

        pose.goal_pose.position.x = 0.0; 
        pose.goal_pose.position.y = 0.0;
        pose.goal_pose.position.z = 0.0;
        pose.goal_pose.orientation.x = 0.0;
        pose.goal_pose.orientation.y = 0.0;
        pose.goal_pose.orientation.z = 0.0;
        pose.goal_pose.orientation.w = 0.0;

        return pose;
    }();

    const scara_msgs::msg::PiecePose first_piece_pose = []{
        scara_msgs::msg::PiecePose pose;

        pose.start_pose.position.x = 0.105;
        pose.start_pose.position.y = -0.136;
        pose.start_pose.position.z = 0.221;
        pose.start_pose.orientation.x = 0.0;
        pose.start_pose.orientation.y = 0.0;
        pose.start_pose.orientation.z = 0.0;
        pose.start_pose.orientation.w = 1.0;

        pose.goal_pose.position.x = 0.0;
        pose.goal_pose.position.y = 0.0;
        pose.goal_pose.position.z = 0.0;
        pose.goal_pose.orientation.x = 0.0;
        pose.goal_pose.orientation.y = 0.0;
        pose.goal_pose.orientation.z = 0.0;
        pose.goal_pose.orientation.w = 0.0;

        return pose;
    }();

    const scara_msgs::msg::PiecePose second_piece_pose = []{
        scara_msgs::msg::PiecePose pose;
        
        pose.start_pose.position.x = 0.145;
        pose.start_pose.position.y = -0.175;
        pose.start_pose.position.z = 0.221;
        pose.start_pose.orientation.x = 0.0;
        pose.start_pose.orientation.y = 0.0;
        pose.start_pose.orientation.z = 0.0;
        pose.start_pose.orientation.w = 1.0;

        pose.goal_pose.position.x = 0.0;
        pose.goal_pose.position.y = 0.0;
        pose.goal_pose.position.z = 0.0;
        pose.goal_pose.orientation.x = 0.0;
        pose.goal_pose.orientation.y = 0.0;
        pose.goal_pose.orientation.z = 0.0;
        pose.goal_pose.orientation.w = 0.0;

        return pose;
    }();

    const scara_msgs::msg::PiecePose third_piece_pose = []{
        scara_msgs::msg::PiecePose pose;

        pose.start_pose.position.x = 0.198;
        pose.start_pose.position.y = -0.14;
        pose.start_pose.position.z = 0.221;
        pose.start_pose.orientation.x = 0.0;
        pose.start_pose.orientation.y = 0.0;
        pose.start_pose.orientation.z = 0.0;
        pose.start_pose.orientation.w = 1.0;

        pose.goal_pose.position.x = 0.0;
        pose.goal_pose.position.y = 0.0;
        pose.goal_pose.position.z = 0.0;
        pose.goal_pose.orientation.x = 0.0;
        pose.goal_pose.orientation.y = 0.0;
        pose.goal_pose.orientation.z = 0.0;
        pose.goal_pose.orientation.w = 0.0;

        return pose;
    }();

    const scara_msgs::msg::PiecePose fourth_piece_pose = []{
        scara_msgs::msg::PiecePose pose;
        
        pose.start_pose.position.x = 0.156;
        pose.start_pose.position.y = -0.091;
        pose.start_pose.position.z = 0.221;
        pose.start_pose.orientation.x = 0.0;
        pose.start_pose.orientation.y = 0.0;
        pose.start_pose.orientation.z = 0.0;
        pose.start_pose.orientation.w = 1.0;

        pose.goal_pose.position.x = 0.0;
        pose.goal_pose.position.y = 0.0;
        pose.goal_pose.position.z = 0.0;
        pose.goal_pose.orientation.x = 0.0;
        pose.goal_pose.orientation.y = 0.0;
        pose.goal_pose.orientation.z = 0.0;
        pose.goal_pose.orientation.w = 0.0;

        return pose;
    }();

    const scara_msgs::msg::PiecePose fifth_piece_pose = []{
        scara_msgs::msg::PiecePose pose;

        pose.start_pose.position.x = 0.0;
        pose.start_pose.position.y = 0.0;
        pose.start_pose.position.z = 0.0;
        pose.start_pose.orientation.x = 0.0;
        pose.start_pose.orientation.y = 0.0;
        pose.start_pose.orientation.z = 0.0;
        pose.start_pose.orientation.w = 0.0;

        pose.goal_pose.position.x = 0.0;
        pose.goal_pose.position.y = 0.0;
        pose.goal_pose.position.z = 0.0;
        pose.goal_pose.orientation.x = 0.0;
        pose.goal_pose.orientation.y = 0.0;
        pose.goal_pose.orientation.z = 0.0;
        pose.goal_pose.orientation.w = 0.0;

        return pose;
    }();

    const scara_msgs::msg::PiecePose sixth_piece_pose = []{
        scara_msgs::msg::PiecePose pose;
        
        pose.start_pose.position.x = 0.0;
        pose.start_pose.position.y = 0.0;
        pose.start_pose.position.z = 0.0;
        pose.start_pose.orientation.x = 0.0;
        pose.start_pose.orientation.y = 0.0;
        pose.start_pose.orientation.z = 0.0;
        pose.start_pose.orientation.w = 0.0;

        pose.goal_pose.position.x = 0.0;
        pose.goal_pose.position.y = 0.0;
        pose.goal_pose.position.z = 0.0;
        pose.goal_pose.orientation.x = 0.0;
        pose.goal_pose.orientation.y = 0.0;
        pose.goal_pose.orientation.z = 0.0;
        pose.goal_pose.orientation.w = 0.0;

        return pose;
    }();

    const scara_msgs::msg::PiecePose seventh_piece_pose = []{
        scara_msgs::msg::PiecePose pose;

        pose.start_pose.position.x = 0.0;
        pose.start_pose.position.y = 0.0;
        pose.start_pose.position.z = 0.0;
        pose.start_pose.orientation.x = 0.0;
        pose.start_pose.orientation.y = 0.0;
        pose.start_pose.orientation.z = 0.0;
        pose.start_pose.orientation.w = 0.0;

        pose.goal_pose.position.x = 0.0;
        pose.goal_pose.position.y = 0.0;
        pose.goal_pose.position.z = 0.0;
        pose.goal_pose.orientation.x = 0.0;
        pose.goal_pose.orientation.y = 0.0;
        pose.goal_pose.orientation.z = 0.0;
        pose.goal_pose.orientation.w = 0.0;

        return pose;
    }();

    const scara_msgs::msg::PiecePose eight_piece_pose = []{
        scara_msgs::msg::PiecePose pose;
        
        pose.start_pose.position.x = 0.0;
        pose.start_pose.position.y = 0.0;
        pose.start_pose.position.z = 0.0;
        pose.start_pose.orientation.x = 0.0;
        pose.start_pose.orientation.y = 0.0;
        pose.start_pose.orientation.z = 0.0;
        pose.start_pose.orientation.w = 0.0;

        pose.goal_pose.position.x = 0.0;
        pose.goal_pose.position.y = 0.0;
        pose.goal_pose.position.z = 0.0;
        pose.goal_pose.orientation.x = 0.0;
        pose.goal_pose.orientation.y = 0.0;
        pose.goal_pose.orientation.z = 0.0;
        pose.goal_pose.orientation.w = 0.0;

        return pose;
    }();

    const scara_msgs::msg::PiecePose nineth_piece_pose = []{
        scara_msgs::msg::PiecePose pose;

        pose.start_pose.position.x = 0.0;
        pose.start_pose.position.y = 0.0;
        pose.start_pose.position.z = 0.0;
        pose.start_pose.orientation.x = 0.0;
        pose.start_pose.orientation.y = 0.0;
        pose.start_pose.orientation.z = 0.0;
        pose.start_pose.orientation.w = 0.0;

        pose.goal_pose.position.x = 0.0;
        pose.goal_pose.position.y = 0.0;
        pose.goal_pose.position.z = 0.0;
        pose.goal_pose.orientation.x = 0.0;
        pose.goal_pose.orientation.y = 0.0;
        pose.goal_pose.orientation.z = 0.0;
        pose.goal_pose.orientation.w = 0.0;

        return pose;
    }();

    const std::vector<scara_msgs::msg::PiecePose>robot_poses = {
        first_piece_pose,
        second_piece_pose,
        third_piece_pose,
        fourth_piece_pose,
        fifth_piece_pose,
        sixth_piece_pose,
        seventh_piece_pose,
        eight_piece_pose,
        nineth_piece_pose
    };
    
        const scara_msgs::msg::PiecePose first_placed_piece_pose = []{
        scara_msgs::msg::PiecePose pose;

        pose.start_pose.position.x = 0.112;
        pose.start_pose.position.y = 0.13;
        pose.start_pose.position.z = 0.221;
        pose.start_pose.orientation.x = 0.0;
        pose.start_pose.orientation.y = 0.0;
        pose.start_pose.orientation.z = 0.0;
        pose.start_pose.orientation.w = 1.0;

        pose.goal_pose.position.x = 0.0;
        pose.goal_pose.position.y = 0.0;
        pose.goal_pose.position.z = 0.0;
        pose.goal_pose.orientation.x = 0.0;
        pose.goal_pose.orientation.y = 0.0;
        pose.goal_pose.orientation.z = 0.0;
        pose.goal_pose.orientation.w = 0.0;

        return pose;
    }();

} // namespace solver_constants