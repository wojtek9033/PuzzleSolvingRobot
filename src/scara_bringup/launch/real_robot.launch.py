import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription,DeclareLaunchArgument
from launch.substitutions import Command, LaunchConfiguration
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from launch.conditions import IfCondition
from launch.actions import TimerAction

def generate_launch_description():

    scara_description_dir = get_package_share_directory('scara_description')

    model_arg = DeclareLaunchArgument(
        name='model', default_value=os.path.join(
        scara_description_dir, 'urdf', 'scara.urdf.xacro'
        ),
        description='Absolute path to robot urdf file'
    )
    
    calibration_arg = DeclareLaunchArgument(
        name='calibration',
        default_value='False',
        description='Start in calibration mode'
    )

    robot_description = ParameterValue(
        Command([
            'xacro ',
            LaunchConfiguration('model'),
            " is_sim:=False"
            ]),
            value_type=str
        )

    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description': robot_description,
                    'use_sim_time': False}]
    )

    controller = TimerAction(
        period=3.0,
        actions=[IncludeLaunchDescription(
                os.path.join(
                    get_package_share_directory("scara_controller"),
                    "launch",
                    "controller.launch.py"
            ),
            launch_arguments={
                "is_sim": "False",
                "robot_description": str(robot_description)
            }.items()
        )]
    )

    iksolver = TimerAction(
        period=6.0,
        actions=[IncludeLaunchDescription(
                os.path.join(
                    get_package_share_directory("scara_kinematics"),
                    "launch",
                    "scara_kinematics.launch.py"
        ),
        launch_arguments={"is_sim": "False"}.items()
        )]
    )

    server = TimerAction(
        period=6.0,
        actions=[IncludeLaunchDescription(
                os.path.join(
                    get_package_share_directory("scara_server"),
                    "launch",
                    "scara_server.launch.py"
        ))]
    )

    tuner = TimerAction(
        period=6.0,
        actions=[IncludeLaunchDescription(
                os.path.join(
                    get_package_share_directory("puzzle_tuner"),
                    "launch",
                    "tuner.launch.py"
        ),
        condition=IfCondition(LaunchConfiguration('calibration'))
        )]
    )
    
    return LaunchDescription([
        model_arg,
        calibration_arg,
        robot_state_publisher_node,
        controller,
        iksolver,
        server,
        tuner
    ])
    