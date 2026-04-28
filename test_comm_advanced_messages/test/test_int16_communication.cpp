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
#include <std_msgs/msg/int16.hpp>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

class Int16CommunicationTest : public ::testing::Test
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

TEST_F(Int16CommunicationTest, PublishAndReceiveInt16)
{
  // Create a node
  auto node = rclcpp::Node::make_shared("test_int16_communication");

  // Test value to send
  const int16_t test_value = 42;
  int16_t received_value = 0;
  bool message_received = false;

  // Create a subscriber
  auto subscriber = node->create_subscription<std_msgs::msg::Int16>(
    "test_int16_topic", 10,
    [&received_value, &message_received](const std_msgs::msg::Int16::SharedPtr msg) {
      received_value = msg->data;
      message_received = true;
    });

  // Create a publisher
  auto publisher = node->create_publisher<std_msgs::msg::Int16>("test_int16_topic", 10);

  // Wait for the publisher to have subscribers
  auto start_time = std::chrono::steady_clock::now();
  while (publisher->get_subscription_count() == 0 &&
         std::chrono::steady_clock::now() - start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  // Publish the test message
  std_msgs::msg::Int16 message;
  message.data = test_value;
  publisher->publish(message);

  // Spin the node to process the message
  auto receive_start_time = std::chrono::steady_clock::now();
  while (!message_received &&
    std::chrono::steady_clock::now() - receive_start_time < 5s)
  {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(10ms);
  }

  // Verify the message was received and the value matches
  ASSERT_TRUE(message_received) << "Message was not received within timeout";
  EXPECT_EQ(received_value, test_value) << "Received value does not match sent value";
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
