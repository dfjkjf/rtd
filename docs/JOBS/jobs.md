---
layout: default
title: 岗位1
nav_order: 1
---

我们正在招聘 1 名中间件性能优化工程师，负责机器人中间件、通信链路和系统运行性能优化。你将围绕机器人在多传感器、多算法模块、多进程协同场景下的通信延迟、资源占⽤、数据同步、系统卡顿、消息丢失、节点异常等问题，提升机器⼈整机软件系统的实时性、稳定性和可维护性。

### ⼯作职责
1. 负责机器人中间件性能优化，包括 ROS/ROS2 通信、DDS、Topic、Service、Action、参数、Launch、生命周期管理等模块。
2. 排查和优化机器⼈系统中的通信异常、消息延迟、丢包、频率抖动、节点阻塞、数据堆积等问题。
3. 优化多传感器数据传输链路，包括相机、深度相机、激光雷达、IMU、机械臂、底盘等数据同步和时间戳一致性。
4. 分析并优化 CPU、内存、网络 I/O、磁盘 I/O、线程调度、进程资源占用等系统性能瓶颈。
5. 负责机器人系统卡顿、响应延迟、异常重启、内存泄露、死锁、线程阻塞等问题定位和修复。
6. 参与机器人边缘计算平台适配和性能优化，支持算法模块在资源受限平台上的稳定运行。
7. 建设系统性能监控、日志分析、链路追踪和自动化诊断工具，提升问题定位效率。
8. 与感知、SLAM、导航、机械臂、控制、测试和硬件团队协作，推动系统性能问题闭环。

### 任职要求
1. 本科及以上学历，计算机、软件工程、自动化、电子信息、机器人、嵌入式等相关专业优先。
2. 熟练掌握 C/C++，熟悉 Linux 系统开发和性能调优。
3. 熟悉 ROS/ROS2，有机器人项目开发经验，理解 Topic、Service、Action、TF、参数、Launch、Lifecycle 等机制。
4. 熟悉 DDS 通信机制，了解 QoS、消息队列、序列化/反序列化、共享内存通信、进程间通信等相关技术。
5. 理解多线程、进程通信、网络通信、内存管理、任务调度、锁机制等系统基础。
6. 熟悉常见性能分析和调试工具，如 top、htop、perf、gdb、valgrind、ASan、strace、tcpdump、dmesg 等。
7. 有系统级问题定位能力，能排查 CPU 高、内存泄露、通信延迟、数据不同步、系统卡顿等复杂问题。
8. 具备良好的工程习惯，重视日志、监控、异常处理、接口设计和问题闭环。

----

这个岗位其实**非常贴合你的现有背景**。如果按照你之前告诉我的经历来判断，你不是“从零准备”，而是已经覆盖了这个岗位大约 **60%～70% 的核心能力**。

但有一个明显的问题：

> **你的 DDS / Middleware 深度已经超过岗位要求，但 ROS2 应用层、Linux 系统性能分析、机器人数据链路和“完整系统问题闭环”还需要补齐。**

所以我不会建议你重新学一遍 ROS2，而是建议你做一次**针对 JD 的能力补全**。

---

# 一、先把这个岗位拆成 5 个能力域

这个岗位实际上不是在招：

> “ROS2 开发工程师”

而是在招：

> **机器人系统性能 / Middleware Engineer**

我把 JD 拆成：

```text
                    岗位核心
                       │
        ┌──────────────┼──────────────┐
        ↓              ↓              ↓
   ROS2 Middleware   Linux Runtime   Robot Data
        │              │              │
   DDS / RMW        CPU / Memory    TF / Sensor
   QoS              Thread          Timestamp
   Executor         Network         Sync
   IPC              I/O             Pipeline
        │              │              │
        └──────────────┼──────────────┘
                       ↓
                 System Debugging
                       │
              perf / gdb / ASan
              strace / tcpdump
                       │
                       ↓
                Performance Design
```

你的情况大概是：

