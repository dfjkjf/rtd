---
layout: default
title: ROS2 / DDS / Linux 全链路性能分析与 Trace
nav_order: 8
---

好，进入 **Week 8：ROS2 / DDS / Linux 全链路性能分析与 Trace**。

这一周是前面几周的一个重要跃迁：

> **从“知道哪里可能有问题”，进入“用工具证明问题发生在哪里”。**

你前面已经建立了：

```text
Sensor
  ↓
Timestamp
  ↓
TF
  ↓
ROS2
  ↓
DDS
  ↓
Executor
  ↓
Callback
  ↓
CPU / Scheduler
```

Week 8 要解决的是：

> **如何把这条链真正“测出来”。**

---

# Week 8 总目标

最终面对：

```text
Camera 延迟 80ms
```

你不能只看：

```bash
ros2 topic hz
top
```

而是要能够逐层建立证据：

```text
Sensor timestamp
        ↓
ROS2 publish
        ↓
DDS write
        ↓
DDS receive
        ↓
Executor wakeup
        ↓
Callback start
        ↓
Callback end
        ↓
CPU scheduling
```

最终能够回答：

> **80ms 到底花在哪里？**

---

# Week 8 七天安排

| Day   | 主题              | 工具                   | 核心能力               |
| ----- | --------------- | -------------------- | ------------------ |
| Day 1 | ROS2 Trace 基础   | ros2_tracing / LTTng | 看 ROS2 生命周期        |
| Day 2 | Callback 延迟     | Trace                | publish → callback |
| Day 3 | Executor 调度     | Trace + Linux        | ready → execute    |
| Day 4 | CPU / Scheduler | perf / ftrace        | 找调度瓶颈              |
| Day 5 | DDS 层分析         | Fast DDS + Trace     | ROS2 → DDS         |
| Day 6 | 综合 Trace        | 多工具                  | 建立完整时间线            |
| Day 7 | 性能 RCA          | 全部工具                 | 独立定位问题             |

---

# Day 1：ROS2 Trace

先建立一个概念：

```text
ros2 topic hz
```

只能告诉你：

```text
频率是多少
```

而 Trace 能告诉你：

```text
什么时候发生了什么
```

例如：

```text
10.000000 publish
10.001000 DDS write
10.003000 DDS receive
10.004000 callback ready
10.005000 callback start
10.007000 callback end
```

于是：

```text
publish → callback
```

就可以真正测量。

---

# 1. 为什么这一周必须学 Trace？

你之前做 Fast DDS 时，比较容易进入这种思路：

```text
“消息慢”
    ↓
看 Fast DDS
```

但是 ROS2 的实际问题可能发生在：

```text
rclcpp
rcl
rmw
DDS
Executor
Linux scheduler
```

Trace 的价值就是：

> **把这些“猜测”变成时间上的证据。**

---

# 2. 第一实验：最简单的 talker/listener

先准备：

```text
talker
   ↓
/chatter
   ↓
listener
```

不要用 Camera。

因为我们现在不是测试传感器，而是在学习 Trace。

运行：

```bash
ros2 run demo_nodes_cpp talker
```

另一个终端：

```bash
ros2 run demo_nodes_cpp listener
```

然后确认：

```bash
ros2 topic hz /chatter
```

---

# 3. Trace 第一阶段

ROS2 Humble 环境下，可以先检查：

```bash
ros2 trace -h
```

如果系统已经安装 ROS 2 tracing 工具，就直接使用。

如果没有，再安装对应的 tracing 包。

核心目标不是记命令，而是最终得到类似：

```text
publish
   ↓
DDS
   ↓
callback
```

的时间事件。

---

# Day 1 不要急着分析 DDS

第一天只回答：

> **ROS2 中一个消息从 publish 到 callback，大概经历了什么事件？**

建立：

```text
Publisher
   │
   ▼
rclcpp
   │
   ▼
rcl
   │
   ▼
rmw
   │
   ▼
DDS
   │
   ▼
rmw
   │
   ▼
rcl
   │
   ▼
Executor
   │
   ▼
Callback
```

---

# Day 2：测 Callback Latency

今天开始真正做性能实验。

定义：

```text
T_pub
T_cb_start
T_cb_end
```

那么：

```text
Publish → Callback
=
T_cb_start - T_pub
```

Callback execution：

```text
Callback execution
=
T_cb_end - T_cb_start
```

