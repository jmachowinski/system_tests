// Copyright 2024 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int8.hpp>
#include <std_msgs/msg/u_int8.hpp>
#include <std_msgs/msg/int16.hpp>
#include <std_msgs/msg/u_int16.hpp>
#include <std_msgs/msg/int32.hpp>
#include <std_msgs/msg/u_int32.hpp>
#include <std_msgs/msg/int64.hpp>
#include <std_msgs/msg/u_int64.hpp>
#include <std_msgs/msg/float32.hpp>
#include <std_msgs/msg/float64.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/string.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

class StdMsgsCommunicationTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rclcpp::init(0, nullptr);
  }

  void TearDown() override
  {
    rclcpp::shutdown();
  }
};

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveInt8)
{
  auto node = rclcpp::Node::make_shared("test_int8_communication");
  const int8_t test_value = 42;
  int8_t received_value = 0;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::Int8>(
    "test_int8_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::Int8::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::Int8>("test_int8_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::Int8 message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveUInt8)
{
  auto node = rclcpp::Node::make_shared("test_uint8_communication");
  const uint8_t test_value = 200;
  uint8_t received_value = 0;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::UInt8>(
    "test_uint8_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::UInt8::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::UInt8>("test_uint8_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::UInt8 message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveInt16)
{
  auto node = rclcpp::Node::make_shared("test_int16_communication");
  const int16_t test_value = 12345;
  int16_t received_value = 0;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::Int16>(
    "test_int16_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::Int16::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::Int16>("test_int16_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::Int16 message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveUInt16)
{
  auto node = rclcpp::Node::make_shared("test_uint16_communication");
  const uint16_t test_value = 50000;
  uint16_t received_value = 0;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::UInt16>(
    "test_uint16_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::UInt16::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::UInt16>("test_uint16_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::UInt16 message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveInt32)
{
  auto node = rclcpp::Node::make_shared("test_int32_communication");
  const int32_t test_value = 123456789;
  int32_t received_value = 0;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::Int32>(
    "test_int32_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::Int32::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::Int32>("test_int32_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::Int32 message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveUInt32)
{
  auto node = rclcpp::Node::make_shared("test_uint32_communication");
  const uint32_t test_value = 3456789012;
  uint32_t received_value = 0;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::UInt32>(
    "test_uint32_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::UInt32::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::UInt32>("test_uint32_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::UInt32 message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveInt64)
{
  auto node = rclcpp::Node::make_shared("test_int64_communication");
  const int64_t test_value = 1234567890123456789LL;
  int64_t received_value = 0;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::Int64>(
    "test_int64_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::Int64::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::Int64>("test_int64_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::Int64 message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveUInt64)
{
  auto node = rclcpp::Node::make_shared("test_uint64_communication");
  const uint64_t test_value = 12345678901234567890ULL;
  uint64_t received_value = 0;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::UInt64>(
    "test_uint64_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::UInt64::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::UInt64>("test_uint64_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::UInt64 message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveFloat32)
{
  auto node = rclcpp::Node::make_shared("test_float32_communication");
  const float test_value = 3.14159f;
  float received_value = 0.0f;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::Float32>(
    "test_float32_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::Float32::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::Float32>("test_float32_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::Float32 message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_FLOAT_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveFloat64)
{
  auto node = rclcpp::Node::make_shared("test_float64_communication");
  const double test_value = 2.718281828459045;
  double received_value = 0.0;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::Float64>(
    "test_float64_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::Float64::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::Float64>("test_float64_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::Float64 message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_DOUBLE_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveBool)
{
  auto node = rclcpp::Node::make_shared("test_bool_communication");
  const bool test_value = true;
  bool received_value = false;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::Bool>(
    "test_bool_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::Bool::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::Bool>("test_bool_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::Bool message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveString)
{
  auto node = rclcpp::Node::make_shared("test_string_communication");
  const std::string test_value = "Hello, ROS 2!";
  std::string received_value;
  bool message_received = false;

  auto subscriber = node->create_subscription<std_msgs::msg::String>(
    "test_string_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::String::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  auto publisher = node->create_publisher<std_msgs::msg::String>("test_string_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  std_msgs::msg::String message;
  message.data = test_value;
  publisher->publish(message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_value, test_value) << "Received value does not match sent value";
}

TEST_F(StdMsgsCommunicationTest, PublishAndReceiveMarkerArray)
{
  auto node = rclcpp::Node::make_shared("test_marker_array_communication");
  visualization_msgs::msg::MarkerArray test_message;
  visualization_msgs::msg::MarkerArray received_message;
  bool message_received = false;

  // Create a simple marker
  visualization_msgs::msg::Marker marker;
  marker.header.frame_id = "test_frame";
  marker.header.stamp.sec = 123;
  marker.header.stamp.nanosec = 456789;
  marker.ns = "test_namespace";
  marker.id = 42;
  marker.type = visualization_msgs::msg::Marker::SPHERE;
  marker.action = visualization_msgs::msg::Marker::ADD;
  marker.pose.position.x = 1.0;
  marker.pose.position.y = 2.0;
  marker.pose.position.z = 3.0;
  marker.pose.orientation.x = 0.0;
  marker.pose.orientation.y = 0.0;
  marker.pose.orientation.z = 0.0;
  marker.pose.orientation.w = 1.0;
  marker.scale.x = 0.5;
  marker.scale.y = 0.5;
  marker.scale.z = 0.5;
  marker.color.r = 1.0;
  marker.color.g = 0.0;
  marker.color.b = 0.0;
  marker.color.a = 1.0;
  test_message.markers.push_back(marker);

  auto subscriber = node->create_subscription<visualization_msgs::msg::MarkerArray>(
    "test_marker_array_topic", 10,
    [&received_message, &message_received](const visualization_msgs::msg::MarkerArray::SharedPtr msg) {
      received_message = *msg;
      message_received = true;
    });

  auto publisher = node->create_publisher<visualization_msgs::msg::MarkerArray>("test_marker_array_topic", 10);

  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  publisher->publish(test_message);

  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_message.markers.size(), test_message.markers.size()) << "Marker count mismatch";
  if (received_message.markers.size() == test_message.markers.size()) {
    EXPECT_EQ(received_message.markers[0].header.frame_id, test_message.markers[0].header.frame_id);
    EXPECT_EQ(received_message.markers[0].header.stamp.sec, test_message.markers[0].header.stamp.sec);
    EXPECT_EQ(received_message.markers[0].header.stamp.nanosec, test_message.markers[0].header.stamp.nanosec);
    EXPECT_EQ(received_message.markers[0].ns, test_message.markers[0].ns);
    EXPECT_EQ(received_message.markers[0].id, test_message.markers[0].id);
    EXPECT_EQ(received_message.markers[0].type, test_message.markers[0].type);
    EXPECT_EQ(received_message.markers[0].action, test_message.markers[0].action);
    EXPECT_DOUBLE_EQ(received_message.markers[0].pose.position.x, test_message.markers[0].pose.position.x);
    EXPECT_DOUBLE_EQ(received_message.markers[0].pose.position.y, test_message.markers[0].pose.position.y);
    EXPECT_DOUBLE_EQ(received_message.markers[0].pose.position.z, test_message.markers[0].pose.position.z);
    EXPECT_DOUBLE_EQ(received_message.markers[0].pose.orientation.w, test_message.markers[0].pose.orientation.w);
    EXPECT_DOUBLE_EQ(received_message.markers[0].scale.x, test_message.markers[0].scale.x);
    EXPECT_DOUBLE_EQ(received_message.markers[0].scale.y, test_message.markers[0].scale.y);
    EXPECT_DOUBLE_EQ(received_message.markers[0].scale.z, test_message.markers[0].scale.z);
    EXPECT_DOUBLE_EQ(received_message.markers[0].color.r, test_message.markers[0].color.r);
    EXPECT_DOUBLE_EQ(received_message.markers[0].color.g, test_message.markers[0].color.g);
    EXPECT_DOUBLE_EQ(received_message.markers[0].color.b, test_message.markers[0].color.b);
    EXPECT_DOUBLE_EQ(received_message.markers[0].color.a, test_message.markers[0].color.a);
  }
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
