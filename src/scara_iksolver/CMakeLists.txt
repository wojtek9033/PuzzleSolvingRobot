cmake_minimum_required(VERSION 3.8)
project(scara_iksolver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(urdf REQUIRED)
find_package(kdl_parser REQUIRED)
find_package(orocos_kdl REQUIRED)

add_executable(scara_ik_node
  src/scara_ik_node.cpp
)

ament_target_dependencies(scara_ik_node
  rclcpp
  std_msgs
  geometry_msgs
  urdf
  kdl_parser
  orocos_kdl
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

install(TARGETS
  scara_ik_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
