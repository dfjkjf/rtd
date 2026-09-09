---
layout: default
title: ROS2 实时性
nav_order: 10
---

好，进入 **Week 10：ROS2 实时性（Real-Time）**。

这一周非常关键，因为它会把你前面学过的：

```text
CPU
Thread
Scheduler
Executor
DDS
Memory
Trace
```

统一到一个更高层的问题：

> **为什么一个 ROS2 系统“平均很快”，却仍然可能不满足实时要求？**

---

# Week 10 的核心目标

先记住一个非常重要的区别：

```text
低延迟 Low Latency
        ≠
实时 Real-Time
```

例如：

```text
callback latency：

1ms
1ms
1ms
1ms
1ms
1ms
1ms
1ms
100ms   ← 偶尔一次
```

平均值：

```text
≈ 2ms
```

你可能会说：

> 很快。

但如果这是一个：

```text
1kHz 控制回路
```

那么偶发的：

```text
100ms
```

可能已经是严重的实时性问题。

所以 Real-Time 更关注：

> **在规定的时间约束内，任务是否能够稳定完成。**

---

# Week 10 七天安排

| Day   | 主题                        | 核心问题               |
| ----- | ------------------------- | ------------------ |
| Day 1 | Latency vs Jitter         | 什么叫实时              |
| Day 2 | Linux Scheduler           | 普通调度为什么会抖          |
| Day 3 | SCHED_FIFO / Priority     | 实时线程如何调度           |
| Day 4 | CPU Affinity              | 为什么绑核可能有效，也可能更差    |
| Day 5 | Memory / Page Fault       | 为什么 malloc 可能影响实时  |
| Day 6 | Lock / Priority Inversion | 锁为什么会破坏实时性         |
| Day 7 | 综合实验                      | 构建实时 ROS2 callback |

---

# Day 1：Latency、Jitter、Deadline

先建立三个概念。

假设：

```text
IMU callback
目标周期 = 5ms
```

理想：

```text
5ms
5ms
5ms
5ms
5ms
```

这叫：

```text
Period = 5ms
```

---

## 情况 A：平均延迟很低

```text
1ms
1ms
1ms
1ms
1ms
```

很好。

---

## 情况 B：出现抖动

```text
1ms
2ms
1ms
5ms
1ms
10ms
```

这就是：

```text
Jitter
```

即：

> **执行时间/周期围绕目标值的波动。**

---

## 情况 C：Deadline Miss

假设：

```text
deadline = 5ms
```

实际：

```text
1ms
2ms
3ms
8ms ← miss
2ms
```

那么：

```text
Deadline Miss = 1
```

---

# 为什么 P99 比平均值重要？

例如两个系统：

### System A

```text
平均 = 2ms
P99 = 3ms
Max = 5ms
```

### System B

```text
平均 = 1ms
P99 = 4ms
Max = 100ms
```

如果你做控制系统：

```text
System A
```

通常反而更可靠。

因此 Week 10 开始，你以后所有实验都尽量记录：

```text
Mean
P95
P99
Max
Deadline miss
```

而不是只看：

```text
Average
```

---

# Day 1 实验

写一个 C++ 周期任务：

```cpp
while (rclcpp::ok()) {
    auto start = std::chrono::steady_clock::now();

    // 模拟 callback 工作
    do_work();

    auto end = std::chrono::steady_clock::now();

    auto elapsed =
        std::chrono::duration_cast<
            std::chrono::microseconds>(
                end - start);

    // sleep 到下一个周期
}
```

目标：

```text
Period = 5ms
```

运行：

```text
10000 次
```

统计：

```text
min
mean
p95
p99
max
```

然后同时启动：

```bash
stress-ng --cpu 8
```

再跑一次。

你应该开始看到：

```text
Baseline:
P99 = xxx us

CPU stress:
P99 = xxx us
Max = xxx us
```

---

# Day 2：Linux Scheduler

现在研究：

> **为什么线程明明已经 runnable，却没有立即执行？**

线程状态可以简化理解为：

```text
             runnable
                │
                ▼
             scheduler
                │
         ┌──────┴──────┐
         │             │
       running       waiting
         │
         ▼
      callback
```

例如：

```text
Thread A
Runnable
```

但是 CPU 上正在运行：

```text
Thread B
Thread C
Thread D
```

那么 A：

```text
Runnable
   ↓
等待 CPU
   ↓
Running
```

这段时间就是调度延迟的一部分。

---

# 实验：CPU contention

运行你的 ROS2 callback。

同时：

```bash
stress-ng --cpu 8
```

然后：

```bash
top -H
```

观察：

```text
ROS2 executor thread
stress threads
```

再：

```bash
pidstat -t -p <PID> 1
```

关注线程级 CPU。

最后：

```bash
perf sched timehist
```

观察：

```text
wait time
runtime
```

---

# 你之前做过的实验，现在重新做一次

