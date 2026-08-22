#include <functional>
#include <memory>
#include <chrono>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

class VizMarkerRelayNode : public rclcpp::Node
{
public:
  VizMarkerRelayNode()
  : rclcpp::Node("viz_marker_relay"),
    messages_sent_(0),
    messages_received_(0),
    total_publish_time_ns_(0),
    start_time_(std::chrono::steady_clock::now())
  {
    publisher_ = this->create_publisher<visualization_msgs::msg::MarkerArray>(
      "viz_marker_test_recv", rclcpp::SystemDefaultsQoS());

    subscriber_ = this->create_subscription<visualization_msgs::msg::MarkerArray>(
      "viz_marker_test_send",
      rclcpp::SystemDefaultsQoS(),
      std::bind(&VizMarkerRelayNode::on_msg, this, std::placeholders::_1));

    // Create timer for statistics reporting
    stats_timer_ = this->create_wall_timer(
      std::chrono::seconds(1),
      std::bind(&VizMarkerRelayNode::report_stats, this));

    RCLCPP_INFO(this->get_logger(), "Relaying viz_marker_test_send -> viz_marker_test_recv");
  }

private:
  void on_msg(const visualization_msgs::msg::MarkerArray::SharedPtr msg)
  {
    // Time the publish call
    auto start_time = std::chrono::high_resolution_clock::now();
    
    publisher_->publish(*msg);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    
    messages_received_++;
    messages_sent_++;
    total_publish_time_ns_ += duration.count();
  }
  
  void report_stats()
  {
    double avg_publish_time_ms = messages_sent_ > 0 ?
      (static_cast<double>(total_publish_time_ns_) / messages_sent_ / 1e6) : 0.0;

    auto now = std::chrono::steady_clock::now();
    double elapsed_sec = std::chrono::duration<double>(now - start_time_).count();
    double msgs_per_sec_sent = elapsed_sec > 0 ? messages_sent_ / elapsed_sec : 0.0;
    double msgs_per_sec_recv = elapsed_sec > 0 ? messages_received_ / elapsed_sec : 0.0;

    RCLCPP_INFO(get_logger(),
      "Stats - Sent: %zu (%.1f msg/s), Received: %zu (%.1f msg/s), Avg Publish Time: %.3f ms",
      messages_sent_, msgs_per_sec_sent, messages_received_, msgs_per_sec_recv, avg_publish_time_ms);
  }

  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr publisher_;
  rclcpp::Subscription<visualization_msgs::msg::MarkerArray>::SharedPtr subscriber_;
  rclcpp::TimerBase::SharedPtr stats_timer_;
  
  // Statistics tracking
  size_t messages_sent_;
  size_t messages_received_;
  uint64_t total_publish_time_ns_;
  std::chrono::steady_clock::time_point start_time_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<VizMarkerRelayNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
