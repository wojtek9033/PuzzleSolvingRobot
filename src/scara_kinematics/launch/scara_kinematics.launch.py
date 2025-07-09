from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():

    is_sim_arg = DeclareLaunchArgument(
        "is_sim",
        default_value="False"
    )

    is_sim = LaunchConfiguration("is_sim")

    ik_node = Node(
        package="scara_kinematics",
        executable="scara_kinematics_node",
        parameters=[{"is_sim": is_sim}]
    )

    return LaunchDescription([
        ik_node
    ])