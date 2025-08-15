import os
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    puzzle_solver_node = Node(
        package="puzzle_solver",
        executable="puzzle_solver_node",
    )

    return LaunchDescription([
        puzzle_solver_node
    ])