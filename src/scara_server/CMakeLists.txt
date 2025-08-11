cmake_minimum_required(VERSION 3.8)
project(scara_server)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(task_server_node
    src/task_server_node.cpp
)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(action_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(scara_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(puzzle_solver REQUIRED)

ament_target_dependencies(task_server_node
  rclcpp
  rclcpp_action
  scara_msgs
  sensor_msgs
  geometry_msgs
  puzzle_solver
)

target_link_libraries(task_server_node
  puzzle_solver::scara_positions
)

target_include_directories(task_server_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS 
  task_server_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

ament_package()