你之前已经看到过：

```text
perf sched timehist
```

中 callback 线程：

```text
wait time
```

出现明显抖动。

现在把它正式定义为：

```text
Scheduler interference
```

然后验证：

```text
CPU contention
       ↓
scheduler wait ↑
       ↓
callback start delay ↑
       ↓
sensor data age ↑
```

这就是 Week 10 的第一条实时性因果链。

---

# Day 3：SCHED_FIFO

普通 Linux 线程通常：

```text
SCHED_OTHER
```

而实时调度常见：

```text
SCHED_FIFO
SCHED_RR
```

先理解 FIFO。

假设：

```text
Priority 80
```

的实时线程正在运行。

普通线程：

```text
Priority normal
```

一般不能简单地“抢走”它的 CPU。

于是：

```text
Realtime Thread
      ↓
SCHED_FIFO
      ↓
更强的调度优先级
```

---

# 做一个简单 C++ 实验

先打印线程调度策略。

使用：

```cpp
sched_getscheduler(0);
```

以及：

```cpp
sched_getparam(
    0,
    &param);
```

查看当前线程。

然后使用：

```cpp
pthread_setschedparam()
```

尝试设置：

```text
SCHED_FIFO
```

注意：

**普通用户通常没有权限直接设置实时优先级。**

可能出现：

```text
Operation not permitted
```

这是正常现象。

---

# 为什么不要一上来就把 ROS2 全部改成 FIFO？

因为：

```text
实时优先级过高
        ↓
长期占用 CPU
        ↓
其他关键线程无法运行
```

甚至可能：

```text
DDS receive thread
       ↓
没有 CPU
```

结果：

```text
Executor虽然很快
DDS反而堵了
```

所以：

> **实时优化是整个系统的调度设计，不是简单给线程加优先级。**

---

# Day 4：CPU Affinity

你之前已经实验过绑核。

这一周重新从实时性的角度理解。

假设：

```text
CPU0
CPU1
CPU2
CPU3
```

把控制 callback：

```text
CPU2
```

把其他工作：

```text
CPU0/1/3
```

理论上：

```text
控制线程
 ↓
CPU2
 ↓
减少竞争
```

可能降低 jitter。

---

# 但为什么绑核可能更差？

这是非常好的面试题。

例如：

```text
Executor → CPU2
```

同时：

```text
DDS thread → CPU2
Camera processing → CPU2
```

结果：

```text
CPU2
████████████████████
```

反而：

```text
scheduler contention ↑
```

所以：

> **CPU affinity 的价值不是“绑核”，而是控制关键线程与其他线程之间的竞争关系。**

---

# Day 4 实验矩阵

测试四种情况：

```text
A. 不绑核
B. ROS2绑核
C. stress和ROS2绑同一个核
D. stress与ROS2分核
```

记录：

| Case | CPU | P99 | Max | Scheduler wait |
| ---- | --: | --: | --: | -------------: |
| A    |     |     |     |                |
| B    |     |     |     |                |
| C    |     |     |     |                |
| D    |     |     |     |                |

这个实验非常值得保留。

以后面试可以直接讲：

> 我通过 CPU affinity 和 perf sched 对比验证了 CPU contention 对 callback tail latency 的影响。

---

# Day 5：Memory Real-Time

这是很多 ROS2 工程师容易忽略的。

假设 callback：

```cpp
void callback(...)
{
    std::vector<uint8_t> buffer;
    buffer.resize(5 * 1024 * 1024);
}
```

看起来只是：

```text
分配 5MB
```

但实时系统真正关心：

```text
malloc
 ↓
allocator
 ↓
page allocation
 ↓
page fault
 ↓
kernel
```

都可能引入不可预测延迟。

---

# Page Fault

程序访问：

```text
Virtual Address
```

如果对应页面还没有准备好：

```text
Page Fault
```

可能触发：

```text
kernel
 ↓
memory management
 ↓
disk / page allocation
```

这对于实时任务非常危险。

---

# 实验

运行：

```bash
/usr/bin/time -v ./your_program
```

观察：

```text
Minor page faults
Major page faults
```

然后：

```bash
perf stat -e page-faults,minor-faults,major-faults ./your_program
```

比较：

```text
首次运行
vs
预热以后
```

---

# mlockall

Linux 实时程序常见：

```cpp
mlockall(
    MCL_CURRENT | MCL_FUTURE);
```

思想：

> 把进程需要的内存锁在 RAM 中，避免后续被换出。

但不要简单理解成：

> `mlockall()` = 实时。

它只是减少一类内存不确定性。

---

# Day 6：Lock + Priority Inversion

这是实时系统非常经典的问题。

假设：

```text
High Priority Thread
        │
        ▼
       Lock
        ▲
        │
Low Priority Thread
```

低优先级线程拿着锁。

高优先级线程：

```text
blocked
```

