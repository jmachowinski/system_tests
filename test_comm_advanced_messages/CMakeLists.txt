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
find_package(fastcdr REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rcl REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosidl_typesupport_fastrtps_c REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tf2_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)

add_executable(viz_marker_roundtrip_node src/viz_marker_roundtrip_node.cpp)
target_link_libraries(viz_marker_roundtrip_node
  geometry_msgs::geometry_msgs__rosidl_typesupport_cpp
  rcl::rcl
  rclcpp::rclcpp
  std_msgs::std_msgs__rosidl_typesupport_cpp
  visualization_msgs::visualization_msgs__rosidl_typesupport_cpp
)

add_executable(viz_marker_relay_node src/viz_marker_relay_node.cpp)
target_link_libraries(viz_marker_relay_node
  rclcpp::rclcpp
  visualization_msgs::visualization_msgs__rosidl_typesupport_cpp
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  add_executable(benchmark_serialization test/benchmark_serialization.cpp)
  target_link_libraries(benchmark_serialization benchmark::benchmark)
  target_link_libraries(benchmark_serialization
    fastcdr
    geometry_msgs::geometry_msgs__rosidl_typesupport_fastrtps_cpp
    geometry_msgs::geometry_msgs__rosidl_typesupport_fastrtps_c
    rosidl_typesupport_fastrtps_c::rosidl_typesupport_fastrtps_c
    std_msgs::std_msgs__rosidl_typesupport_fastrtps_cpp
    std_msgs::std_msgs__rosidl_typesupport_fastrtps_c
    tf2_msgs::tf2_msgs__rosidl_typesupport_fastrtps_cpp
    tf2_msgs::tf2_msgs__rosidl_typesupport_fastrtps_c
    visualization_msgs::visualization_msgs__rosidl_typesupport_fastrtps_cpp
    visualization_msgs::visualization_msgs__rosidl_typesupport_fastrtps_c
  )
  install(TARGETS benchmark_serialization DESTINATION lib/${PROJECT_NAME})

  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(test_placeholder test/test_placeholder.cpp)
  ament_add_gtest(test_int16_communication test/test_int16_communication.cpp)
  target_link_libraries(test_int16_communication
    rclcpp::rclcpp
    std_msgs::std_msgs__rosidl_typesupport_cpp
  )
  ament_add_gtest(test_std_msgs_communication test/test_std_msgs_communication.cpp)
  target_link_libraries(test_std_msgs_communication
    rclcpp::rclcpp
    std_msgs::std_msgs__rosidl_typesupport_cpp
    visualization_msgs::visualization_msgs__rosidl_typesupport_cpp
  )
endif()

ament_package()
