// Benchmark: FastRTPS CDR serialization throughput for various ROS message types.
// Exercises the inline detail:: code paths introduced by the inlining optimization.
#include <benchmark/benchmark.h>

#include <fastcdr/Cdr.h>
#include <fastcdr/FastBuffer.h>

#include <geometry_msgs/msg/detail/pose__rosidl_typesupport_fastrtps_cpp.hpp>
#include <geometry_msgs/msg/detail/pose__struct.hpp>
#include <geometry_msgs/msg/detail/transform_stamped__rosidl_typesupport_fastrtps_cpp.hpp>
#include <geometry_msgs/msg/detail/transform_stamped__struct.hpp>
#include <geometry_msgs/msg/detail/vector3__rosidl_typesupport_fastrtps_cpp.hpp>
#include <geometry_msgs/msg/detail/vector3__struct.hpp>
#include <std_msgs/msg/detail/header__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/detail/header__struct.hpp>
#include <std_msgs/msg/detail/string__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/detail/string__struct.hpp>
#include <tf2_msgs/msg/detail/tf_message__rosidl_typesupport_fastrtps_cpp.hpp>
#include <tf2_msgs/msg/detail/tf_message__struct.hpp>
#include <visualization_msgs/msg/detail/marker__rosidl_typesupport_fastrtps_cpp.hpp>
#include <visualization_msgs/msg/detail/marker__struct.hpp>
#include <visualization_msgs/msg/detail/marker_array__rosidl_typesupport_fastrtps_cpp.hpp>
#include <visualization_msgs/msg/detail/marker_array__struct.hpp>

// C type support headers
#include <visualization_msgs/msg/detail/marker_array__rosidl_typesupport_fastrtps_c.h>
#include <visualization_msgs/msg/detail/marker_array__functions.h>
#include <visualization_msgs/msg/detail/marker__functions.h>
#include <visualization_msgs/msg/detail/marker__struct.h>
#include <geometry_msgs/msg/detail/point__functions.h>
#include <rosidl_runtime_c/string_functions.h>

static constexpr size_t kBufferSize = 4 * 1024 * 1024;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
template<typename Msg>
static Msg make_msg();

template<>
std_msgs::msg::String make_msg()
{
  std_msgs::msg::String m;
  m.data = "Hello, ROS 2 serialization benchmark!";
  return m;
}

template<>
std_msgs::msg::Header make_msg()
{
  std_msgs::msg::Header m;
  m.stamp.sec = 123;
  m.stamp.nanosec = 456789;
  m.frame_id = "base_link";
  return m;
}

template<>
geometry_msgs::msg::Vector3 make_msg()
{
  geometry_msgs::msg::Vector3 m;
  m.x = 1.0; m.y = 2.0; m.z = 3.0;
  return m;
}

template<>
geometry_msgs::msg::Pose make_msg()
{
  geometry_msgs::msg::Pose m;
  m.position.x = 1.0; m.position.y = 2.0; m.position.z = 3.0;
  m.orientation.w = 1.0;
  return m;
}

template<>
visualization_msgs::msg::Marker make_msg()
{
  visualization_msgs::msg::Marker m;
  m.header = make_msg<std_msgs::msg::Header>();
  m.ns = "benchmark";
  m.id = 42;
  m.type = visualization_msgs::msg::Marker::SPHERE;
  m.action = visualization_msgs::msg::Marker::ADD;
  m.pose = make_msg<geometry_msgs::msg::Pose>();
  m.scale.x = m.scale.y = m.scale.z = 0.1;
  m.color.r = 1.0f; m.color.g = 0.0f; m.color.b = 0.0f; m.color.a = 1.0f;
  m.lifetime.sec = 0; m.lifetime.nanosec = 0;
  // Add some points to exercise the vector path
  m.points.resize(64);
  for (auto & p : m.points) { p.x = 1.0; p.y = 2.0; p.z = 3.0; }
  return m;
}

