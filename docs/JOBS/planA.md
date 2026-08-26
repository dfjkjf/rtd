---
layout: default
title: 岗位匹配计划
nav_order: 3
---

# 岗位匹配计划

可以。按照你这份 JD、你现在的 DDS/Fast DDS 基础，以及你希望转向**机器人中间件性能优化 / Runtime**的目标，我建议不要把三个月安排成“ROS2 八股背诵”。

你的三个月目标应该是：

> **从“DDS 专家”升级成“能够独立定位 ROS2 机器人系统性能问题的 Middleware / Runtime 工程师”。**

最终你要能面对这样的现场问题：

> “机器人运行 2 小时后 CPU 升高、Camera 延迟 100ms、LiDAR 偶尔丢帧、Planner 卡顿，怎么定位？”

然后你能从：

```text
应用
 ↓
ROS2
 ↓
Executor
 ↓
RMW
 ↓
DDS / Zenoh
 ↓
Thread
 ↓
Linux Scheduler
 ↓
CPU / Memory / Network
```

一路建立假设、设计实验、抓数据、看源码，最后定位根因。

---

# 一、三个月总体路线

我建议按照：

```text
第1个月：ROS2 Runtime + Executor + Linux
                ↓
第2个月：机器人数据链路 + IPC + Tracing
                ↓
第3个月：系统级性能项目 + Zenoh + 面试
```

具体优先级：

| 优先级   | 方向                                    |    时间 |
| ----- | ------------------------------------- | ----: |
| ★★★★★ | ROS2 Executor / Callback / QoS / RMW  |    2周 |
| ★★★★★ | Linux 性能分析                            |    1周 |
| ★★★★★ | 系统级性能定位                               | 贯穿3个月 |
| ★★★★☆ | TF2 / 时间同步 / Sensor Pipeline          |    1周 |
| ★★★★☆ | IPC / SHM / Intra-process / Zero-copy |    4天 |
| ★★★★☆ | ROS2 Tracing / LTTng                  |    4天 |
| ★★★★☆ | DDS 深化与 ROS2 RMW                      |    贯穿 |
| ★★★☆☆ | Zenoh                                 |  3～4天 |
| ★★★☆☆ | Nav2 / SLAM / 机器人系统                   |  3～4天 |
| ★★☆☆☆ | URDF / Gazebo / RViz / MoveIt         |  2～3天 |
| ★★★★★ | 项目整合 + 面试                             |  最后2周 |

---

# 二、先定一个三个月的最终项目

这个非常重要。

不要三个月学完以后发现：

> “我学了很多东西，但是没有项目。”

从第一天开始就建立：

# ROS2 Robot Performance Lab

整体结构：

```text
                        Robot System
                             │
       ┌─────────────────────┼─────────────────────┐
       ↓                     ↓                     ↓
    Camera                 LiDAR                  IMU
       │                     │                     │
       └──────────────┬──────┴──────────────┬──────┘
                      ↓                     ↓
                 Sensor Nodes          TF2
                      │
                      ↓
                 Perception
                      │
                      ↓
                   Fusion
                      │
                      ↓
                   Planner
                      │
                      ↓
                   Control
```

底层：

```text
ROS2
 │
 ├── Executor
 ├── Callback Group
 ├── QoS
 ├── RMW
 │
 └── DDS / Zenoh
        │
        ↓
Linux
 ├── Thread
 ├── Scheduler
 ├── CPU
 ├── Memory
 ├── Network
 └── I/O
```

三个月所有实验都往这个项目里面放。

---

# 三个月的时间假设

按照：

**工作日每天 1.5～2 小时 + 周末每天 3～4 小时**

大约：

> **每周 12～15 小时**

三个月约：

> **150～180 小时**

如果你只能每天 1 小时，我后面也给你一个压缩方法。

---

# 第一个月：ROS2 Runtime + Linux

目标：

> **搞清楚 ROS2 是怎么运行的。**

---

# Week 1：ROS2 基础架构

## Day 1：ROS2 总体架构

学习：

```text
Application
 ↓
rclcpp
 ↓
rcl
 ↓
rmw
 ↓
DDS
```

