cmake_minimum_required(VERSION 3.8)
project(scara_kinematics)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(urdf REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(tf2 REQUIRED)
find_package(yaml-cpp REQUIRED)

add_executable(scara_kinematics_node
src/scara_kinematics_node.cpp
)

ament_target_dependencies(scara_kinematics_node
  rclcpp
  std_msgs
  sensor_msgs
  geometry_msgs
  urdf
  Eigen3
  tf2
  ament_index_cpp
)

target_link_libraries(scara_kinematics_node
  yaml-cpp
)

target_include_directories(scara_kinematics_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
)

install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

install(TARGETS
  scara_kinematics_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
