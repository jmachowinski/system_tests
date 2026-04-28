#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include <rcl/time.h>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/quaternion.hpp>
#include <geometry_msgs/msg/vector3.hpp>
#include <std_msgs/msg/color_rgba.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

namespace
{
using visualization_msgs::msg::Marker;
using visualization_msgs::msg::MarkerArray;

geometry_msgs::msg::Point make_point(std::mt19937 & rng, double min_v, double max_v)
{
  std::uniform_real_distribution<double> dist(min_v, max_v);
  geometry_msgs::msg::Point p;
  p.x = dist(rng);
  p.y = dist(rng);
  p.z = dist(rng);
  return p;
}

std_msgs::msg::ColorRGBA make_color(std::mt19937 & rng)
{
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);
  std_msgs::msg::ColorRGBA c;
  c.r = dist(rng);
  c.g = dist(rng);
  c.b = dist(rng);
  c.a = 1.0f;
  return c;
}

Marker make_marker(std::mt19937 & rng, int32_t seq, int32_t marker_index)
{
  std::uniform_int_distribution<int> type_dist(0, 2);
  const int type_choice = type_dist(rng);

  Marker m;
  m.header.frame_id = "map";
  m.header.stamp = rclcpp::Time(0, 0, RCL_ROS_TIME);
  m.ns = "test_comm_advanced_messages";
  m.id = seq;
  m.action = Marker::ADD;

  if (type_choice == 0) {
    m.type = Marker::LINE_STRIP;
    m.scale.x = 0.03;
    m.color = make_color(rng);

    std::uniform_int_distribution<int> n_points_dist(3, 30);
    const int n_points = n_points_dist(rng);
    m.points.reserve(static_cast<size_t>(n_points));
    for (int i = 0; i < n_points; ++i) {
      m.points.push_back(make_point(rng, -5.0, 5.0));
    }
  } else if (type_choice == 1) {
    m.type = Marker::SPHERE;
    m.pose.position = make_point(rng, -5.0, 5.0);
    m.pose.orientation.w = 1.0;
    std::uniform_real_distribution<double> scale_dist(0.05, 1.5);
    const double s = scale_dist(rng);
    m.scale.x = s;
    m.scale.y = s;
    m.scale.z = s;
    m.color = make_color(rng);
  } else {
    m.type = Marker::CUBE;
    m.pose.position = make_point(rng, -5.0, 5.0);
    m.pose.orientation.w = 1.0;
    std::uniform_real_distribution<double> scale_dist(0.05, 2.0);
    m.scale.x = scale_dist(rng);
    m.scale.y = scale_dist(rng);
    m.scale.z = scale_dist(rng);
    m.color = make_color(rng);
  }

  m.lifetime = rclcpp::Duration(0, 0);
  m.frame_locked = false;

  std::ostringstream ss;
  ss << "marker_index=" << marker_index;
  m.text = ss.str();

  return m;
}