| 能力                   |   目前判断 |      优先级 |
| -------------------- | -----: | -------: |
| DDS                  |  ★★★★★ |       保持 |
| Fast DDS             |  ★★★★★ |       保持 |
| ROS2 Middleware      |   ★★★★ |       提升 |
| ROS2 应用机制            | ★★～★★★ |  **重点补** |
| C/C++                |   ★★★★ |       保持 |
| Linux                |    ★★★ |  **重点补** |
| 多线程/锁                |   ★★★★ |       深化 |
| CPU/Memory profiling | ★★～★★★ |  **重点补** |
| 网络分析                 |    ★★★ |       提升 |
| TF / 时间同步            |     ★★ |  **重点补** |
| 多传感器链路               |     ★★ |  **重点补** |
| Nav2/SLAM/机械臂        |      ★ |     了解即可 |
| 监控/Tracing           |     ★★ |  **重点补** |
| 系统级问题闭环              |    ★★★ | **核心提升** |

---

# 二、最重要的变化：你要从“Middleware 专家”变成“系统性能专家”

这个 JD 有一句话非常关键：

> **“系统卡顿、响应延迟、异常重启、内存泄露、死锁、线程阻塞等问题定位和修复。”**

这已经超出了 DDS。

比如：

> “机器人突然卡顿。”

你未来面试时应该能够自然地展开：

```text
机器人卡顿
   │
   ├── CPU saturation？
   │
   ├── Memory pressure？
   │
   ├── Page fault？
   │
   ├── IO blocking？
   │
   ├── Thread starvation？
   │
   ├── Mutex contention？
   │
   ├── DDS blocking？
   │
   ├── Executor scheduling？
   │
   ├── Callback blocking？
   │
   ├── Message backlog？
   │
   └── Sensor timestamp？
```

然后：

```text
top
 ↓
perf
 ↓
pidstat
 ↓
vmstat
 ↓
iostat
 ↓
strace
 ↓
gdb
 ↓
tcpdump
 ↓
ROS2 tracing
```

最终把问题定位到：

> **“某个 callback 阻塞 + executor 线程不足 + DDS receive queue 堆积，导致下游 planner 延迟。”**

这种能力才是这个岗位真正要的。

---

# 三、第一优先级：Linux 性能分析

这是我认为你现在最需要补的一块。

JD 直接点名：

```text
top
htop
perf
gdb
valgrind
ASan
strace
tcpdump
dmesg
```

不要把它理解成：

> “我要把这些命令背下来。”

你应该建立：

```text
现象
 ↓
指标
 ↓
工具
 ↓
证据
 ↓
假设
 ↓
实验
 ↓
根因
```

例如：

### CPU 高

```text
top
 ↓
哪个进程？
 ↓
哪个线程？
 ↓
top -H
 ↓
perf top
 ↓
perf record
 ↓
perf report
```

最终回答：

> CPU 到底花在哪里。

---

### 内存持续增长

```text
top
 ↓
pmap
 ↓
valgrind
 ↓
ASan
 ↓
heap profiling
```

最终回答：

> 是 leak、fragmentation、cache 还是消息堆积。

---

### 系统卡顿

```text
top
vmstat
iostat
pidstat
perf
strace
```

最终回答：

> CPU？IO？锁？调度？系统调用？

---

# 四、第二优先级：ROS2 要补，但不要平均用力

你之前问的：

> URDF、TF2、RViz2、Nav2、MoveIt、Gazebo 怎么学？

现在这个 JD 给了答案。

### 必须掌握

```text
Node
Topic
Service
Action
QoS
TF2
Parameter
Launch
Lifecycle
Executor
Callback Group
Composition
RMW
```

其中我给你排序：

```text
Executor       ★★★★★
QoS             ★★★★★
Topic           ★★★★★
TF2             ★★★★
Lifecycle       ★★★★
Composition     ★★★★
Action          ★★★
Service         ★★★
Launch          ★★★
Parameter       ★★★
```

### 了解即可

```text
Nav2
SLAM
MoveIt
Gazebo
RViz2
URDF
```

因为这个岗位需要你：

> **和这些团队合作。**

而不是让你成为 Nav2 算法专家。

---

# 五、第三优先级：TF2 + 时间同步

这个 JD 有一句话值得你特别重视：

> **“多传感器数据同步和时间戳一致性。”**

这是你目前知识体系里比较明显的缺口。

你至少要理解：

```text
Camera
   │ timestamp
   ↓
Depth Camera
   │ timestamp
   ↓
LiDAR
   │ timestamp
   ↓
IMU
   │ timestamp
   ↓
TF2
   │
   ↓
Fusion
```

