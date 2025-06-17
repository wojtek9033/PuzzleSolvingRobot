from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    ik_node = Node(
        package="scara_kinematics",
        executable="scara_kinematics_node",
    )

    return LaunchDescription([
        ik_node
    ])