于是：

```text
Total
├── waiting / transport
└── callback processing
```

---

# 实验 1：正常情况

运行：

```text
talker → listener
```

记录：

```text
publish → callback
```

得到：

```text
min
avg
p95
p99
max
```

---

# 实验 2：增加 Callback 时间

在 listener 中：

```cpp
std::this_thread::sleep_for(
    std::chrono::milliseconds(10));
```

然后重新测试。

你应该看到：

```text
callback execution ↑
```

但一个非常重要的问题是：

> **publish → callback 一定也会上升吗？**

答案是：

**不一定。**

这取决于：

```text
Executor
线程
消息频率
队列
并发情况
```

这就是 Trace 的价值。

---

# Day 3：Executor 调度

这是 Week 8 最重要的一天之一。

我们把：

```text
DDS receive
```

和：

```text
callback start
```

之间单独拿出来。

建立：

```text
DDS receives message
        │
        ▼
callback becomes ready
        │
        │  waiting
        ▼
Executor executes
        │
        ▼
callback start
```

定义：

```text
Executor delay
=
Callback Start
-
Callback Ready
```

如果：

```text
Executor delay = 80ms
```

而：

```text
DDS transport = 2ms
Callback execution = 3ms
```

那么你就已经可以非常有底气地说：

> **问题主要不在 DDS，而在消息已经到达之后没有及时获得执行机会。**

---

# 实验：制造 Executor 阻塞

创建两个 callback：

```text
callback_A
callback_B
```

A：

```cpp
std::this_thread::sleep_for(
    std::chrono::milliseconds(100));
```

B：

```text
快速执行
```

使用：

```text
SingleThreadedExecutor
```

观察 B：

```text
ready
   ↓
等待
   ↓
start
```

应该出现明显延迟。

---

# 然后换 MultiThreadedExecutor

再做：

```text
ready
   ↓
thread scheduling
   ↓
callback start
```

比较：

```text
SingleThreaded
vs
MultiThreaded
```

这次不要只测平均值。

重点看：

```text
p95
p99
max
```

因为：

> **实时性问题往往藏在尾延迟，而不是平均值。**

---

# Day 4：Linux Scheduler

现在进入你前面 Week 3 学过的：

```text
CPU
Thread
Scheduler
perf
```

这里开始真正把 ROS2 和 Linux 联系起来。

假设：

```text
callback ready
```

但是：

```text
callback thread
```

没有马上运行。

为什么？

可能是：

```text
CPU busy
   ↓
其他线程运行
   ↓
scheduler没有立即调度callback
```

也可能：

```text
callback thread
   ↓
被抢占
```

---

# 1. perf stat

先从：

```bash
perf stat
```

开始。

观察：

```text
context-switches
cpu-migrations
cycles
instructions
```

尤其：

```text
context-switches
```

---

# 2. perf sched

进一步观察 scheduler：

```bash
perf sched
```

核心不是背命令，而是建立：

```text
ROS2 callback latency
        ↓
Linux thread
        ↓
scheduler
        ↓
context switch
        ↓
CPU
```

---

# 3. 制造 CPU contention

启动一个 CPU 消耗线程。

同时运行：

```text
ROS2 publisher
ROS2 subscriber
```

观察：

```text
CPU ↑
scheduler contention ↑
callback latency ↑
```

然后：

```bash
taskset
```

进行绑核实验。

例如：

```text
ROS2 executor → CPU 2
stress        → CPU 2
```

然后：

```text
ROS2 executor → CPU 2
stress        → CPU 3
```

比较：

```text
callback p99
```

---

# 这个实验非常重要

因为它可以验证：

```text
CPU contention
      ↓
scheduler delay
      ↓
callback delay
      ↓
sensor data age ↑
```

这就是你之前一直在学习的：

> **因果链验证。**

---

# Day 5：进入 Fast DDS

现在终于回到你的优势领域。

建立：

```text
rclcpp
  ↓
rcl
  ↓
rmw_fastrtps_cpp
  ↓
Fast DDS
  ↓
DataWriter
```

Subscriber：

```text
DataReader
  ↓
rmw_fastrtps_cpp
  ↓
rcl
  ↓
rclcpp
  ↓
Executor
```

---

# 重点实验

逐渐增加消息大小：

```text
100 KB
500 KB
1 MB
5 MB
10 MB
```

观察：

