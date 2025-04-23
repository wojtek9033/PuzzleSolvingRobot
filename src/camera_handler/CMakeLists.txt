cmake_minimum_required(VERSION 3.8)
project(camera_handler)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(
  include
  /usr/local/include/libcamera-apps
  /usr/local/include/rpicam-apps
  /usr/local/include/libcamera
)

link_directories(
  /usr/local/lib
)

# find dependencies
find_package(ament_cmake REQUIRED)

find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(camera_node src/camera_node.cpp)

target_link_libraries(camera_node
  rpicam_app
  Boost::program_options
)

ament_target_dependencies( camera_node
  rclcpp
  sensor_msgs
  std_msgs
  cv_bridge
  OpenCV
  image_transport
)

install(TARGETS
  camera_node
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)


  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