重点：

* Node
* Topic
* Publisher
* Subscriber
* Message
* RMW

不要花时间做复杂机器人功能。

### 实验

建立：

```text
Publisher
 ↓
/test_topic
 ↓
Subscriber
```

记录：

```text
frequency
message size
latency
CPU
```

---

# Day 2：Topic

重点：

* Publisher
* Subscriber
* Topic
* Message
* ROS graph

命令：

```bash
ros2 node list
ros2 node info
ros2 topic list
ros2 topic info
ros2 topic echo
ros2 topic hz
```

### 你必须回答

> ROS2 Topic 和 DDS Topic 是什么关系？

---

# Day 3：Service / Action

理解：

```text
Topic
Service
Action
```

重点不是 API。

而是：

> 三种通信模型解决什么问题？

---

# Day 4：QoS

重点：

```text
Reliability
Durability
History
Depth
Deadline
Lifespan
Liveliness
```

做：

```text
BEST_EFFORT
vs
RELIABLE
```

以及：

```text
depth = 1
depth = 100
```

观察：

```text
latency
drop
memory
```

---

# Day 5：RMW

深入：

```text
rclcpp
 ↓
rcl
 ↓
rmw
 ↓
rmw_fastrtps_cpp
 ↓
Fast DDS
```

这是你的优势区域。

你要开始把以前的 DDS 知识映射到 ROS2。

---

# Weekend：源码追踪

只追一条：

```text
publisher
 ↓
rclcpp
 ↓
rcl
 ↓
rmw
 ↓
Fast DDS
```

不要读大量源码。

输出：

```text
knowledge/ros2_architecture.md
```

画一张调用链。

---

# Week 2：Executor——第一个核心战场

这是三个月最重要的一周之一。

---

## Day 1

理解：

```text
Executor
Wait Set
Callback
```

重点：

> 为什么 ROS2 需要 Executor？

---

# Day 2

实验：

```text
SingleThreadedExecutor
```

设计：

```text
Callback A = 1ms
Callback B = 10ms
Callback C = 100ms
```

测：

```text
latency
jitter
```

---

# Day 3

换：

```text
MultiThreadedExecutor
```

比较：

```text
CPU
Thread
Latency
P99
```

---

# Day 4

学习：

```text
Callback Group

MutuallyExclusive
Reentrant
```

实验：

```text
Group A
 ├── A
 └── B

Group B
 └── C
```

观察并发行为。

---

# Day 5

源码：

```text
spin()
 ↓
Executor
 ↓
wait_for_work()
 ↓
get_next_executable()
 ↓
execute_any_executable()
```

最终回答：

> 一个 ROS2 callback 从“消息到达”到“真正执行”，中间发生了什么？

---

# Weekend：Executor 性能报告

写：

```text
experiments/executor/README.md
```

必须包括：

```text
实验设计
实验数据
P50
P95
P99
CPU
Thread
结论
```

这是以后简历里的一个项目素材。

---

# Week 3：Linux Performance

这是第二个核心战场。

---

# Day 1：CPU

掌握：

```bash
top
htop
top -H
pidstat
```

回答：

> CPU 高到底是进程高还是线程高？

---

# Day 2：perf

掌握：

```bash
perf top
perf record
perf report
```

做一个 CPU hotspot。

从：

```text
CPU 80%
```

定位到：

```text
Thread
 ↓
Function
```

---

# Day 3：Memory

掌握：

```text
RSS
VSZ
Page Fault
Heap
Stack
```

工具：

```text
pmap
Valgrind
ASan
```

人为制造：

```text
memory leak
```

然后定位。

---

# Day 4：Thread / Scheduler

理解：

```text
Thread
Context Switch
Run Queue
CPU Affinity
Priority
```

工具：

```bash
pidstat
perf
vmstat
```

重点理解：

> 为什么线程多了以后不一定更快？

---

# Day 5：strace / I/O

学习：

```bash
strace
iostat
vmstat
```

制造：

```text
file I/O
blocking syscall
```

观察：

```text
latency
```

---

# Weekend：Linux 故障实验

制造：

