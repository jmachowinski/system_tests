#!/usr/bin/env python3
# Copyright 2024 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import random
import time
from typing import List

import rclpy
from rclpy.executors import SingleThreadedExecutor
# from rclpy.experimental import EventsExecutor
from rclpy.node import Node
from geometry_msgs.msg import Point, Pose, Quaternion, Vector3
from std_msgs.msg import ColorRGBA
from visualization_msgs.msg import Marker, MarkerArray


def make_point(rng: random.Random, min_v: float, max_v: float) -> Point:
    """Create a random Point."""
    p = Point()
    p.x = rng.uniform(min_v, max_v)
    p.y = rng.uniform(min_v, max_v)
    p.z = rng.uniform(min_v, max_v)
    return p


def make_color(rng: random.Random) -> ColorRGBA:
    """Create a random ColorRGBA."""
    c = ColorRGBA()
    c.r = rng.random()
    c.g = rng.random()
    c.b = rng.random()
    c.a = 1.0
    return c


def make_marker(rng: random.Random, seq: int, marker_index: int) -> Marker:
    """Create a random Marker."""
    type_choice = rng.randint(0, 2)

    m = Marker()
    m.header.frame_id = 'map'
    m.header.stamp = rclpy.time.Time(seconds=0, nanoseconds=0).to_msg()
    m.ns = 'test_comm_advanced_messages'
    m.id = seq
    m.action = Marker.ADD

    if type_choice == 0:
        m.type = Marker.LINE_STRIP
        m.scale.x = 0.03
        m.color = make_color(rng)

        n_points = rng.randint(3, 30)
        m.points = [make_point(rng, -5.0, 5.0) for _ in range(n_points)]
    elif type_choice == 1:
        m.type = Marker.SPHERE
        m.pose.position = make_point(rng, -5.0, 5.0)
        m.pose.orientation.w = 1.0
        s = rng.uniform(0.05, 1.5)
        m.scale.x = s
        m.scale.y = s
        m.scale.z = s
        m.color = make_color(rng)
    else:
        m.type = Marker.CUBE
        m.pose.position = make_point(rng, -5.0, 5.0)
        m.pose.orientation.w = 1.0
        m.scale.x = rng.uniform(0.05, 2.0)
        m.scale.y = rng.uniform(0.05, 2.0)
        m.scale.z = rng.uniform(0.05, 2.0)
        m.color = make_color(rng)

    m.lifetime.sec = 0
    m.lifetime.nanosec = 0
    m.frame_locked = False
    m.text = f'marker_index={marker_index}'

    return m


def equal_point(a: Point, b: Point) -> bool:
    """Check if two Points are equal."""
    return a.x == b.x and a.y == b.y and a.z == b.z


def equal_quat(a: Quaternion, b: Quaternion) -> bool:
    """Check if two Quaternions are equal."""
    return a.x == b.x and a.y == b.y and a.z == b.z and a.w == b.w


def equal_pose(a: Pose, b: Pose) -> bool:
    """Check if two Poses are equal."""
    return (equal_point(a.position, b.position) and
            equal_quat(a.orientation, b.orientation))


def equal_vector3(a: Vector3, b: Vector3) -> bool:
    """Check if two Vector3s are equal."""
    return a.x == b.x and a.y == b.y and a.z == b.z


def equal_color(a: ColorRGBA, b: ColorRGBA) -> bool:
    """Check if two ColorRGBAs are equal."""
    return a.r == b.r and a.g == b.g and a.b == b.b and a.a == b.a


