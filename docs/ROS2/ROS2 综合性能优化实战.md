---
layout: default
title: ROS2 综合性能优化实战
nav_order: 11
---

好，进入 **Week 11：ROS2 综合性能优化实战**。

这一周不再新增很多零散知识，而是把前 10 周全部串起来，做一次接近真实工作场景的 **性能问题定位 → 实验设计 → 优化 → 验证 → RCA 报告**。

你的目标岗位如果是“中间件性能优化工程师”，Week 11 开始就是核心训练。

---

# Week 11：完整性能优化项目

## 一、模拟真实机器人数据链

我们构造一个简化机器人：

```text
                     Robot
                       │
       ┌───────────────┼───────────────┐
       │               │               │
    Camera           LiDAR            IMU
    30 Hz             10 Hz           200 Hz
    5 MB              1 MB            1 KB
       │               │               │
       └───────────────┼───────────────┘
                       ↓
                    ROS2 DDS
                       ↓
                   Executor
                       ↓
              ┌────────┴────────┐
              │                 │
          perception        controller
              │                 │
              └────────┬────────┘
                       ↓
                    Robot
```

我们故意让它存在一些问题。

---

# 二、最终模拟的问题

假设产品经理告诉你：

> **机器人运行一段时间后，CPU 很高，IMU 控制偶尔出现明显延迟，Camera 数据也会越来越“旧”。**

监控数据：

```text
CPU Average     72%
CPU Peak        98%
IMU P99         18ms
IMU Max         83ms
Camera latency  120ms
Memory          持续增长
```

控制周期：

```text
5ms
```

也就是：

```text
200Hz
```

要求：

```text
IMU P99 < 5ms
Deadline Miss = 0
Camera P99 < 50ms
CPU < 60%
Memory稳定
```

---

# 三、第一原则：绝对不要马上改代码

这是这周最重要的训练。

拿到问题：

```text
CPU 72%
IMU P99 18ms
```

不要直接：

```text
MultiThreadedExecutor
↓
绑核
↓
换 QoS
↓
Zero-copy
```

因为你还不知道：

> **到底是什么导致了问题。**

正确流程：

```text
                    Problem
                       ↓
                  Baseline
                       ↓
                 Collect Data
                       ↓
                 Form Hypothesis
                       ↓
                  Experiment
                       ↓
                   Evidence
                       ↓
                  Optimization
                       ↓
                 Regression Test
```

---

# 四、Day 1：建立 Baseline

第一天只做一件事情：

> **把系统现在的状态测清楚。**

---

## 1. CPU

先：

```bash
top
```

然后：

```bash
top -H -p <PID>
```

再：

```bash
pidstat -t -p <PID> 1
```

得到：

```text
Process CPU
Thread CPU
```

---

## 2. Memory

```bash
free -h
```

然后：

```bash
pidstat -r -p <PID> 1
```

重点：

```text
RSS
fault
```

---

## 3. ROS2

```bash
ros2 topic hz /imu
ros2 topic hz /camera
ros2 topic hz /lidar
```

再：

```bash
ros2 topic info /imu
```

记录：

```text
Publisher
Subscriber
QoS
```

---

## 4. Latency

不要只看：

```text
topic hz
```

我们自己定义：

```text
T_sensor
T_publish
T_callback
T_control
```

然后：

```text
Sensor → Publish
Publish → Callback
Callback → Control
Sensor → Control
```

分别测。

---

# 五、Day 2：Trace

第二天：

```text
ros2_tracing
+
LTTng
```

重新抓一遍。

最终你希望得到：

```text
Camera

Sensor
 ↓
Publish             5ms
 ↓
DDS                 3ms
 ↓
Callback Ready
 ↓
Executor Wait      80ms  ← ?
 ↓
Callback            20ms
```

IMU：

```text
IMU
 ↓
Publish
 ↓
DDS
 ↓
Executor Wait       15ms ← ?
 ↓
Callback             1ms
```

这时候你会发现：

> Camera 和 IMU 的问题可能完全不是同一个原因。

---

# 六、Day 3：CPU / Scheduler RCA

现在使用：

```bash
perf stat
```

以及：

```bash
perf sched timehist
```

同时：

```bash
pidstat -t 1
```

