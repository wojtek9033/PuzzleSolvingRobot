cmake_minimum_required(VERSION 3.8)
project(puzzle_tuner)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(scara_msgs REQUIRED)
find_package(rclcpp_action REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS} ${Qt5_INCLUDE_DIRS})

add_executable(puzzle_tuner_node
  src/tuner_node.cpp
)

target_link_libraries(puzzle_tuner_node
    ${OpenCV_LIBS}
    ${Qt5_LIBRARIES}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# ROS2 package dependencies
ament_target_dependencies(puzzle_tuner_node
    rclcpp sensor_msgs
    rclcpp_action
    std_msgs
    cv_bridge
    OpenCV
    scara_msgs
)
ament_target_dependencies(puzzle_tuner_node ament_index_cpp)

install(TARGETS 
    puzzle_tuner_node 
    DESTINATION lib/${PROJECT_NAME}
)

ament_package()
