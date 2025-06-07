import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    controller = IncludeLaunchDescription(
        os.path.join(
            get_package_share_directory("scara_controller"),
            "launch",
            "controller.launch.py"
            ),
            launch_arguments={"is_sim": "False"}.items()
        )
    
    moveit = IncludeLaunchDescription(
        os.path.join(
            get_package_share_directory("scara_moveit"),
            "launch",
            "moveit.launch.py"
            ),
            launch_arguments={"is_sim": "False"}.items()
        )
    
    server = IncludeLaunchDescription(
        os.path.join(
            get_package_share_directory("scara_server"),
            "launch",
            "scara_server.launch.py"
            ),
            launch_arguments={"is_sim": "False"}.items()
        )
    
    return LaunchDescription([
        controller,
        moveit,
        server
    ])
    