template<>
geometry_msgs::msg::TransformStamped make_msg()
{
  geometry_msgs::msg::TransformStamped m;
  m.header = make_msg<std_msgs::msg::Header>();
  m.child_frame_id = "camera_link";
  m.transform.translation.x = 1.0;
  m.transform.translation.y = 0.5;
  m.transform.translation.z = 1.5;
  m.transform.rotation.x = 0.0;
  m.transform.rotation.y = 0.0;
  m.transform.rotation.z = 0.0;
  m.transform.rotation.w = 1.0;
  return m;
}

static tf2_msgs::msg::TFMessage make_tf_message(size_t n)
{
  tf2_msgs::msg::TFMessage m;
  m.transforms.reserve(n);
  for (size_t i = 0; i < n; ++i) {
    auto tf = make_msg<geometry_msgs::msg::TransformStamped>();
    tf.child_frame_id = "link_" + std::to_string(i);
    m.transforms.push_back(std::move(tf));
  }
  return m;
}

template<>
visualization_msgs::msg::MarkerArray make_msg()
{
  visualization_msgs::msg::MarkerArray m;
  m.markers.resize(16, make_msg<visualization_msgs::msg::Marker>());
  for (int i = 0; i < 16; ++i) {
    m.markers[i].id = i;
  }
  return m;
}

// ---------------------------------------------------------------------------
// C-type helpers
// ---------------------------------------------------------------------------
static void fill_c_marker(visualization_msgs__msg__Marker * m, int id)
{
  visualization_msgs__msg__Marker__init(m);
  rosidl_runtime_c__String__assign(&m->ns, "benchmark");
  m->id = id;
  m->type = 2;   // SPHERE
  m->action = 0; // ADD
  m->pose.position.x = 1.0; m->pose.position.y = 2.0; m->pose.position.z = 3.0;
  m->pose.orientation.w = 1.0;
  m->scale.x = m->scale.y = m->scale.z = 0.1;
  m->color.r = 1.0f; m->color.a = 1.0f;
  // 64 points
  geometry_msgs__msg__Point__Sequence__init(&m->points, 64);
  for (size_t i = 0; i < 64; ++i) {
    m->points.data[i].x = 1.0; m->points.data[i].y = 2.0; m->points.data[i].z = 3.0;
  }
}

// ---------------------------------------------------------------------------
// Serialize benchmark
// ---------------------------------------------------------------------------
template<typename Msg, bool (*SerFn)(const Msg &, eprosima::fastcdr::Cdr &)>
static void BM_Serialize(benchmark::State & state)
{
  const Msg msg = make_msg<Msg>();
  std::vector<char> raw(kBufferSize);
  eprosima::fastcdr::FastBuffer buffer(raw.data(), raw.size());

  for (auto _ : state) {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    SerFn(msg, cdr);
    benchmark::DoNotOptimize(raw.data());
    benchmark::ClobberMemory();
  }
}

// ---------------------------------------------------------------------------
// Deserialize benchmark (serialize once into buffer, then repeatedly deser)
// ---------------------------------------------------------------------------
template<typename Msg,
  bool (*SerFn)(const Msg &, eprosima::fastcdr::Cdr &),
  bool (*DeserFn)(eprosima::fastcdr::Cdr &, Msg &)>
static void BM_Deserialize(benchmark::State & state)
{
  const Msg src = make_msg<Msg>();
  std::vector<char> raw(kBufferSize);
  eprosima::fastcdr::FastBuffer buffer(raw.data(), raw.size());

  // Pre-serialize into buffer
  {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    SerFn(src, cdr);
  }

  Msg dst;
  for (auto _ : state) {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    DeserFn(cdr, dst);
    benchmark::DoNotOptimize(dst);
    benchmark::ClobberMemory();
  }
}

// ---------------------------------------------------------------------------
// Registrations
// ---------------------------------------------------------------------------
namespace ser = std_msgs::msg::typesupport_fastrtps_cpp;
namespace gser = geometry_msgs::msg::typesupport_fastrtps_cpp;
namespace tfser = tf2_msgs::msg::typesupport_fastrtps_cpp;
namespace vser = visualization_msgs::msg::typesupport_fastrtps_cpp;