```text
CPU high
Memory leak
Thread blocking
I/O blocking
```

每个问题写：

```text
现象
 ↓
指标
 ↓
工具
 ↓
证据
 ↓
根因
```

---

# Week 4：ROS2 + Linux 联合分析

这是第一个月的收尾。

做：

```text
ROS2 Node
 ↓
Callback
 ↓
CPU
 ↓
Thread
```

人为制造：

```text
callback sleep
CPU busy loop
mutex contention
```

然后用：

```text
top
perf
gdb
```

定位。

---

# 第一个月结束，你必须达到：

你可以回答：

> ROS2 callback 为什么会延迟？

> MultiThreadedExecutor 为什么可能更快，也可能更慢？

> QoS 为什么会导致消息行为变化？

> CPU 高怎么定位？

> 一个线程为什么一直占 CPU？

> 一个 callback blocking 会影响谁？

> DDS receive thread 和 ROS2 Executor 是什么关系？

如果这些问题答不上来，**不要进入第二个月。**

---

# 第二个月：Robot Data Pipeline

第二个月开始进入“机器人味道”。

---

# Week 5：TF2

重点：

```text
TF
TF2
Frame
Transform
Buffer
Listener
```

理解：

```text
base_link
odom
map
camera_link
laser
imu_link
```

---

# Day 3～4

理解：

```text
/tf
/tf_static
```

以及：

```text
timestamp
extrapolation
transform lookup
```

---

# Day 5

人为制造：

```text
timestamp offset
TF delay
TF frequency jitter
```

观察：

```text
TF lookup
```

---

# Weekend

建立一个：

```text
Robot TF Tree
```

用 RViz2 看。

重点不是学 RViz，而是理解：

> **TF 在机器人系统里到底解决什么问题。**

---

# Week 6：Sensor Pipeline + 时间同步

这是 JD 的核心要求。

模拟：

```text
Camera
LiDAR
IMU
```

不同：

```text
frequency
timestamp
delay
```

例如：

```text
Camera = 30Hz
LiDAR = 10Hz
IMU = 200Hz
```

---

# Day 3

建立：

```text
Sensor timestamp
 ↓
Driver timestamp
 ↓
ROS timestamp
 ↓
DDS transport
 ↓
Callback timestamp
```

测：

```text
transport delay
callback delay
processing delay
```

---

# Day 4～5

制造：

```text
Camera delay = 50ms
LiDAR delay = 20ms
IMU jitter = ±5ms
```

观察 fusion 数据。

---

# Weekend

完成：

# Sensor Synchronization Lab

输出：

```text
timestamp
latency
frequency
jitter
```

---

# Week 7：IPC / SHM / Zero Copy

重点：

```text
Process A
 ↓
DDS
 ↓
Process B
```

vs

```text
Process A
 ↓
Shared Memory
 ↓
Process B
```

以及：

```text
ROS2 intra-process
DDS SHM
Loaned Message
Zero Copy
```

一定要搞清楚：

> 这些概念不是完全等价的。

---

# Day 1～2

大消息：

```text
1920×1080 image
```

测试：

```text
copy
serialization
deserialization
```

---

# Day 3

比较：

```text
Inter-process
vs
Intra-process
```

测：

```text
CPU
Memory
Latency
```

---

# Day 4～5

研究 DDS SHM / Fast DDS SHM。

因为这是你的强项，所以这一块可以快速深入。

---

# Weekend

写：

> **ROS2 大数据通信性能分析**

这可以成为你面试的一个重点项目。

---

# Week 8：ROS2 Tracing

学习：

```text
ros2_tracing
LTTng
Tracepoint
```

目标：

把：

```text
Publish
 ↓
DDS
 ↓
Executor
 ↓
Callback
```

放到一条时间线上。

---

# Day 3

解决：

> “消息已经到了，但 callback 为什么没执行？”

拆：

```text
Transport latency
Scheduler latency
Executor latency
Callback execution
```

---

# Day 4～5

做 tracing：

```text
T0 publish
T1 receive
T2 executor wake
T3 callback start
T4 callback end
```

计算：

```text
T1-T0
T2-T1
T3-T2
T4-T3
```