```text
Latency
CPU
Memory
Throughput
```

---

# 你需要重点关注

```text
Message size ↑
       ↓
Serialization
       ↓
Fast-CDR
       ↓
DataWriter
       ↓
Transport
       ↓
DataReader
       ↓
Deserialization
```

你的 Fast DDS 背景在这里会非常有优势。

你应该开始问：

> **到底是 serialization 慢，还是 transport 慢？**

而不是：

> DDS 慢不慢？

---

# Day 6：完整 Trace

今天把所有东西拼起来。

最终希望看到：

```text
10:00:00.000
Sensor timestamp
       │
10:00:00.005
publish
       │
10:00:00.006
DDS write
       │
10:00:00.008
DDS receive
       │
10:00:00.009
callback ready
       │
10:00:00.050
callback start
       │
10:00:00.055
callback end
```

那么：

```text
Sensor → Publish
= 5ms

DDS
= 2ms

Executor wait
= 41ms

Callback
= 5ms
```

总数据 age：

```text
55ms
```

于是问题一目了然：

```text
███████████████████████████████████████
Sensor       5ms
DDS          2ms
Executor    41ms   ← 最大瓶颈
Callback     5ms
```

这就是 Trace 最核心的价值。

---

# Day 7：最终 RCA

最后一天做一个完整事故。

人为制造：

```text
Camera 30Hz
LiDAR 10Hz
IMU 200Hz
```

然后：

```text
Camera callback = 30ms
LiDAR callback = 50ms
IMU callback = 1ms
```

使用：

```text
SingleThreadedExecutor
```

观察：

```text
IMU p99
Camera p99
LiDAR p99
```

然后逐步改：

```text
SingleThreaded
      ↓
MultiThreaded
      ↓
CallbackGroup
      ↓
CPU affinity
      ↓
QoS
```

每改变一个变量，只做一次实验。

---

# 最终形成性能实验矩阵

| 条件           | Executor | CPU | Camera P99 | IMU P99 |
| ------------ | -------- | --: | ---------: | ------: |
| Baseline     | Single   |     |            |         |
| Blocking     | Single   |     |            |         |
| MultiThread  | Multi    |     |            |         |
| CPU stress   | Multi    |     |            |         |
| CPU affinity | Multi    |     |            |         |
| QoS change   | Multi    |     |            |         |

然后回答：

> **哪一个改动真正解决了问题？**

不是：

> 改完以后感觉好多了。

而是：

```text
Baseline:
IMU P99 = 42ms

CPU affinity:
IMU P99 = 5ms

CPU increase:
+3%
```

于是才能得出：

> **CPU contention / scheduler interference 是主要原因，CPU affinity 显著降低了尾延迟。**

---

# Week 8 最终要掌握的工具链

最终形成：

```text
                ROS2
                 │
       ┌─────────┴─────────┐
       │                   │
   ros2 topic          ros2 node
       │
       ▼
   ros2_tracing
       │
       ▼
      LTTng
       │
       ├──────────────┐
       │              │
       ▼              ▼
     perf           ftrace
       │              │
       └──────┬───────┘
              ▼
          Linux Kernel
```

而 DDS：

```text
ROS2
 ↓
rmw_fastrtps
 ↓
Fast DDS
 ↓
RTPS
 ↓
UDP / SHM
```

成为另一条可以深入的证据链。

---

# Week 8 最重要的能力升级

你到这里以后，排查问题的方式应该发生变化。

以前：

```text
Camera 延迟
   ↓
“可能 DDS 有问题”
```

现在：

```text
Camera 延迟 80ms
        ↓
先测量
        ↓
Sensor → Publish      5ms
Publish → DDS Receive 3ms
DDS → Callback Ready  2ms
Ready → Callback      65ms ← 异常
Callback processing   5ms
        ↓
进入 Executor / Scheduler
        ↓
perf / trace
        ↓
发现 CPU contention
        ↓
验证 CPU affinity
        ↓
P99 从 80ms → 12ms
        ↓
结论
```

这就是你要培养的**性能工程师能力**。

---

## 下一阶段

完成 Week 8 后，建议不要马上继续堆 ROS2 知识。

**Week 9 应该进入 Memory + Zero-Copy + Loaned Message + Allocator。**

因为你已经把：

```text
CPU
Thread
Scheduler
DDS
Executor
Sensor
Timestamp
TF
Trace
```