// ---------------------------------------------------------------------------
// TFMessage with N transforms (variable-size, not templatable via make_msg<>)
// ---------------------------------------------------------------------------
static void BM_Serialize_TFMessage(benchmark::State & state)
{
  const tf2_msgs::msg::TFMessage msg = make_tf_message(
    static_cast<size_t>(state.range(0)));
  std::vector<char> raw(kBufferSize);
  eprosima::fastcdr::FastBuffer buffer(raw.data(), raw.size());

  for (auto _ : state) {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    tfser::cdr_serialize(msg, cdr);
    benchmark::DoNotOptimize(raw.data());
    benchmark::ClobberMemory();
  }
  state.SetLabel(std::to_string(state.range(0)) + " transforms");
}

static void BM_Deserialize_TFMessage(benchmark::State & state)
{
  const tf2_msgs::msg::TFMessage src = make_tf_message(
    static_cast<size_t>(state.range(0)));
  std::vector<char> raw(kBufferSize);
  eprosima::fastcdr::FastBuffer buffer(raw.data(), raw.size());
  {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    tfser::cdr_serialize(src, cdr);
  }
  tf2_msgs::msg::TFMessage dst;
  for (auto _ : state) {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    tfser::cdr_deserialize(cdr, dst);
    benchmark::DoNotOptimize(dst);
    benchmark::ClobberMemory();
  }
  state.SetLabel(std::to_string(state.range(0)) + " transforms");
}

static void BM_Serialize_MarkerArray_N(benchmark::State & state)
{
  visualization_msgs::msg::MarkerArray msg;
  const auto n = static_cast<size_t>(state.range(0));
  msg.markers.resize(n, make_msg<visualization_msgs::msg::Marker>());
  for (size_t i = 0; i < n; ++i) { msg.markers[i].id = static_cast<int>(i); }
  std::vector<char> raw(kBufferSize);
  eprosima::fastcdr::FastBuffer buffer(raw.data(), raw.size());

  for (auto _ : state) {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    vser::cdr_serialize(msg, cdr);
    benchmark::DoNotOptimize(raw.data());
    benchmark::ClobberMemory();
  }
  state.SetLabel(std::to_string(n) + " markers");
}

static void BM_Deserialize_MarkerArray_N(benchmark::State & state)
{
  const auto n = static_cast<size_t>(state.range(0));
  visualization_msgs::msg::MarkerArray src;
  src.markers.resize(n, make_msg<visualization_msgs::msg::Marker>());
  for (size_t i = 0; i < n; ++i) { src.markers[i].id = static_cast<int>(i); }
  std::vector<char> raw(kBufferSize);
  eprosima::fastcdr::FastBuffer buffer(raw.data(), raw.size());
  {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    vser::cdr_serialize(src, cdr);
  }
  visualization_msgs::msg::MarkerArray dst;
  for (auto _ : state) {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    vser::cdr_deserialize(cdr, dst);
    benchmark::DoNotOptimize(dst);
    benchmark::ClobberMemory();
  }
  state.SetLabel(std::to_string(n) + " markers");
}

static void BM_Deserialize_MarkerArray_N_Cleanup(benchmark::State & state)
{
  const auto n = static_cast<size_t>(state.range(0));
  visualization_msgs::msg::MarkerArray src;
  src.markers.resize(n, make_msg<visualization_msgs::msg::Marker>());
  for (size_t i = 0; i < n; ++i) { src.markers[i].id = static_cast<int>(i); }
  std::vector<char> raw(kBufferSize);
  eprosima::fastcdr::FastBuffer buffer(raw.data(), raw.size());
  {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    vser::cdr_serialize(src, cdr);
  }
  for (auto _ : state) {
    visualization_msgs::msg::MarkerArray dst;
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    vser::cdr_deserialize(cdr, dst);
    benchmark::DoNotOptimize(dst);
    benchmark::ClobberMemory();
  }
  state.SetLabel(std::to_string(n) + " markers");
}