如果中优先级线程不断运行：

```text
Medium Thread
████████████████
```

那么：

```text
Low Thread
    ↓
拿着锁
    ↓
无法运行
    ↓
High Thread
    ↓
一直等
```

这就是：

> **Priority Inversion**

---

# 实验

三个线程：

```text
High
Medium
Low
```

Low：

```cpp
mutex.lock();

sleep(100ms);

mutex.unlock();
```

High：

```cpp
mutex.lock();
```

Medium：

```text
CPU busy loop
```

观察：

```text
High
 ↓
等待 Low
 ↓
Low 又被 Medium 抢占
 ↓
High 长时间不能执行
```

---

# 然后研究 Priority Inheritance

Linux pthread mutex 可以配置：

```text
PTHREAD_PRIO_INHERIT
```

核心思想：

```text
Low
持有 High 需要的锁
        ↓
临时提升 Low 优先级
        ↓
Low 尽快释放锁
        ↓
High继续
```

这就是实时系统里：

> **为什么锁的实现方式也会影响实时性。**

---

# Day 7：最终综合实验

现在构造一个：

```text
200Hz IMU control loop
```

目标：

```text
Period = 5ms
Deadline = 5ms
```

系统同时运行：

```text
Camera      30Hz
LiDAR       10Hz
IMU        200Hz
Logging
DDS
Executor
```

然后故意加入：

```text
CPU stress
Memory allocation
Blocking callback
Mutex contention
```

---

# 你要建立的最终系统

```text
                         ROS2 System
                              │
          ┌───────────────────┼───────────────────┐
          │                   │                   │
       Camera               LiDAR                IMU
          │                   │                   │
          └───────────────────┼───────────────────┘
                              │
                            DDS
                              │
                          Executor
                              │
                    ┌─────────┴─────────┐
                    │                   │
                 callback            control
                    │                   │
                    │                   ▼
                    │             SCHED_FIFO
                    │                   │
                    │              CPU Affinity
                    │                   │
                    └─────────┬─────────┘
                              │
                          Scheduler
                              │
                    ┌─────────┼─────────┐
                    │         │         │
                   CPU      Memory     Lock
                    │         │         │
                    └─────────┼─────────┘
                              ▼
                         Real-Time
```

---

# 最终实验

给自己定义一个：

```text
5ms deadline
```

运行：

```text
10000 cycles
```

统计：

```text
Mean
P95
P99
Max
Deadline Miss Count
```

然后逐步优化：

### Baseline

```text
SCHED_OTHER
无 affinity
普通 malloc
```

↓

### 优化 1

```text
CPU affinity
```

↓

### 优化 2

```text
SCHED_FIFO
```

↓

### 优化 3

```text
预分配 memory
```

↓

### 优化 4

```text
减少 mutex contention
```

↓

### 优化 5

```text
关闭/隔离非关键任务
```

---

# 最终你应该得到这样的结果

比如：

```text
Baseline

Mean = 0.8ms
P99  = 3.2ms
Max  = 17ms
Miss = 32
```

优化后：

```text
Realtime configuration

Mean = 0.7ms
P99  = 1.1ms
Max  = 2.4ms
Miss = 0
```

真正重要的不是：

```text
0.8ms → 0.7ms
```

而是：

```text
Max
17ms → 2.4ms

Deadline miss
32 → 0
```

这才是实时性优化。

---

# Week 10 最重要的认知

你现在应该把：

```text
实时性
```

理解成：

```text
                    Deadline
                       │
                       ▼
              ┌─────────────────┐
              │   Control Task  │
              └────────┬────────┘
                       │
          ┌────────────┼────────────┐
          │            │            │
       Scheduler     Memory       Lock
          │            │            │
       CPU竞争      Page Fault    Contention
          │            │            │
          └────────────┼────────────┘
                       ▼
                    Jitter
                       │
                       ▼
                Deadline Miss
```

所以实时性不是某一个 API。

它是：

> **Scheduler + CPU + Memory + Lock + Executor + DDS + Application 的系统级行为。**

---

## 到这里，你的前 10 周路线已经形成一条完整主线

```text
Week 1-2
ROS2 / DDS 基础
       ↓
Week 3
Linux CPU / perf
       ↓
Week 4
Thread / Scheduler
       ↓
Week 5
TF2
       ↓
Week 6
Sensor + Timestamp
       ↓
Week 7
多传感器数据链故障定位
       ↓
Week 8
Trace + LTTng
       ↓
Week 9
Memory / Copy / Zero-Copy
       ↓
Week 10
Real-Time
```

**下一阶段 Week 11 就可以开始做真正的“综合性能优化项目”：给你一个故意设计得很差的 ROS2 机器人数据链，要求你在不改变业务逻辑的情况下，把 CPU、P99 latency、memory 和 jitter 降下来。**

这一阶段会非常接近你目标岗位实际会遇到的工作。

