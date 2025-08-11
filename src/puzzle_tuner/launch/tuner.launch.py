import os
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    puzzle_tuner_node = Node(
        package="puzzle_tuner",
        executable="puzzle_tuner_node",
    )

    return LaunchDescription([
        puzzle_tuner_node
    ])