import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription,DeclareLaunchArgument
from launch.substitutions import Command, LaunchConfiguration
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue

def generate_launch_description():

    scara_description_dir = get_package_share_directory('scara_description')

    model_arg = DeclareLaunchArgument(name='model', default_value=os.path.join(
                                        scara_description_dir, 'urdf', 'scara.urdf.xacro'
                                        ),
                                      description='Absolute path to robot urdf file')

    robot_description = ParameterValue(Command(['xacro ', LaunchConfiguration('model')]),
                                       value_type=str)

    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description': robot_description,
                    'use_sim_time': False}]
    )

    controller = IncludeLaunchDescription(
        os.path.join(
            get_package_share_directory("scara_controller"),
            "launch",
            "controller.launch.py"
            ),
            launch_arguments={"is_sim": "False"}.items()
        )
    
    iksolver = IncludeLaunchDescription(
        os.path.join(
            get_package_share_directory("scara_kinematics"),
            "launch",
            "scara_kinematics.launch.py"
            ),
            launch_arguments={"is_sim": "False"}.items()
        )

    #server = IncludeLaunchDescription(
    #    os.path.join(
    #        get_package_share_directory("scara_server"),
    #        "launch",
    #        "scara_server.launch.py"
    #        ),
    #        launch_arguments={"is_sim": "False"}.items()
    #    )
    
    return LaunchDescription([
        controller,
        iksolver,
        model_arg,
        robot_state_publisher_node
        #server
    ])
    