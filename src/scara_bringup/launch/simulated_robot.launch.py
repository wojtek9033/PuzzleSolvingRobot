import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    gazebo = IncludeLaunchDescription(
        os.path.join(
            get_package_share_directory("scara_description"),
            "launch",
            "gazebo.launch.py"
            )
        )

    controller = IncludeLaunchDescription(
        os.path.join(
            get_package_share_directory("scara_controller"),
            "launch",
            "controller.launch.py"
            ),
            launch_arguments={"is_sim": "True"}.items()
        )
    
    iksolver = IncludeLaunchDescription(
        os.path.join(
            get_package_share_directory("scara_kinematics"),
            "launch",
            "scara_kinematics.launch.py"
            ),
            launch_arguments={"is_sim": "True"}.items()
        )
    
    #server = IncludeLaunchDescription(
    #    os.path.join(
    #        get_package_share_directory("scara_server"),
    #        "launch",
    #        "scara_server.launch.py"
    #        )
    #    )
    
    return LaunchDescription([
        gazebo,
        controller,
        iksolver
        #server
    ])
    