---

# Weekend

完成：

# ROS2 End-to-End Latency Analyzer

这是非常值得放到简历上的项目。

---

# 第三个月：系统级能力 + Zenoh + 面试

---

# Week 9：机器人系统整体架构

现在把：

```text
Camera
LiDAR
IMU
TF2
Perception
SLAM
Nav2
Control
```

串起来。

你不需要成为：

> SLAM 工程师

但必须知道：

```text
谁产生数据
谁消费数据
数据频率
消息大小
QoS
时间戳
CPU
```

---

# Nav2

只学：

```text
Localization
 ↓
Planner
 ↓
Controller
 ↓
Cmd_vel
```

理解 Topic / Action / TF 的关系。

---

# Week 10：Zenoh

这时候才学 Zenoh。

不要一开始就：

> Zenoh 好不好？

先建立：

```text
ROS2
 ↓
RMW
 ↓
rmw_fastrtps_cpp
 ↓
DDS

ROS2
 ↓
RMW
 ↓
rmw_zenoh_cpp
 ↓
Zenoh
```

---

# 重点比较

```text
Discovery
Communication model
QoS semantics
Transport
Shared Memory
Large data
WAN
Resource usage
Latency
```

做：

```text
10 nodes
50 nodes
100 nodes
```

测试：

```text
DDS
vs
Zenoh
```

---

# Week 11：综合故障实验

这一周非常重要。

你人为制造 6 个问题：

### Case 1

```text
CPU 90%
```

---

### Case 2

```text
Memory continuously increasing
```

---

### Case 3

```text
Camera latency 100ms
```

---

### Case 4

```text
LiDAR message drop
```

---

### Case 5

```text
Planner occasionally blocks
```

---

### Case 6

```text
TF extrapolation
```

然后每一个问题：

```text
现象
 ↓
假设
 ↓
实验
 ↓
工具
 ↓
数据
 ↓
源码
 ↓
根因
 ↓
修复
 ↓
验证
```

---

# Week 12：面试冲刺

这一周不要再学新东西。

开始准备：

## ① 自我介绍

必须从：

> “我做了 7 年 DDS。”

变成：

> **“我有 7 年 C/C++ 和 Middleware 开发经验，长期负责 DDS/RTPS 协议栈、通信性能和稳定性问题，目前重点转向 ROS2 机器人 Runtime 和系统级性能优化。”**

---

# ② 准备 10 个技术问题

你必须能够深入回答：

### ROS2

1. ROS2 Executor 是什么？
2. Single vs MultiThreadedExecutor？
3. Callback Group？
4. QoS？
5. RMW？
6. Composition？

### DDS

7. Discovery？
8. Reliable vs Best Effort？
9. SHM？
10. Serialization？

---

# ③ 准备 10 个故障题

例如：

> CPU 90%，怎么办？

> 内存不断上涨怎么办？

> Topic 从 30Hz 变成 15Hz 怎么查？

> 消息偶尔丢失怎么办？

> Camera 延迟 100ms 怎么查？

> Callback 阻塞怎么办？

> 节点突然死掉怎么办？

> TF extrapolation 怎么查？

> 多传感器不同步怎么办？

> 100 个节点后 Discovery 变慢怎么办？

---

# 四、三个月必须产出什么？

不要以：

> “我学完了 ROS2。”

作为目标。

你应该最终拥有：

```text
ros2-performance-lab/

├── architecture/
│   └── ros2-runtime.md
│
├── executor/
│   ├── single_vs_multi
│   └── callback_group
│
├── qos/
│
├── linux/
│   ├── cpu
│   ├── memory
│   └── thread
│
├── sensor/
│   ├── camera
│   ├── lidar
│   └── imu
│
├── tf2/
│
├── ipc/
│
├── tracing/
│
├── zenoh/
│
└── benchmark/
```

最后至少形成 **5 个可以面试讲 10～15 分钟的案例**：

```text
① Executor 性能优化

② ROS2 大数据通信优化

③ CPU 高定位

④ 内存增长定位

⑤ Sensor / TF 时间同步问题
```

---

