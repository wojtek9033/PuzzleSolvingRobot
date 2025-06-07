import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import UnlessCondition, IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from moveit_configs_utils import MoveItConfigsBuilder
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    is_sim_arg = DeclareLaunchArgument(
        "is_sim",
        default_value="True"
    )

    is_sim = LaunchConfiguration("is_sim")

    moveit_config = (
        MoveItConfigsBuilder("SCARA", package_name="scara_moveit")
        .robot_description(file_path=os.path.join(
            get_package_share_directory("scara_description"),
            "urdf",
            "scara.urdf.xacro"
            )
        )
        .robot_description_semantic(file_path="config/scara.srdf")
        .robot_description_kinematics(file_path="config/kinematics.yaml")
        .trajectory_execution(file_path="config/moveit_controllers.yaml")
        .moveit_cpp(file_path="config/planning_python_api.yaml")
        .to_moveit_configs()
    )

    task_server_node = Node(
        package="scara_server",
        executable="task_server_node",
        parameters=[moveit_config.to_dict(),
                    {"use_sim_time": is_sim}]
    )

    return LaunchDescription([
        is_sim_arg,
        task_server_node
    ])