我们假设发现：

```text
Camera callback
CPU = 40%

Perception thread
CPU = 20%

IMU callback
CPU = 2%
```

然后：

```text
IMU callback
wait time = 12ms
```

这时候一个很强的假设出现了：

> IMU callback 本身并不慢，主要问题是 Executor thread 没有及时获得 CPU。

---

# 七、Day 3 的关键实验

制造 CPU contention：

```text
Camera processing
████████████████
Perception
████████
Logging
████
```

然后：

```text
IMU callback
        ↓
runnable
        ↓
waiting
        ↓
running
```

测：

```text
Scheduler wait
```

如果：

```text
CPU contention ↑
        ↓
scheduler wait ↑
        ↓
IMU P99 ↑
```

你的假设得到验证。

---

# 八、Day 4：Executor 优化

现在才开始改 Executor。

先比较：

```text
SingleThreadedExecutor
```

vs

```text
MultiThreadedExecutor
```

但不要简单比较平均延迟。

记录：

```text
Mean
P95
P99
Max
CPU
```

例如：

| Executor | Mean |  P99 |  Max | CPU |
| -------- | ---: | ---: | ---: | --: |
| Single   |  2ms | 18ms | 83ms | 62% |
| Multi    |  1ms |  7ms | 22ms | 71% |

这时候结论不是：

> MultiThreaded 更好。

而是：

> **MultiThreaded 显著降低 latency，但增加 CPU 消耗。**

这才是性能分析。

---

# 九、Day 4：Callback Group

进一步拆：

```text
IMU
 ↓
Control Callback
```

和：

```text
Camera
 ↓
Perception Callback
```

使用不同 Callback Group。

目标：

```text
IMU Control
     │
     ▼
关键路径

Camera
     │
     ▼
非关键路径
```

最终形成：

```text
                Executor
                   │
          ┌────────┴────────┐
          │                 │
       Critical          Non-critical
          │                 │
        IMU              Camera
        Control          LiDAR
```

这是一个非常典型的机器人实时系统设计思路。

---

# 十、Day 5：CPU Affinity + Priority

现在才进入：

```text
taskset
pthread affinity
SCHED_FIFO
```

目标不是：

> 所有线程都绑核。

而是：

```text
CPU 0-3
│
├── Control
├── IMU
│
CPU 4-7
│
├── Camera
├── Perception
├── Logging
```

如果你的硬件是大小核架构，还要考虑：

```text
CPU capacity
```

你之前已经测过：

```text
cpu0-3 = 422
cpu4-7 = 1024
```

所以这里可以进一步提出：

> **关键控制线程应该放在哪类 CPU 上？**

这就从简单的“绑核”升级到了：

> **CPU capacity-aware scheduling。**

---

# 十一、Day 6：Memory / Copy 优化

现在处理 Camera：

```text
5MB × 30Hz
```

原始数据：

```text
≈150MB/s
```

假设存在：

```text
Sensor buffer
 ↓ copy
ROS message
 ↓ serialization
DDS buffer
 ↓ copy
Receiver
 ↓ deserialize
ROS message
```

那么开始测：

```text
CPU
Memory bandwidth
Latency
```

然后逐步测试：

```text
普通 DDS
        ↓
Intra-process
        ↓
Loaned Message
        ↓
SHM / Zero-Copy
```

注意：

**每次只改变一个变量。**

否则最后你不知道到底是哪项优化有效。

---

# 十二、Day 6 的核心实验

假设：

```text
普通 DDS：

CPU = 72%
Camera P99 = 120ms
```

启用 intra-process：

```text
CPU = 64%
Camera P99 = 80ms
```

进一步：

```text
Loaned / SHM：

CPU = 57%
Camera P99 = 45ms
```

这时候你可以比较：

```text
CPU
72 → 57%

P99
120 → 45ms
```

---

# 十三、Day 7：最终 RCA

现在假设你最终找到：

```text
问题 1
Camera callback CPU 占用高
        ↓
大消息 copy + serialization
        ↓
CPU bandwidth pressure


问题 2
IMU P99 高
        ↓
Executor wait
        ↓
Camera callback 长时间占用 executor


问题 3
偶发 80ms
        ↓
CPU contention
        ↓
scheduler delay


问题 4
Memory 增长
        ↓
频繁 allocation
        ↓
allocator / queue accumulation
```