# 五、你的三个月学习时间怎么分配

我建议：

```text
ROS2 Runtime             25%
Linux Performance        20%
实验与Benchmark          20%
Robot Data / TF2         15%
Tracing                  10%
Zenoh                     5%
Nav2/Gazebo/其他          5%
```

不要反过来。

尤其：

> **Gazebo、RViz、URDF 不要吃掉你大量时间。**

这个岗位不是招机器人算法工程师。

---

# 六、每周固定采用这个学习循环

这是整个计划中**最重要的一条**：

```text
周一
理解模型
 ↓
周二
最小代码
 ↓
周三
实验
 ↓
周四
源码
 ↓
周五
性能数据
 ↓
周末
总结 + 面试题
```

例如 Executor：

```text
理解 Executor
 ↓
写 3 callbacks
 ↓
Single vs Multi
 ↓
perf / tracing
 ↓
看 rclcpp Executor 源码
 ↓
解释实验结果
 ↓
面试模拟
```

---

# 七、Agent 在这三个月里怎么参与

你前面问过“本地 Agent 如何帮助学习”，现在可以直接把 Agent 嵌进这个计划。

每周都让它做：

### 周一

> 教练：帮我建立本周知识地图，不要直接给答案。

### 周二

> 实验工程师：帮我设计最小实验。

### 周三

> Coding Agent：根据已经确定的实验设计写代码。

### 周四

> Source Agent：帮我定位 ROS2 源码。

### 周五

> Data Analyst：分析我的实验数据，但不要替我下最终结论。

### 周末

> Interviewer：针对本周内容连续追问我。

这样 Agent 就真正成为：

```text
老师
+
实验助手
+
源码助手
+
数据分析师
+
面试官
```

而不是简单的 ChatGPT。

---

# 八、最终你应该达到什么程度？

三个月结束的时候，如果面试官问：

> **“ROS2 节点 CPU 占用很高，你怎么排查？”**

你应该自然回答：

```text
先确认进程还是线程级 CPU。

top / pidstat
    ↓
定位具体 ROS2 node/thread
    ↓
perf
    ↓
确定 CPU hotspot
    ↓
判断：
Callback？
DDS receive thread？
Executor？
Serialization？
Memory allocation？
    ↓
如果是 Callback：
检查 Executor / Callback Group

如果是 DDS：
检查 serialization / network / discovery

如果是内存：
ASan / Valgrind

如果是调度：
perf / scheduler trace

最后通过 benchmark 验证优化结果。
```

如果面试官继续问：

> “消息已经通过 DDS 收到了，但是 callback 延迟 20ms，怎么办？”

你就继续：

```text
DDS receive
 ↓
RMW
 ↓
wait set
 ↓
Executor
 ↓
scheduler
 ↓
Callback
```

然后利用 tracing 把：

```text
T0
T1
T2
T3
T4
```

拆开。

**这就是这个岗位真正想要的人。**

---

# 九、我给你的最终目标不是“3个月学会这些东西”

而是完成一次能力升级：

```text
现在

DDS / Fast DDS 专家
        │
        ↓
      ROS2
        │
        ↓
   Middleware Engineer
```

三个月后：

```text
              Robot System
                   │
          ┌────────┼────────┐
          ↓        ↓        ↓
        ROS2      DDS      Linux
          │        │        │
       Runtime   Network   Thread
          │        │        │
          └────────┼────────┘
                   ↓
            Performance
                   ↓
          Root Cause Analysis
                   ↓
            System Optimization
```

也就是：

> **从“会实现通信中间件”，升级成“能分析整个机器人软件系统为什么慢、为什么卡、为什么丢、为什么不稳定”。**

这正是你这份 JD 和你现有经历之间最值得补上的那一块。

如果严格按照这个计划执行，**第 1 个月重点打 Executor + Linux，第 2 个月重点打 Sensor/TF2 + IPC + Tracing，第 3 个月把所有东西做成一个完整性能项目并进行面试化训练**。这条路线与你现有 DDS 优势的复用率最高，也最有可能把你从“DDS 专家型候选人”推到“机器人中间件性能优化型候选人”。

