cmake_minimum_required(VERSION 3.8)
project(puzzle_solver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find necessary dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Include directories
include_directories(include ${OpenCV_INCLUDE_DIRS} ${Qt5_INCLUDE_DIRS})

# Define the executable
add_executable(puzzle_solver_node
    src/puzzle_solver_node.cpp
    src/puzzle_matching.cpp
    src/puzzle_processing.cpp
    src/puzzle_utils.cpp
)

# Link libraries
target_link_libraries(puzzle_solver_node
    ${OpenCV_LIBS}
    ${Qt5_LIBRARIES}
)

# ROS2 package dependencies
ament_target_dependencies(puzzle_solver_node
    rclcpp
    sensor_msgs
    cv_bridge
    OpenCV
)

install(TARGETS 
    puzzle_solver_node 
    DESTINATION lib/${PROJECT_NAME})

ament_package()