串起来了。

下一块最大的性能问题就是：

```text
Camera / PointCloud2
        ↓
大消息
        ↓
Serialization
        ↓
Memory allocation
        ↓
Copy
        ↓
DDS
        ↓
Copy
        ↓
Deserialization
```

也就是：

> **为什么一个 5 MB 的 Camera/PointCloud 消息，明明只有几十 Hz，却可能把 CPU 和内存带宽吃掉？**

这会直接进入你熟悉的 **Fast-CDR / Fast DDS / ROS2 loaned message / zero-copy** 领域。

---

可以。我们先不讲概念，直接做一个**最小可运行实验**：

> 用 `ros2_tracing + LTTng` 抓一个 ROS2 `talker → listener`，然后从 Trace 里回答：**消息什么时候发布、什么时候被执行、callback 花了多久。**

你是 C++ 背景，所以整个实验都用 C++。

---

# 1. 先理解 Trace 到底在干什么

普通运行：

```text
talker
   │
   │ publish()
   ▼
DDS
   │
   ▼
listener
   │
   ▼
callback()
```

我们肉眼看不到中间发生了什么。

Trace 相当于在代码关键位置插入：

```text
10:00:00.001  publish
10:00:00.002  DDS write
10:00:00.003  callback ready
10:00:00.004  callback start
10:00:00.005  callback end
```

然后 LTTng 负责：

```text
程序
 ↓
tracepoint
 ↓
LTTng
 ↓
trace buffer
 ↓
CTF trace
 ↓
babeltrace / tracetools分析
```

所以：

> **ros2_tracing 决定“记录哪些 ROS2 事件”，LTTng 负责“把这些事件高效记录下来”。**

---

# 2. 先确认环境

你的环境是 Ubuntu 22.04 + ROS2 Humble，所以先检查：

```bash
lsb_release -a
```

以及：

```bash
printenv ROS_DISTRO
```

应该类似：

```text
Ubuntu 22.04
humble
```

然后：

```bash
which lttng
```

如果没有：

```text
lttng: command not found
```

说明还没安装。

---

# 3. 安装 LTTng

先：

```bash
sudo apt update
```

然后：

```bash
sudo apt install \
    lttng-tools \
    lttng-modules-dkms \
    liblttng-ust-dev \
    babeltrace
```

确认：

```bash
lttng --version
```

例如：

```text
lttng-tools 2.x
```

---

# 4. 安装 ROS2 tracing

Humble 通常可以直接：

```bash
sudo apt install \
    ros-humble-ros2trace \
    ros-humble-tracetools
```

然后：

```bash
source /opt/ros/humble/setup.bash
```

检查：

```bash
ros2 trace -h
```

如果能看到帮助信息，就说明 ROS2 tracing 工具已经进入环境。

---

# 5. 第一个实验：talker → listener

先开终端 A：

```bash
source /opt/ros/humble/setup.bash

ros2 run demo_nodes_cpp talker
```

终端 B：

```bash
source /opt/ros/humble/setup.bash

ros2 run demo_nodes_cpp listener
```

然后确认：

```bash
ros2 topic hz /chatter
```

大概：

```text
average rate: 10
```

现在我们知道系统正常。

---

# 6. 现在开始 Trace

**不要先运行 talker。**

先开第三个终端：

```bash
source /opt/ros/humble/setup.bash
```

创建 trace session。

最简单的方式：

```bash
ros2 trace
```

如果你的 Humble 环境要求指定 session，可以先：

```bash
ros2 trace -h
```

你会看到相关参数。

不同 Humble 安装版本的 `ros2 trace` CLI 参数可能略有差异，所以这里不要死记命令；**以你机器上的 `ros2 trace -h` 为准**。

---

# 7. 一个更底层的 LTTng 实验

我建议你**一定做一次**。

因为你要做的是中间件性能优化，而不是只会：

```bash
ros2 trace
```

我们直接使用 LTTng。

先：

```bash
lttng create ros2_demo
```

成功后类似：

```text
Session ros2_demo created.
Traces will be written in ...
```

---

# 8. 查看 tracepoint

LTTng 可以查看当前有哪些用户态 tracepoint：

```bash
lttng list --userspace
```

你应该能够看到：

```text
tracetools
```

相关事件。

ROS2 tracing 的核心就在这里。

例如可能看到：

