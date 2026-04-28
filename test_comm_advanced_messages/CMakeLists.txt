cmake_minimum_required(VERSION 3.20)

project(test_comm_advanced_messages)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

find_package(benchmark REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosidl_typesupport_fastrtps_c REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tf2_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)

add_executable(viz_marker_roundtrip_node src/viz_marker_roundtrip_node.cpp)
ament_target_dependencies(viz_marker_roundtrip_node
  geometry_msgs
  rclcpp
  std_msgs
  visualization_msgs
)

add_executable(viz_marker_relay_node src/viz_marker_relay_node.cpp)
ament_target_dependencies(viz_marker_relay_node
  rclcpp
  visualization_msgs
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  add_executable(benchmark_serialization test/benchmark_serialization.cpp)
  target_link_libraries(benchmark_serialization benchmark::benchmark)
  ament_target_dependencies(benchmark_serialization
    geometry_msgs
    rosidl_typesupport_fastrtps_c
    std_msgs
    tf2_msgs
    visualization_msgs
  )
  install(TARGETS benchmark_serialization DESTINATION lib/${PROJECT_NAME})

  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(test_placeholder test/test_placeholder.cpp)
  ament_add_gtest(test_int16_communication test/test_int16_communication.cpp)
  ament_target_dependencies(test_int16_communication
    rclcpp
    std_msgs
  )
  ament_add_gtest(test_std_msgs_communication test/test_std_msgs_communication.cpp)
  ament_target_dependencies(test_std_msgs_communication
    rclcpp
    std_msgs
    visualization_msgs
  )
endif()

ament_package()
