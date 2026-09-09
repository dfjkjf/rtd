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