```text
ros2:rclcpp_publish
ros2:rclcpp_subscription_init
ros2:rclcpp_callback_register
...
```

具体事件名称以你的 Humble 安装版本为准。

---

# 9. 选择 ROS2 tracepoint

可以先让 LTTng 抓所有用户态事件：

```bash
lttng enable-event --userspace 'ros2:*'
```

然后：

```bash
lttng start
```

现在：

```text
Trace 正在运行
```

---

# 10. 运行你的 ROS2 程序

这时候再启动：

```bash
ros2 run demo_nodes_cpp talker
```

另一个终端：

```bash
ros2 run demo_nodes_cpp listener
```

让它运行：

```text
10～20 秒
```

然后停止：

```bash
Ctrl+C
```

---

# 11. 停止 Trace

回到 LTTng 终端：

```bash
lttng stop
```

然后：

```bash
lttng destroy
```

注意：

```bash
lttng destroy
```

**不是删除 trace 数据。**

它是结束 session。

你的 trace 数据已经写到了之前创建的 trace directory。

可以：

```bash
ls
```

或者：

```bash
find . -maxdepth 3 -type f
```

看看生成的文件。

---

# 12. 用 Babeltrace 看 Trace

这一步非常重要。

LTTng 保存的不是：

```text
txt
csv
```

而是：

```text
CTF
(Common Trace Format)
```

用：

```bash
babeltrace <trace目录>
```

例如：

```bash
babeltrace ~/lttng-traces/ros2_demo*
```

具体目录以 `lttng create` 的输出为准。

你会看到大量类似：

```text
ros2:rclcpp_publish
ros2:rclcpp_callback_register
...
```

---

# 13. 现在开始“读 Trace”

假设你看到：

```text
10:15:01.123456
ros2:rclcpp_publish

10:15:01.223789
ros2:rclcpp_publish
```

说明 publisher 大约：

```text
100.333 ms
```

发布一次。

这和：

```bash
ros2 topic hz /chatter
```

得到的：

```text
10 Hz
```

是一致的。

---

# 14. 但是 Trace 真正厉害的地方在这里

假设：

```text
T1 = publish
T2 = callback_start
T3 = callback_end
```

那么：

```text
T2 - T1
```

就是你关心的：

```text
Publish → Callback
```

而：

```text
T3 - T2
```

是：

```text
Callback execution
```

例如：

```text
publish
10:15:01.123456

callback_start
10:15:01.125200

callback_end
10:15:01.126100
```

那么：

```text
Publish → Callback
= 1.744 ms

Callback execution
= 0.900 ms
```

于是：

```text
总延迟
├── Publish → Callback = 1.744ms
└── Callback           = 0.900ms
```

---

# 15. 但这里有一个关键陷阱

**不要看到 `rclcpp_publish` 就认为这等于 DDS DataWriter::write()。**

这是非常重要的。

：

```text
rclcpp_publish
```

发生在 ROS2 上层。

后面还可能经过：

```text
rclcpp
 ↓
rcl
 ↓
rmw
 ↓
Fast DDS
 ↓
DataWriter
 ↓
RTPS
 ↓
transport
```

所以：

```text
rclcpp_publish
```

和：

```cpp
DataWriter::write()
```

不是同一个时间点。

这正好与你之前研究的：

> `talker → listener` 源码链

连接起来。

---

# 16. 第二个实验：故意制造 100ms 延迟

现在开始真正有价值的实验。

修改 listener：

```cpp
void topic_callback(
    const std_msgs::msg::String::SharedPtr msg)
{
    std::this_thread::sleep_for(
        std::chrono::milliseconds(100));

    RCLCPP_INFO(
        this->get_logger(),
        "I heard: '%s'",
        msg->data.c_str());
}
```

重新编译。

然后：

```text
talker
 ↓
listener
```

重新 Trace。

---

# 17. 你应该观察什么？

你会发现：

```text
callback execution
```

明显增加。

例如：

```text
callback_start
10:20:00.000

callback_end
10:20:00.100
```

因此：

```text
callback execution ≈ 100ms
```

---

# 18. 然后把 publisher 提高到 30Hz

原来：

```text
10Hz
```

现在：

```text
30Hz
```

而 callback：

```text
100ms
```

意味着：

```text
Producer = 30 msg/s
Consumer ≈ 10 msg/s
```

于是：

```text
Queue
 ↓
逐渐积压
```