为什么：

```text
Camera timestamp = 10.000s
LiDAR timestamp  = 10.050s
IMU timestamp    = 10.002s
```

会造成问题？

以及：

> 时间戳到底来自哪里？

```text
Sensor HW clock
       ↓
Driver
       ↓
ROS2 message
       ↓
DDS
       ↓
Application
```

这里非常容易出现：

* 时钟不同步
* timestamp 延迟
* queue delay
* transport delay
* callback delay
* TF extrapolation
* sensor fusion 错位

你以后面试这个岗位，**如果能把这套链路讲清楚，会非常加分。**

---

# 六、第四优先级：IPC + Shared Memory

JD 明确提到：

> **共享内存通信、进程间通信**

这个你要重点补。

特别是：

```text
Camera
 ↓
Image 1920×1080
 ↓
多个算法节点
```

如果每次：

```text
copy
 ↓
serialize
 ↓
DDS
 ↓
deserialize
 ↓
copy
```

性能会非常差。

你需要理解：

```text
Process A
    │
    │ shared memory
    ↓
Process B
```

和：

```text
Process A
    │
    │ DDS
    ↓
Process B
```

的区别。

进一步理解：

```text
ROS2 Intra-process
       ↓
Shared Memory
       ↓
Zero Copy
       ↓
Loaned Message
```

这是非常适合你的方向。

---

# 七、第五优先级：把“性能”变成可量化的东西

你现在已经习惯讨论：

> latency / jitter / CPU / memory

下一步需要做到：

> **建立性能指标体系。**

例如通信：

```text
Message latency
P50
P95
P99
P999
Max
```

系统：

```text
CPU utilization
Context switches
Run queue
Memory RSS
Page faults
IO wait
Network throughput
Packet loss
```

ROS2：

```text
Publish timestamp
 ↓
DDS receive timestamp
 ↓
Executor wakeup
 ↓
Callback start
 ↓
Callback end
```

这样你就能画出：

```text
T0 = Publisher
T1 = DDS received
T2 = Executor wakeup
T3 = Callback start
T4 = Callback end
```

然后：

```text
Communication latency = T1 - T0

Scheduling latency = T3 - T1

Callback execution = T4 - T3
```

**这就是系统性能工程师的思维。**

---

# 八、第六优先级：Tracing

这个岗位写：

> **“建设系统性能监控、日志分析、链路追踪和自动化诊断工具。”**

这里我建议你补：

### ROS 2 tracing

重点了解：

```text
ros2_tracing
LTTng
Tracepoints
```

以及：

```text
Publisher
 ↓
RMW
 ↓
DDS
 ↓
Executor
 ↓
Callback
```

的时间线。

这比单纯：

```bash
ros2 topic hz
```

高级很多。

你甚至可以做：

```text
Topic:
/camera/image

Timeline:

Publish
│
├── DDS receive
│
├── Executor wakeup
│
├── Callback start
│
├── Processing
│
└── Callback end
```

这就是这个岗位非常需要的能力。

---

# 九、你需要做一个“对标 JD 的项目”

这是我最建议你做的。

不要继续：

> 今天学 TF2，明天学 Nav2。

而是做一个：

# ROS2 Robot Performance Lab

架构：

```text
                   Camera
                     │
                     ↓
                  Driver
                     │
                     ↓
              /camera/image
                     │
            ┌────────┴────────┐
            ↓                 ↓
       Perception A      Perception B
            │                 │
            └────────┬────────┘
                     ↓
                   Fusion
                     │
                     ↓
                  Planner
                     │
                     ↓
                  Control
```

然后人为制造：

```text
CPU 高
Memory leak
Callback blocking
Message backlog
QoS mismatch
Network delay
Timestamp offset
Thread contention
```

然后自己定位。

---

# 十、这个项目可以直接覆盖 JD 的 80%

比如：

### 实验 1：消息延迟

```text
10Hz
30Hz
60Hz
120Hz
```

测：

```text
P50/P95/P99
```

---

### 实验 2：消息堆积

让：

```text
Publisher = 100Hz
Subscriber processing = 20ms
```

观察：

```text
queue
latency
memory
```

---

### 实验 3：Executor

比较：

```text
SingleThreaded
MultiThreaded
Callback Group
```

---

### 实验 4：CPU

使用：

