cmake_minimum_required(VERSION 3.8)
project(puzzle_solver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
set(CMAKE_CXX_STANDARD 17)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(scara_msgs REQUIRED)

# API for Positions library
add_library(scara_positions SHARED
  src/scara_positions.cpp
)

add_library(puzzle_solver::scara_positions ALIAS scara_positions)

target_include_directories(scara_positions PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(scara_positions scara_msgs)

# executable
add_executable(puzzle_solver_node
  src/puzzle_solver_node.cpp
  src/puzzle_matching.cpp
  src/puzzle_processing.cpp
  src/puzzle_utils.cpp
)
target_include_directories(puzzle_solver_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(puzzle_solver_node
  scara_positions
  Qt5::Core Qt5::Gui Qt5::Widgets
  ${OpenCV_LIBS}
)
ament_target_dependencies(puzzle_solver_node
  ament_index_cpp
  rclcpp rclcpp_action
  sensor_msgs std_msgs
  cv_bridge OpenCV scara_msgs
)

install(TARGETS scara_positions
  EXPORT export_puzzle_solverTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)
install(TARGETS puzzle_solver_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_export_include_directories(include)
ament_export_targets(export_puzzle_solverTargets HAS_LIBRARY_TARGET)
ament_export_dependencies(scara_msgs)

ament_package()