然后 Trace 中你应该观察：

```text
callback
callback
callback
callback
...
```

之间的时间关系。

这就把 Week 7 的：

```text
Producer > Consumer
        ↓
Queue accumulation
        ↓
Latency ↑
```

用 Trace 真正观察出来。

---

# 19. 再做一个实验：Executor 阻塞

这个实验比上面的更重要。

建立两个 subscription：

```text
/topic_a
/topic_b
```

A：

```cpp
void callback_a(...)
{
    std::this_thread::sleep_for(
        std::chrono::milliseconds(100));
}
```

B：

```cpp
void callback_b(...)
{
    RCLCPP_INFO(...);
}
```

使用：

```text
SingleThreadedExecutor
```

---

# 20. Trace 应该呈现什么？

理论上：

```text
callback_A_start
       │
       │ 100ms
       │
callback_A_end
       │
       ▼
callback_B_start
```

如果 B 的消息在 A 执行期间已经 ready：

```text
B ready
   │
   │ 100ms waiting
   ▼
B start
```

这时候你真正测出来：

```text
Executor wait ≈ 100ms
```

---

# 21. 这就是 Trace 和普通 ros2 topic 工具的区别

`ros2 topic hz`：

```text
/chatter = 10Hz
```

你只知道：

> 频率正常。

Trace：

```text
publish
   ↓ 2ms
ready
   ↓ 98ms
callback_start
   ↓ 100ms
callback_end
```

你知道：

> **问题发生在 callback 执行等待阶段。**

这已经是性能工程的思维。

---

# 22. LTTng 在这里到底干什么？

你可以把三者理解成：

```text
ROS2 tracing
    │
    │ 定义/提供 ROS2 tracepoints
    ▼
LTTng
    │
    │ 高效采集事件
    ▼
CTF trace
    │
    ▼
Babeltrace
    │
    ▼
人分析
```

所以不要把：

```text
Trace
LTTng
ros2_tracing
```

当成三个互相竞争的工具。

它们是一个链条。

---

# 23. 再往下一层：Linux Scheduler

这就是 Week 8 后半段真正要做的。

现在你已经可以看到：

```text
callback_start
```

但你还不知道：

> **为什么 callback 这么晚才运行？**

于是继续往 Linux kernel：

```text
ROS2 Trace
    ↓
LTTng
    ↓
Linux scheduler
    ↓
perf / ftrace
```

例如：

```text
callback ready
       │
       │ 50ms
       ▼
callback start
```

然后用 scheduler trace 发现：

```text
Executor thread
      ↓
Runnable
      ↓
没有获得 CPU
      ↓
其他线程运行
      ↓
50ms
      ↓
Executor thread running
```

那么最终结论就从：

> “ROS2 callback 延迟”

变成：

> **Executor 所在线程处于 runnable 状态，但由于 CPU contention 没有及时获得执行机会。**

这个结论的含金量就完全不一样了。

---

# 24. 你现在先做这一个实验

不要一次做太复杂。

你的第一轮实验只做：

```text
talker → listener
```

然后：

### Step 1

```bash
ros2 run demo_nodes_cpp talker
```

### Step 2

```bash
ros2 run demo_nodes_cpp listener
```

### Step 3

```bash
lttng create ros2_demo
```

### Step 4

```bash
lttng enable-event --userspace 'ros2:*'
```

### Step 5

```bash
lttng start
```

### Step 6

让 talker/listener 跑 10 秒。

### Step 7

```bash
lttng stop
lttng destroy
```

### Step 8

找到 trace directory：

```bash
find ~/ -type d -name 'ros2_demo*' 2>/dev/null
```

### Step 9

用：

```bash
babeltrace <trace目录>
```

查看。

---

## 你第一轮不要急着分析所有事件

只要先找到这几个东西：

```text
rclcpp_publish
```

以及 callback 相关事件。

**把你的 `ros2 trace -h` 输出和 `babeltrace` 输出中涉及 `rclcpp` / `callback` 的几行贴给我。**

我下一步可以直接带你**逐行读 Trace**，把它还原成：

```text
publish
   ↓
rclcpp
   ↓
rmw
   ↓
Fast DDS
   ↓
receive
   ↓
Executor
   ↓
callback
```

然后我们再做第二个实验：**故意让 callback 阻塞 100ms，用 Trace 证明 Executor 为什么会产生延迟。**