```text
top
perf
pidstat
```

定位：

```text
node
thread
function
```

---

### 实验 5：内存

制造：

```text
leak
message backlog
large allocation
```

使用：

```text
ASan
Valgrind
pmap
```

---

### 实验 6：网络

使用：

```text
tcpdump
tc
iperf3
```

人为：

```text
delay
loss
bandwidth limit
```

观察 ROS2：

```text
latency
drop
reliability
```

---

### 实验 7：时间同步

模拟：

```text
Camera + LiDAR + IMU
```

人为增加：

```text
timestamp offset
clock drift
transport delay
```

观察 fusion。

---

# 十一、你现在和这个岗位的差距，我会这样排

### 已经很强

```text
DDS                 ██████████
Fast DDS             ██████████
Middleware           █████████
C/C++                 ████████
ROS2底层              ████████
```

### 需要明显加强

```text
ROS2应用机制         █████
Linux profiling      ████
Tracing              ███
IPC/SHM              █████
TF2                  ███
时间同步              ██
机器人数据链路        ██
```

### 不需要投入太多

```text
感知算法              ██
SLAM算法              ██
Nav2算法内部          ██
MoveIt算法内部        █
```

---

# 十二、如果你准备 3 个月后投这个岗位

我会给你这样的路线：

```text
第1个月
ROS2 Runtime
+
Linux Performance
```

重点：

```text
Executor
QoS
Lifecycle
Composition
TF2
perf
gdb
ASan
strace
```

↓

```text
第2个月
Robot Data Pipeline
+
Tracing
```

重点：

```text
Camera
LiDAR
IMU
TF2
Timestamp
Synchronization
IPC
SHM
ros2_tracing
```

↓

```text
第3个月
系统级项目
```

完成：

```text
ROS2 Robot Performance Lab
```

最后形成一个完整案例：

> **“针对多传感器、多节点机器人系统，建立端到端通信性能分析链路，实现 CPU、内存、通信延迟、消息堆积、线程调度和数据同步问题的定位与优化。”**

这句话和 JD 的匹配度会非常高。

---

# 十三、还有一个很重要的点：面试时不要只讲“我会什么”

你应该准备 **5 个完整的故障定位故事**。

例如：

### Story 1：通信延迟

```text
现象
 ↓
指标
 ↓
假设
 ↓
实验
 ↓
定位
 ↓
修改
 ↓
优化结果
```

### Story 2：CPU 高

### Story 3：内存增长

### Story 4：Callback 阻塞

### Story 5：消息丢失 / 数据不同步

每个故事最好能够讲：

> **现象 → 定位 → 根因 → 修改 → 数据结果**

例如：

> “某 ROS2 节点 CPU 从 40% 上升到 85%，我首先用 `top -H` 定位到具体线程，再用 `perf` 发现主要消耗在消息序列化，随后通过 composition/intra-process 减少数据拷贝，最终 CPU 降到 52%。”

这种回答比：

> “我熟悉 ROS2、DDS、Linux、perf。”

有价值得多。

---

# 十四、结合你现在的情况，我认为这个岗位其实是一个很好的目标岗位

因为它要求的能力恰好形成：

```text
                 System Performance
                        ↑
                        │
              Robotics Runtime
                        ↑
                 ROS2 Middleware
                        ↑
             DDS ←──────┼──────→ Zenoh
                        │
                Linux / Network
                        │
             CPU / Memory / Thread
```

而你的已有优势正好在：

```text
汽车 Middleware
      ↓
DDS
      ↓
Fast DDS
      ↓
ROS2
```

你现在**不要把时间大量花在“重新学算法”上**。

最应该做的是把缺的这几个模块补起来：

> **Linux 性能分析 + ROS2 Runtime + TF2/时间同步 + IPC/SHM + tracing + 真实机器人数据链路。**

然后用一个完整的 **ROS2 Robot Performance Lab** 把它们串起来。

如果你能把这个项目真正做出来，再把其中 **2～3 个问题深入到源码 + perf/trace + 实验数据**，我认为你和这个 JD 的匹配度会从现在的 **60%～70% 提升到 85%～90% 左右**。

而且这条路线比单纯继续钻 Fast DDS 更值得，因为它正好把你从 **“DDS 专项”** 推到 **“机器人系统性能/Runtime”**。

---
