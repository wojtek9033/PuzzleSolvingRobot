from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    base_link_arg = DeclareLaunchArgument("base_link", default_value="base_link")
    tip_link_arg = DeclareLaunchArgument("tip_link", default_value="gripper")

    ik_node = Node(
        package="scara_iksolver",
        executable="scara_ik_node",
        parameters=[
            {"base_link": LaunchConfiguration("base_link")},
            {"tip_link": LaunchConfiguration("tip_link")},
        ],
    )

    return LaunchDescription([
        base_link_arg,
        tip_link_arg,
        ik_node
    ])