bool equal_point(const geometry_msgs::msg::Point & a, const geometry_msgs::msg::Point & b)
{
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool equal_quat(const geometry_msgs::msg::Quaternion & a, const geometry_msgs::msg::Quaternion & b)
{
  return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

bool equal_pose(const geometry_msgs::msg::Pose & a, const geometry_msgs::msg::Pose & b)
{
  return equal_point(a.position, b.position) && equal_quat(a.orientation, b.orientation);
}

bool equal_vector3(const geometry_msgs::msg::Vector3 & a, const geometry_msgs::msg::Vector3 & b)
{
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool equal_color(const std_msgs::msg::ColorRGBA & a, const std_msgs::msg::ColorRGBA & b)
{
  return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

bool equal_marker(const Marker & a, const Marker & b)
{
  if (a.header.frame_id != b.header.frame_id) {
    return false;
  }
  if (a.ns != b.ns || a.id != b.id || a.type != b.type || a.action != b.action) {
    return false;
  }
  if (!equal_pose(a.pose, b.pose)) {
    return false;
  }
  if (!equal_vector3(a.scale, b.scale)) {
    return false;
  }
  if (!equal_color(a.color, b.color)) {
    return false;
  }
  if (
    a.lifetime.sec != b.lifetime.sec ||
    a.lifetime.nanosec != b.lifetime.nanosec ||
    a.frame_locked != b.frame_locked)
  {
    return false;
  }
  if (a.points.size() != b.points.size()) {
    return false;
  }
  for (size_t i = 0; i < a.points.size(); ++i) {
    if (!equal_point(a.points[i], b.points[i])) {
      return false;
    }
  }
  if (a.colors.size() != b.colors.size()) {
    return false;
  }
  for (size_t i = 0; i < a.colors.size(); ++i) {
    if (!equal_color(a.colors[i], b.colors[i])) {
      return false;
    }
  }
  if (a.text != b.text) {
    return false;
  }
  if (a.mesh_resource != b.mesh_resource) {
    return false;
  }
  if (a.mesh_use_embedded_materials != b.mesh_use_embedded_materials) {
    return false;
  }

  return true;
}

bool equal_marker_array(const MarkerArray & a, const MarkerArray & b)
{
  if (a.markers.size() != b.markers.size()) {
    return false;
  }
  for (size_t i = 0; i < a.markers.size(); ++i) {
    if (!equal_marker(a.markers[i], b.markers[i])) {
      return false;
    }
  }
  return true;
}

}  // namespace

class VizMarkerRoundtripNode : public rclcpp::Node
{
public:
  VizMarkerRoundtripNode()
  : rclcpp::Node("viz_marker_roundtrip_test"),
    messages_sent_(0),
    messages_received_(0),
    total_publish_time_ns_(0)
  {
    message_count_ = this->declare_parameter<int>("message_count", 10000);
    markers_per_message_ = this->declare_parameter<int>("markers_per_message", 500);
    timer_period_ms_ = this->declare_parameter<int>("timer_period_ms", 2);
    rng_seed_ = this->declare_parameter<int>("rng_seed", 12345);

    if (message_count_ <= 0) {
      throw std::runtime_error("message_count must be > 0");
    }
    if (markers_per_message_ <= 0) {
      throw std::runtime_error("markers_per_message must be > 0");
    }
    if (timer_period_ms_ <= 0) {
      throw std::runtime_error("timer_period_ms must be > 0");
    }

    publisher_ = this->create_publisher<MarkerArray>("viz_marker_test_send", rclcpp::SystemDefaultsQoS());
    subscriber_ = this->create_subscription<MarkerArray>(
      "viz_marker_test_recv",
      rclcpp::SystemDefaultsQoS(),
      std::bind(&VizMarkerRoundtripNode::on_recv, this, std::placeholders::_1));

    pregenerate_messages();

    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(timer_period_ms_),
      std::bind(&VizMarkerRoundtripNode::on_timer, this));

    // Create timer for statistics reporting
    stats_timer_ = this->create_wall_timer(
      std::chrono::seconds(1),
      std::bind(&VizMarkerRoundtripNode::report_stats, this));

    RCLCPP_INFO(
      this->get_logger(),
      "Started. Publishing %d pre-generated MarkerArray messages round-robin (markers_per_message=%d) every %d ms",
      message_count_, markers_per_message_, timer_period_ms_);
  }

private:
  void pregenerate_messages()
  {
    std::mt19937 rng(static_cast<uint32_t>(rng_seed_));

    messages_.clear();
    messages_.reserve(static_cast<size_t>(message_count_));
    for (int i = 0; i < message_count_; ++i) {
      MarkerArray arr;
      arr.markers.reserve(static_cast<size_t>(markers_per_message_));
      for (int j = 0; j < markers_per_message_; ++j) {
        arr.markers.push_back(make_marker(rng, i, j));
      }
      messages_.push_back(std::move(arr));
    }

    publish_index_ = 0;
    last_sent_seq_ = -1;
  }

  void on_timer()
  {
    // Time the publish call
    auto start_time = std::chrono::high_resolution_clock::now();
    
    const int seq = publish_index_;
    publisher_->publish(messages_[static_cast<size_t>(publish_index_)]);
    last_sent_seq_ = seq;
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    
    messages_sent_++;
    total_publish_time_ns_ += duration.count();

    publish_index_ = (publish_index_ + 1) % message_count_;
  }

  void report_stats()
  {
    double avg_publish_time_ms = messages_sent_ > 0 ? 
      (static_cast<double>(total_publish_time_ns_) / messages_sent_ / 1e6) : 0.0;
    
    RCLCPP_INFO(get_logger(), 
      "Stats - Sent: %zu, Received: %zu, Avg Publish Time: %.3f ms, Matches: %zu, Mismatches: %zu",
      messages_sent_, messages_received_, avg_publish_time_ms, match_count_, mismatch_count_);
  }

  void on_recv(const MarkerArray::SharedPtr msg)
  {
    messages_received_++;
    
    if (msg->markers.empty()) {
      RCLCPP_ERROR(this->get_logger(), "Received MarkerArray with 0 markers");
      ++mismatch_count_;
      return;
    }

    const auto & first = msg->markers[0];
    const int seq = first.id;

    if (seq != last_sent_seq_) {
      RCLCPP_ERROR(
        this->get_logger(),
        "Received seq=%d but last sent seq=%d",
        seq, last_sent_seq_);
      ++mismatch_count_;
      return;
    }

    if (seq < 0 || seq >= message_count_) {
      RCLCPP_ERROR(this->get_logger(), "Received MarkerArray with out-of-range seq=%d", seq);
      ++mismatch_count_;
      return;
    }

    if (first.ns != "test_comm_advanced_messages") {
      RCLCPP_ERROR(this->get_logger(), "Received MarkerArray with unexpected namespace '%s'", first.ns.c_str());
      ++mismatch_count_;
      return;
    }

    const auto & expected = messages_[static_cast<size_t>(seq)];
    if (!equal_marker_array(*msg, expected)) {
      RCLCPP_ERROR(
        this->get_logger(),
        "Mismatch for seq=%d (last_sent_seq=%d) markers=%zu expected=%zu",
        seq, last_sent_seq_, msg->markers.size(), expected.markers.size());
      ++mismatch_count_;
      return;
    }

    ++match_count_;
  }

  int message_count_{1000};
  int markers_per_message_{5};
  int timer_period_ms_{50};
  int rng_seed_{12345};

  int publish_index_{0};
  int last_sent_seq_{-1};

  size_t match_count_{0};
  size_t mismatch_count_{0};

  std::vector<MarkerArray> messages_;

  rclcpp::Publisher<MarkerArray>::SharedPtr publisher_;
  rclcpp::Subscription<MarkerArray>::SharedPtr subscriber_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::TimerBase::SharedPtr stats_timer_;
  
  // Statistics tracking
  size_t messages_sent_;
  size_t messages_received_;
  uint64_t total_publish_time_ns_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<VizMarkerRoundtripNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