def equal_marker(a: Marker, b: Marker) -> bool:
    """Check if two Markers are equal."""
    if a.header.frame_id != b.header.frame_id:
        return False
    if (a.ns != b.ns or a.id != b.id or
            a.type != b.type or a.action != b.action):
        return False
    if not equal_pose(a.pose, b.pose):
        return False
    if not equal_vector3(a.scale, b.scale):
        return False
    if not equal_color(a.color, b.color):
        return False
    if (a.lifetime.sec != b.lifetime.sec or
            a.lifetime.nanosec != b.lifetime.nanosec or
            a.frame_locked != b.frame_locked):
        return False
    if len(a.points) != len(b.points):
        return False
    for i in range(len(a.points)):
        if not equal_point(a.points[i], b.points[i]):
            return False
    if len(a.colors) != len(b.colors):
        return False
    for i in range(len(a.colors)):
        if not equal_color(a.colors[i], b.colors[i]):
            return False
    if a.text != b.text:
        return False
    if a.mesh_resource != b.mesh_resource:
        return False
    if a.mesh_use_embedded_materials != b.mesh_use_embedded_materials:
        return False
    return True


def equal_marker_array(a: MarkerArray, b: MarkerArray) -> bool:
    """Check if two MarkerArrays are equal."""
    if len(a.markers) != len(b.markers):
        return False
    for i in range(len(a.markers)):
        if not equal_marker(a.markers[i], b.markers[i]):
            return False
    return True


class VizMarkerRoundtripNode(Node):
    """Node that performs round-trip testing of MarkerArray messages."""

    def __init__(self):
        super().__init__('viz_marker_roundtrip_test')

        self.declare_parameter('message_count', 1000)
        self.declare_parameter('markers_per_message', 500)
        self.declare_parameter('timer_period_ms', 15)
        self.declare_parameter('rng_seed', 12345)

        self.message_count = self.get_parameter('message_count').value
        self.markers_per_message = (
            self.get_parameter('markers_per_message').value)
        self.timer_period_ms = self.get_parameter('timer_period_ms').value
        self.rng_seed = self.get_parameter('rng_seed').value

        if self.message_count <= 0:
            raise ValueError('message_count must be > 0')
        if self.markers_per_message <= 0:
            raise ValueError('markers_per_message must be > 0')
        if self.timer_period_ms <= 0:
            raise ValueError('timer_period_ms must be > 0')

        self.publisher = self.create_publisher(
            MarkerArray, 'viz_marker_test_send', 10)
        self.subscription = self.create_subscription(
            MarkerArray, 'viz_marker_test_recv', self.on_recv, 10)

        self.pregenerate_messages()

        self.publish_index = 0
        self.last_sent_seq = -1

        self.messages_sent = 0
        self.messages_received = 0
        self.total_publish_time_ns = 0
        self.match_count = 0
        self.mismatch_count = 0

        self.timer = self.create_timer(
            self.timer_period_ms / 1000.0, self.on_timer)

        self.stats_timer = self.create_timer(1.0, self.report_stats)

        self.get_logger().info(
            f'Started. Publishing {self.message_count} pre-generated '
            f'MarkerArray messages round-robin '
            f'(markers_per_message={self.markers_per_message}) '
            f'every {self.timer_period_ms} ms')

    def pregenerate_messages(self):
        """Pre-generate all test messages."""
        rng = random.Random(self.rng_seed)

        self.messages: List[MarkerArray] = []
        for i in range(self.message_count):
            arr = MarkerArray()
            for j in range(self.markers_per_message):
                arr.markers.append(make_marker(rng, i, j))
            self.messages.append(arr)

    def on_timer(self):
        """Timer callback to publish messages."""
        start_time = time.perf_counter_ns()

        seq = self.publish_index
        self.publisher.publish(self.messages[self.publish_index])
        self.last_sent_seq = seq

        end_time = time.perf_counter_ns()
        duration = end_time - start_time

        self.messages_sent += 1
        self.total_publish_time_ns += duration

        self.publish_index = (self.publish_index + 1) % self.message_count

    def report_stats(self):
        """Report statistics periodically."""
        avg_publish_time_ms = (
            (self.total_publish_time_ns / self.messages_sent / 1e6)
            if self.messages_sent > 0 else 0.0)

        self.get_logger().info(
            f'Stats - Sent: {self.messages_sent}, '
            f'Received: {self.messages_received}, '
            f'Avg Publish Time: {avg_publish_time_ms:.3f} ms, '
            f'Matches: {self.match_count}, '
            f'Mismatches: {self.mismatch_count}')

    def on_recv(self, msg: MarkerArray):
        """Callback for received messages."""
        self.messages_received += 1

        if len(msg.markers) == 0:
            self.get_logger().error('Received MarkerArray with 0 markers')
            self.mismatch_count += 1
            return


        self.match_count += 1


