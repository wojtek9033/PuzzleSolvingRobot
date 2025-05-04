cmake_minimum_required(VERSION 3.8)
project(scara_server)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(puzzle_task_server
    src/puzzle_task_server.cpp
)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(action_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(scara_msgs REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)

ament_target_dependencies(puzzle_task_server
  rclcpp
  rclcpp_action
  scara_msgs
  moveit_ros_planning_interface
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

install(TARGETS 
  puzzle_task_server
  DESTINATION lib/${PROJECT_NAME}
)
ament_package()