注意：

**这时候你已经不是在“调 ROS2 参数”。**

你是在做：

> **系统性能 RCA。**

---

# 十四、最后的优化方案

可能最终得到：

```text
                    Optimization
                          │
       ┌──────────────────┼─────────────────┐
       │                  │                 │
    Executor            Memory             CPU
       │                  │                 │
MultiThread          Pre-allocation      Affinity
Callback Group       Reduce Copy         Priority
       │                  │                 │
       └──────────────────┼─────────────────┘
                          │
                          ▼
                       DDS
                          │
                     QoS / SHM
```

---

# 十五、最终必须做 Regression

这是很多工程师容易漏掉的。

优化以后：

```text
CPU ↓
Latency ↓
```

不代表结束。

还必须重新测：

```text
CPU
Memory
Latency
Throughput
Jitter
Packet loss
DDS queue
```

因为可能出现：

```text
CPU ↓
但是：
Memory ↑
```

或者：

```text
Latency ↓
但是：
CPU ↑
```

或者：

```text
Camera ↓
但是：
IMU ↑
```

所以最终必须：

```text
Before
        ↓
Optimization
        ↓
After
        ↓
Regression
```

---

# 十六、最终报告应该长这样

这也是你以后工作中真正可以交付的东西。

## 问题

```text
IMU control loop deadline = 5ms

P99 = 18ms
Max = 83ms
Deadline Miss = 137
```

## 定位

```text
Trace：
DDS transport < 3ms

Executor wait：
最高 15ms

perf sched：
Executor thread 存在明显 scheduler wait

CPU：
Camera / perception 占用大量 CPU
```

## Root Cause

```text
Camera 大消息处理
        ↓
CPU / Memory pressure
        ↓
Executor contention
        ↓
IMU callback scheduling delay
        ↓
Control deadline miss
```

## 优化

```text
1. MultiThreadedExecutor
2. Callback Group 隔离
3. CPU affinity
4. Camera copy 减少
5. Memory pre-allocation
```

## 验证

```text
             Before     After

CPU           72%        55%

IMU P99       18ms       3.2ms

IMU Max       83ms       4.7ms

Deadline      137         0

Camera P99    120ms       42ms

Memory        ↑           stable
```

---

# 十七、Week 11 最重要的能力

这一周你真正要训练的不是：

```text
perf 怎么用
LTTng 怎么用
ROS2 参数怎么改
```

而是下面这条能力：

```text
                   现象
                    │
                    ▼
                建立指标
                    │
                    ▼
                 Trace
                    │
                    ▼
              找异常时间段
                    │
                    ▼
             CPU / Memory / DDS
                    │
                    ▼
              提出单一假设
                    │
                    ▼
                  实验
                    │
              ┌─────┴─────┐
              │           │
            支持          否定
              │           │
              ▼           ▼
            优化        换假设
              │
              ▼
           Regression
              │
              ▼
          RCA + 报告
```

这其实就是你之前一直问的：

> **“系统模型 + 因果关系 + 实验设计 + 最终判断”到底是什么能力？**

**Week 11 就是在实际训练这套能力。**

---

## Week 12

下一周建议进入一个很重要的方向：

# **Week 12：ROS2/DDS 网络性能与故障注入**

重点研究：

```text
Packet Loss
Packet Delay
Packet Reordering
Network Congestion
UDP Buffer
Socket Buffer
DDS Reliability
History
Depth
Deadline
Liveliness
```

然后做真正的故障注入：

```text
Camera
   ↓
DDS
   ↓
网络延迟 + 丢包
   ↓
Subscriber
```

最终回答几个非常接近面试和实际工作的经典问题：

> **为什么 ROS2 消息会丢？**

> **为什么 QoS Reliable 反而可能导致延迟越来越大？**

> **为什么 Best Effort 丢包了，但系统反而更实时？**

> **DDS queue 堆积和 Executor 堵塞怎么区分？**

> **网络正常，但 ROS2 消息为什么还是延迟？**

这一周结束后，你的能力链基本就从 **ROS2 开发**进入到真正的 **ROS2 Middleware Performance Engineering** 了。

