#include <functional>
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

class VizMarkerRelayNode : public rclcpp::Node
{
public:
  VizMarkerRelayNode()
  : rclcpp::Node("viz_marker_relay")
  {
    publisher_ = this->create_publisher<visualization_msgs::msg::MarkerArray>(
      "viz_marker_test_recv", rclcpp::SystemDefaultsQoS());

    subscriber_ = this->create_subscription<visualization_msgs::msg::MarkerArray>(
      "viz_marker_test_send",
      rclcpp::SystemDefaultsQoS(),
      std::bind(&VizMarkerRelayNode::on_msg, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Relaying viz_marker_test_send -> viz_marker_test_recv");
  }

private:
  void on_msg(const visualization_msgs::msg::MarkerArray::SharedPtr msg)
  {
    publisher_->publish(*msg);
  }

  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr publisher_;
  rclcpp::Subscription<visualization_msgs::msg::MarkerArray>::SharedPtr subscriber_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<VizMarkerRelayNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