BENCHMARK((BM_Serialize<std_msgs::msg::String, ser::cdr_serialize>))
->Name("Serialize/String")->MinTime(0.5);
BENCHMARK((BM_Deserialize<std_msgs::msg::String, ser::cdr_serialize, ser::cdr_deserialize>))
->Name("Deserialize/String")->MinTime(0.5);

BENCHMARK((BM_Serialize<std_msgs::msg::Header, ser::cdr_serialize>))
->Name("Serialize/Header")->MinTime(0.5);
BENCHMARK((BM_Deserialize<std_msgs::msg::Header, ser::cdr_serialize, ser::cdr_deserialize>))
->Name("Deserialize/Header")->MinTime(0.5);

BENCHMARK((BM_Serialize<geometry_msgs::msg::Vector3, gser::cdr_serialize>))
->Name("Serialize/Vector3")->MinTime(0.5);
BENCHMARK((BM_Deserialize<geometry_msgs::msg::Vector3, gser::cdr_serialize, gser::cdr_deserialize>))
->Name("Deserialize/Vector3")->MinTime(0.5);

BENCHMARK((BM_Serialize<geometry_msgs::msg::Pose, gser::cdr_serialize>))
->Name("Serialize/Pose")->MinTime(0.5);
BENCHMARK((BM_Deserialize<geometry_msgs::msg::Pose, gser::cdr_serialize, gser::cdr_deserialize>))
->Name("Deserialize/Pose")->MinTime(0.5);

BENCHMARK((BM_Serialize<visualization_msgs::msg::Marker, vser::cdr_serialize>))
->Name("Serialize/Marker")->MinTime(0.5);
BENCHMARK((BM_Deserialize<visualization_msgs::msg::Marker, vser::cdr_serialize, vser::cdr_deserialize>))
->Name("Deserialize/Marker")->MinTime(0.5);

BENCHMARK((BM_Serialize<visualization_msgs::msg::MarkerArray, vser::cdr_serialize>))
->Name("Serialize/MarkerArray/16")->MinTime(0.5);
BENCHMARK((BM_Deserialize<visualization_msgs::msg::MarkerArray, vser::cdr_serialize, vser::cdr_deserialize>))
->Name("Deserialize/MarkerArray/16")->MinTime(0.5);

BENCHMARK((BM_Serialize<geometry_msgs::msg::TransformStamped, gser::cdr_serialize>))
->Name("Serialize/TransformStamped")->MinTime(0.5);
BENCHMARK((BM_Deserialize<geometry_msgs::msg::TransformStamped, gser::cdr_serialize, gser::cdr_deserialize>))
->Name("Deserialize/TransformStamped")->MinTime(0.5);

BENCHMARK(BM_Serialize_TFMessage)->Name("Serialize/TFMessage")
->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->MinTime(0.5);
BENCHMARK(BM_Deserialize_TFMessage)->Name("Deserialize/TFMessage")
->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->MinTime(0.5);

BENCHMARK(BM_Serialize_MarkerArray_N)->Name("Serialize/MarkerArray_N/cpp")
->Arg(16)->Arg(100)->Arg(1000)->MinTime(0.5);
BENCHMARK(BM_Deserialize_MarkerArray_N)->Name("Deserialize/MarkerArray_N/cpp")
->Arg(16)->Arg(100)->Arg(1000)->MinTime(0.5);
BENCHMARK(BM_Deserialize_MarkerArray_N_Cleanup)->Name("Deserialize/MarkerArray_N/cpp_cleanup")
->Arg(16)->Arg(100)->Arg(1000)->MinTime(0.5);