def main(args=None):
    """Main entry point."""
    rclpy.init(args=args)
    node = VizMarkerRoundtripNode()
    executor = SingleThreadedExecutor()
    # executor = EventsExecutor()
    try:
        executor.add_node(node)
        executor.spin()
    except KeyboardInterrupt:
        pass
    finally:
        executor.shutdown()
        node.destroy_node()
        rclpy.shutdown()


def profile_deserialization():
    """Profile deserialization in isolation without ROS infrastructure."""
    import cProfile
    import pstats
    import io
    from rclpy.serialization import serialize_message, deserialize_message
    
    print("Creating test data...")
    rng = random.Random(12345)

    # Create test MarkerArray with 500 markers (similar to actual test)
    arr = MarkerArray()
    for i in range(500):
        m = Marker()
        m.header.frame_id = 'map'
        m.ns = 'test_comm_advanced_messages'
        m.id = i
        m.action = Marker.ADD

        # Mix of different marker types for realistic test
        type_choice = rng.randint(0, 2)

        if type_choice == 0:
            # LINE_STRIP with points
            m.type = Marker.LINE_STRIP
            m.scale.x = 0.03
            m.color.r = rng.random()
            m.color.g = rng.random()
            m.color.b = rng.random()
            m.color.a = 1.0

            n_points = rng.randint(3, 30)
            m.points = [make_point(rng, -5.0, 5.0) for _ in range(n_points)]
        elif type_choice == 1:
            # SPHERE
            m.type = Marker.SPHERE
            m.pose.position = make_point(rng, -5.0, 5.0)
            m.pose.orientation.w = 1.0
            s = rng.uniform(0.05, 1.5)
            m.scale.x = s
            m.scale.y = s
            m.scale.z = s
            m.color.r = rng.random()
            m.color.g = rng.random()
            m.color.b = rng.random()
            m.color.a = 1.0
        else:
            # CUBE
            m.type = Marker.CUBE
            m.pose.position = make_point(rng, -5.0, 5.0)
            m.pose.orientation.w = 1.0
            m.scale.x = rng.uniform(0.05, 2.0)
            m.scale.y = rng.uniform(0.05, 2.0)
            m.scale.z = rng.uniform(0.05, 2.0)
            m.color.r = rng.random()
            m.color.g = rng.random()
            m.color.b = rng.random()
            m.color.a = 1.0

        m.lifetime.sec = 0
        m.lifetime.nanosec = 0
        m.frame_locked = False
        m.text = f'marker_index={i}'
        arr.markers.append(m)
    
    print("Serializing...")
    serialized = serialize_message(arr)
    print(f"Serialized size: {len(serialized)} bytes")
    
    print("Starting profiling of deserialization (1000 iterations)...")
    profiler = cProfile.Profile()
    profiler.enable()
    
    # Deserialize 1000 times
    for _ in range(1000):
        deserialized = deserialize_message(serialized, MarkerArray)
    
    profiler.disable()
    
    # Output results
    s = io.StringIO()
    stats = pstats.Stats(profiler, stream=s).sort_stats('cumulative')
    stats.print_stats(30)
    print("\n=== DESERIALIZATION PROFILING RESULTS ===")
    print(s.getvalue())
    print("=== END PROFILING ===")


if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == '--profile-deserialization':
        profile_deserialization()
    else:
        main()