// ---------------------------------------------------------------------------
// C-type MarkerArray benchmarks
// ---------------------------------------------------------------------------
static void BM_Serialize_MarkerArray_N_C(benchmark::State & state)
{
  const auto n = static_cast<size_t>(state.range(0));
  visualization_msgs__msg__MarkerArray msg;
  visualization_msgs__msg__MarkerArray__init(&msg);
  visualization_msgs__msg__Marker__Sequence__init(&msg.markers, n);
  for (size_t i = 0; i < n; ++i) {
    fill_c_marker(&msg.markers.data[i], static_cast<int>(i));
  }

  std::vector<char> raw(kBufferSize);
  eprosima::fastcdr::FastBuffer buffer(raw.data(), raw.size());

  for (auto _ : state) {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    cdr_serialize_visualization_msgs__msg__MarkerArray(&msg, cdr);
    benchmark::DoNotOptimize(raw.data());
    benchmark::ClobberMemory();
  }
  visualization_msgs__msg__MarkerArray__fini(&msg);
  state.SetLabel(std::to_string(n) + " markers");
}

static void BM_Deserialize_MarkerArray_N_C(benchmark::State & state)
{
  const auto n = static_cast<size_t>(state.range(0));
  visualization_msgs__msg__MarkerArray src;
  visualization_msgs__msg__MarkerArray__init(&src);
  visualization_msgs__msg__Marker__Sequence__init(&src.markers, n);
  for (size_t i = 0; i < n; ++i) {
    fill_c_marker(&src.markers.data[i], static_cast<int>(i));
  }

  std::vector<char> raw(kBufferSize);
  eprosima::fastcdr::FastBuffer buffer(raw.data(), raw.size());
  {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    cdr_serialize_visualization_msgs__msg__MarkerArray(&src, cdr);
  }
  visualization_msgs__msg__MarkerArray__fini(&src);

  visualization_msgs__msg__MarkerArray dst;
  visualization_msgs__msg__MarkerArray__init(&dst);
  for (auto _ : state) {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    cdr_deserialize_visualization_msgs__msg__MarkerArray(cdr, &dst);
    benchmark::DoNotOptimize(dst);
    benchmark::ClobberMemory();
  }
  visualization_msgs__msg__MarkerArray__fini(&dst);
  state.SetLabel(std::to_string(n) + " markers");
}

static void BM_Deserialize_MarkerArray_N_C_Cleanup(benchmark::State & state)
{
  const auto n = static_cast<size_t>(state.range(0));
  visualization_msgs__msg__MarkerArray src;
  visualization_msgs__msg__MarkerArray__init(&src);
  visualization_msgs__msg__Marker__Sequence__init(&src.markers, n);
  for (size_t i = 0; i < n; ++i) {
    fill_c_marker(&src.markers.data[i], static_cast<int>(i));
  }

  std::vector<char> raw(kBufferSize);
  eprosima::fastcdr::FastBuffer buffer(raw.data(), raw.size());
  {
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    cdr_serialize_visualization_msgs__msg__MarkerArray(&src, cdr);
  }
  visualization_msgs__msg__MarkerArray__fini(&src);

  for (auto _ : state) {
    visualization_msgs__msg__MarkerArray dst;
    visualization_msgs__msg__MarkerArray__init(&dst);
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
      eprosima::fastcdr::CdrVersion::XCDRv1);
    cdr_deserialize_visualization_msgs__msg__MarkerArray(cdr, &dst);
    benchmark::DoNotOptimize(dst);
    benchmark::ClobberMemory();
    visualization_msgs__msg__MarkerArray__fini(&dst);
  }
  state.SetLabel(std::to_string(n) + " markers");
}

BENCHMARK(BM_Serialize_MarkerArray_N_C)->Name("Serialize/MarkerArray_N/c")
->Arg(16)->Arg(100)->Arg(1000)->MinTime(0.5);
BENCHMARK(BM_Deserialize_MarkerArray_N_C)->Name("Deserialize/MarkerArray_N/c")
->Arg(16)->Arg(100)->Arg(1000)->MinTime(0.5);
BENCHMARK(BM_Deserialize_MarkerArray_N_C_Cleanup)->Name("Deserialize/MarkerArray_N/c_cleanup")
->Arg(16)->Arg(100)->Arg(1000)->MinTime(0.5);

BENCHMARK_MAIN();
