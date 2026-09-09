---
layout: default
title: RobotDataPipeline
nav_order: 5
---

非常好。**Week 5 的 TF2 可以正式算进入“机器人系统层”了。**

你前面几周主要在建立：

> CPU / Thread / Scheduler / Executor / DDS / Callback

这些解决的是：

> **数据怎么产生、怎么传输、谁在什么线程执行、为什么会延迟。**

而 TF2 开始解决一个完全不同、但机器人系统极其核心的问题：

> **这个数据“在哪里”？不同传感器看到的东西，怎么放到同一个空间坐标系里？**

我建议这一周不要按照 API 去学，而是按照一个核心问题推进：

# Week 5 总问题

> **机器人系统为什么需要 TF？如果 TF 出错，会让整个机器人产生什么现象？**

---

# Week 5：TF2 实验路线

## Day 1：先理解 Frame —— “坐标系到底是什么”

先不要急着写 TF。

理解几个典型 Frame：

```text
map
 |
odom
 |
base_link
 |\
 | \
 |  camera_link
 |
 laser
 |
 imu_link
```

你要建立的第一层认知是：

### 一个机器人没有一个“坐标系”

而是同时存在大量坐标系。

例如：

```text
camera_link
```

表示：

> 相机自己的坐标系。

相机看到一个点：

```text
(1, 0, 0)
```

并没有意义，直到你知道：

> **这是相对于哪个 Frame？**

例如：

```text
Point:
x = 1
y = 0
z = 0

Frame:
camera_link
```

意思才是：

> 这个物体在相机前方 1 米。

---

三个概念

今天结束时，你只需要能够解释：

① Frame

Frame 是一个坐标参考系，用来描述数据的位置和方向。

② Transform

Transform 描述两个 Frame 之间的位置和方向关系。

③ 为什么需要多个 Frame

因为机器人上的不同传感器和部件位于不同位置，它们产生的数据天然是相对于自己的坐标系描述的。机器人需要通过 Transform 把这些数据转换到统一坐标系中。


```text
map
    ↓
全局参考，负责“我在世界中的位置”

odom
    ↓
连续运动参考，负责“我从起点连续走了多少”

base_link
    ↓
机器人自身，机器人本体

camera_link
    ↓
传感器自身
```

TF 不是“机器人位置”，TF 是 Frame 之间的空间关系；机器人位置只是通过 TF Tree 中的一系列 Transform 组合出来的。

Transform = 某一个时间点上，两个 Frame 之间的空间关系。
---

## Day 1 实验

建议你写一个最简单的 TF Publisher。

发布：

```text
base_link
   |
   |  (1m forward)
   |
camera_link
```

也就是：

```text
parent_frame = base_link
child_frame  = camera_link
```

例如：

```cpp
transform.header.frame_id = "base_link";
transform.child_frame_id = "camera_link";

transform.transform.translation.x = 0.3;
transform.transform.translation.y = 0.0;
transform.transform.translation.z = 1.0;
```

然后：

```bash
ros2 topic echo /tf
```

你应该看到类似：

```text
header:
  stamp:
  frame_id: base_link

child_frame_id: camera_link
```

### Day 1 必须回答

> TF Message 传输的到底是什么？

答案不是：

> “坐标”。

而是：

> **两个坐标系之间的空间关系。**

即：

```text
Transform(base_link -> camera_link)
```

---

# Day 2：理解机器人常见 Frame

这是非常重要的一天。

你需要真正理解：

```text
base_link
odom
map
camera_link
laser
imu_link
```

---

## base_link

```text
base_link
```

通常可以理解成：

> **机器人本体的主要坐标系。**

例如：

```text
        Robot
          |
      base_link
```

所有传感器的位置最终都可以描述成：

```text
sensor_frame
      ↓
base_link
```

例如：

```text
camera_link
     |
     |
base_link
```

---

## camera_link

表示：

> 相机安装位置。

如果：

```text
camera_link
```

相对于：

```text
base_link
```

是：

```text
x = 0.3
z = 1.0
```

那么：

> 相机位于机器人前方 0.3m，高度 1m。

---

## laser

例如：

```text
laser
```

表示：

> 激光雷达自己的坐标系。

激光雷达检测：

```text
(5, 0)
```

意思是：

> 前方 5m。

但如果你想知道：

> 这个障碍物在地图哪里？

就必须进行：

```text
laser
   ↓
base_link
   ↓
odom
   ↓
map
```

这就是 TF 的真正价值。

---

# Day 2 核心实验

人为构造：

```text
camera_link
      |
      |
      v
base_link
      |
      |
      v
odom
```

然后发布一个点：

```text
PointStamped
```

例如：

```text
frame_id = camera_link

point:
x = 2
y = 0
z = 0
```

然后用 TF2：

```text
camera_link → odom
```

进行 Transform。

你要观察：

> **Point 的数值变化了，但它描述的真实物理位置没有变化。**

这是理解坐标变换的关键。

---

# Day 3：理解 `/tf` 和 `/tf_static`

这是 TF2 非常重要的一部分。

---

## `/tf`

通常用于：

> 会变化的坐标关系。

例如机器人运动：

```text
odom
  |
  |   continuously changing
  |
base_link
```

机器人移动：

```text
t1

odom ---- base_link
```

过 1 秒：

```text
t2

odom -------- base_link
```

因此：

```text
odom → base_link
```

需要不断发布。

Topic：

```bash
/tf
```

---

## `/tf_static`

用于：

> 永远不会变化的坐标关系。

例如：

```text
base_link
     |
     |
camera_link
```

如果相机固定在机器人上：

```text
camera_link
```

相对于：

```text
base_link
```

永远不会变化。

所以：

```text
base_link → camera_link
```

放在：

```bash
/tf_static
```

---

# Day 3 实验

启动：

一个动态 TF：

```text
odom → base_link
```

一个静态 TF：

```text
base_link → camera_link
```

观察：

```bash
ros2 topic hz /tf
```

以及：

```bash
ros2 topic echo /tf_static
```

思考：

> 为什么动态 TF 需要持续发送，而静态 TF 不需要？

---

# Day 4：Timestamp 才是真正的 TF2 核心

这里开始和你前面的：

```text
Executor
DDS
Scheduler
Latency
```

连接起来。

TF2 不是简单保存：

```text
A → B
```

而是保存：

```text
A → B
at time T
```

例如：

```text
odom → base_link

time = 10.0s
x = 1m
```

和：

```text
time = 11.0s
x = 2m
```

是不同的 Transform。

因此 TF Buffer 实际保存的是：

```text
Transform History
```

概念上类似：

```text
Time
 |
 |
10.0 ---- Transform A
10.1 ---- Transform B
10.2 ---- Transform C
10.3 ---- Transform D
```

---

# Transform Lookup

当 Camera 数据到达：

```text
timestamp = 10.15
frame = camera_link
```

系统可能需要：

```text
Transform(camera_link → map)

at time 10.15
```

TF2 就需要从：

```text
Buffer
```

中找到正确的 Transform。

这就是：

```text
transform lookup
```

---

# Day 4 必做实验

人为制造：

```text
TF timestamp
```

和：

```text
Sensor timestamp
```

不一致。

例如：

```text
TF:

timestamp = now - 500ms
```

而 Sensor：

```text
timestamp = now
```

然后进行：

```cpp
lookupTransform()
```

观察：

```text
ExtrapolationException
```

---

# 你需要理解的一个关键问题

假设：

```text
Camera Image

timestamp = 100.0
```

但是 TF Buffer 最新数据：

```text
99.5
```

那么系统无法准确回答：

> 在 100.0 秒时，camera_link 相对于 map 在哪里？

这就是：

> **TF delay。**

注意，这个问题和 DDS 非常容易产生联系。

例如真实系统：

```text
Camera Thread
      |
      |
      v
DDS receive
      |
      |
Executor
      |
      |
Callback
```

导致 Image callback 延迟。

与此同时：

```text
TF publisher
```

也可能延迟。

最后出现：

```text
Sensor timestamp = T

TF available = T - Δt
```

然后：

```text
lookupTransform(T)
```

失败。

这就是你未来做性能优化时非常重要的思维：

> **不是看到 TF lookup failed 就认为 TF 有问题。**

可能真正的问题是：

```text
CPU
 ↓
Thread scheduling delay
 ↓
Callback delay
 ↓
TF publish delay
 ↓
TF Buffer not updated
 ↓
ExtrapolationException
```

这就是典型的：

> **系统因果链分析。**

```text
TF异常
 │
 ├── 发布频率问题
 ├── timestamp 问题
 ├── Buffer 时间范围问题
 ├── DDS 通信延迟
 ├── Executor 排队/调度延迟
 └── CPU / scheduler / blocking
```

---

# Day 5：故障注入实验 🔥

这一部分最适合你。

你前面已经在做：

```text
CPU
Thread
Scheduler
Executor
DDS
```

现在可以开始把 TF 当成：

> **机器人系统的时间敏感数据链。**

---

# 实验 1：Timestamp Offset

Publisher：

```text
TF timestamp = now() - 500ms
```

然后：

```cpp
lookupTransform(
    "odom",
    "base_link",
    now()
);
```

观察：

```text
ExtrapolationException
```

改变：

```text
100ms
300ms
500ms
1000ms
```

记录：

| Offset | Lookup |
| ------ | ------ |
| 0ms    | OK     |
| 100ms  | ?      |
| 500ms  | ?      |
| 1000ms | ?      |

重点不是结果本身。

重点是：

> **TF lookup 对时间的要求到底是什么？**

---

# 实验 2：TF Delay

正常：

```text
TF publish:

100Hz
```

然后人为：

```cpp
sleep_for(100ms)
```

或者：

```cpp
sleep_for(500ms)
```

观察：

```bash
ros2 topic hz /tf
```

同时：

```cpp
lookupTransform()
```

观察：

```text
success
timeout
extrapolation
```

建立：

```text
TF delay → Buffer freshness → Lookup behavior
```

的因果关系。

---

# 实验 3：TF Frequency Jitter

正常情况下：

```text
10ms
10ms
10ms
10ms
10ms
```

人为制造：

```text
5ms
20ms
100ms
10ms
200ms
```

即：

> jitter。

例如：

```cpp
std::this_thread::sleep_for(
    random_duration()
);
```

观察：

```bash
ros2 topic hz /tf
```

你会发现：

> Average frequency 可能看起来没有问题。

但是：

```text
max interval
```

可能非常大。

例如：

```text
average = 100Hz
```

但：

```text
某一次间隔 = 500ms
```

这对机器人系统非常危险。

---

# 这里和你 Week 3 / Week 4 完美连接

你之前研究：

```text
CPU high
thread running
scheduler delay
executor blocking
```

现在可以得到：

```text
CPU contention
      ↓
TF publisher thread delayed
      ↓
TF publish jitter
      ↓
TF Buffer update delayed
      ↓
Transform lookup extrapolation
      ↓
Robot perception/navigation failure
```

这就是：

> **Linux Performance → ROS2 → Robot Behavior**

的完整链条。

---

# Weekend：建立 Robot TF Tree

建议你不要一开始使用真实复杂机器人。

先建立自己的最小机器人：

```text
                         map
                          |
                          |
                         odom
                          |
                          |
                      base_link
                     /    |     \
                    /     |      \
                   /      |       \
          camera_link   laser   imu_link
```

其中：

```text
map → odom
```

可以先固定。

```text
odom → base_link
```

动态变化。

```text
base_link → camera_link
base_link → laser
base_link → imu_link
```

使用静态 TF。

---

## 建议启动结构

```text
robot_tf_demo
│
├── dynamic_tf_publisher
│       odom → base_link
│
├── static_tf_publisher
│       base_link → camera
│       base_link → laser
│       base_link → imu
│
├── tf_listener
│       lookupTransform()
│
└── RViz2
```

---

# RViz2 在这里的真正作用

你这一周的 RViz2 **不是用来学怎么拖控件**。

而是：

> **把抽象的 TF 关系可视化。**

你可以观察：

```text
base_link
```

移动时：

```text
camera
laser
imu
```

是否一起移动。

如果：

```text
camera_link
```

位置错误：

```text
camera
```

就会出现在错误的位置。

这可以帮助你真正理解：

> TF 错误最终不是“一个 Topic 错误”。

而可能表现为：

```text
Camera point cloud 漂移
Laser obstacle 偏移
Navigation path 错误
Robot localization 跳动
```

---

# 这一周最终要形成的系统模型

我建议你最后把脑中的 TF2 模型整理成：

```text
                Sensor Data
                     |
                     |
              frame_id + timestamp
                     |
                     v
                TF Lookup
                     |
                     |
                     v
              TF2 Buffer
                     |
                     |
          Transform History
                     |
                     v
      Frame A <----> Frame B
                     |
                     v
           Coordinate Transform
                     |
                     v
           Robot World Model
```

而底层实际上还有：

```text
TF Publisher
     |
     v
    /tf
     |
     v
    DDS
     |
     v
ROS2 Executor
     |
     v
TF Listener
     |
     v
TF Buffer
```

所以 TF2 最重要的理解其实是：

> **TF 不是单纯的坐标变换库。**

在真实机器人系统中，它是：

> **带时间维度的、分布式维护的机器人空间关系数据库。**

---

## Week 5 最终验收问题

到周末，你应该能够不看资料回答：

1. **TF 和 Frame 的区别是什么？**
2. **为什么机器人不能只使用一个坐标系？**
3. **base_link、odom、map 分别解决什么问题？**
4. **为什么 camera_link 通常挂在 base_link 下？**
5. **/tf 和 /tf_static 的区别是什么？**
6. **TF 为什么必须有 timestamp？**
7. **TF Buffer 保存的到底是什么？**
8. **什么是 transform lookup？**
9. **什么是 extrapolation？**
10. **TF delay 最终可能由哪些底层问题导致？**
11. **为什么 TF frequency 的 average 正常，但机器人仍可能出问题？**
12. **DDS receive thread、Executor delay 和 TF lookup failure 如何形成因果链？**

---

下面我按**面试口语回答**来组织，不追求背书，而是让你能体现出“系统模型 + 因果分析”的能力。

## 1. TF 和 Frame 的区别是什么？

**Frame 是坐标系，TF 是描述两个 Frame 之间空间关系的机制。**

例如：

```text
base_link
    ↓
camera_link
```

`base_link` 和 `camera_link` 是两个 Frame，而 TF 描述：

> 在某个时间点，`camera_link` 相对于 `base_link` 的位置和姿态。

所以可以简单理解：

```text
Frame      → 坐标系
Transform  → 两个坐标系之间的空间变换
TF/TF2     → 管理和查询这些 Transform 的机制
```

---

## 2. 为什么机器人不能只使用一个坐标系？

因为机器人内部存在大量**不同物理位置、不同参考对象的数据源**。

例如：

```text
camera_link
laser
imu_link
base_link
odom
map
```

相机测量的是相机坐标系下的数据，激光雷达是雷达坐标系，IMU 也是自己的坐标系。

如果全部强行使用一个坐标系：

* 传感器安装位置变化后很难表达；
* 不同模块无法保持各自自然的参考系；
* 全局定位、局部运动、本体坐标也混在一起。

所以机器人采用多个 Frame，再通过 TF 描述它们之间的关系：

```text
map
 ↓
odom
 ↓
base_link
 ├── camera_link
 ├── laser
 └── imu_link
```

---

## 3. `base_link`、`odom`、`map` 分别解决什么问题？

这是面试高频题。

### `base_link`

表示**机器人本体的参考坐标系**。

例如：

```text
base_link → camera_link
base_link → laser
base_link → imu_link
```

主要解决：

> 机器人自身以及传感器之间的空间关系。

---

### `odom`

表示机器人**连续运动的局部参考系**。

通常来自：

* 轮速里程计
* VIO
* 激光里程计等

它的特点通常是：

> 短时间内连续、平滑，但长期可能产生漂移。

---

### `map`

表示机器人在**全局环境中的参考系**。

通常由：

* SLAM
* 定位
* 地图系统

提供。

它解决的是：

> 机器人在全局环境中的位置。

所以经典结构：

```text
map
 ↓
odom
 ↓
base_link
```

可以理解成：

```text
map       → 全局
odom      → 局部连续运动
base_link → 机器人自身
```

---

## 4. 为什么 `camera_link` 通常挂在 `base_link` 下？

因为相机通常是**固定安装在机器人本体上的**。

例如：

```text
base_link
    ↓
camera_link
```

如果机器人移动：

```text
odom → base_link
```

发生变化，但是：

```text
base_link → camera_link
```

通常不变。

所以它是一个 Static Transform。

这样 TF2 就可以自动组合：

```text
odom
 ↓
base_link
 ↓
camera_link
```

得到：

```text
odom → camera_link
```

这比直接维护每一个传感器相对于 `odom` 的位置更加合理。

---

# 5. `/tf` 和 `/tf_static` 的区别是什么？

核心区别：

> **`/tf` 用于动态 Transform，`/tf_static` 用于静态 Transform。**

例如：

```text
odom → base_link
```

机器人运动时不断变化：

```text
x = 1.0
x = 1.1
x = 1.2
...
```

因此通常发布到：

```text
/tf
```

而：

```text
base_link → camera_link
```

相机固定安装，通常不变化：

```text
x = 0.5
z = 1.0
```

因此发布到：

```text
/tf_static
```

这样可以避免把大量不变的数据当成动态数据反复发布。

---

# 6. TF 为什么必须有 timestamp？

因为机器人是**运动的**。

Transform 不仅仅是：

```text
A → B
```

而实际上是：

```text
T 时刻：
A → B
```

例如：

```text
10.0s → base_link x = 5m
10.1s → base_link x = 5.5m
10.2s → base_link x = 6m
```

假设相机数据是在 `10.1s` 产生的。

那么应该查询：

```text
10.1s 的 TF
```

而不是 callback 当前时间 `10.2s` 的 TF。

否则：

```text
传感器数据时间
        ≠
TF 时间
```

就可能产生空间上的错误。

所以 TF 本质上是一个：

> **带时间维度的空间关系。**

---

# 7. TF Buffer 保存的到底是什么？

可以把 TF Buffer 理解成：

> **TF Transform 的时间历史缓存。**

例如：

```text
10.0s → odom → base_link → x=5.0
10.1s → odom → base_link → x=5.5
10.2s → odom → base_link → x=6.0
```

Buffer 不只是保存：

```text
odom → base_link
```

还保存这个 Transform 的**时间历史**。

因此 lookup 时可以问：

> “给我 `10.1s` 时刻的 `odom → base_link`。”

所以：

```text
TF Buffer
   ↓
Frame关系 + Transform历史 + 时间信息
```

---

# 8. 什么是 transform lookup？

就是：

> **根据目标 Frame、源 Frame 和时间，从 TF Buffer 中查询对应的 Transform。**

例如：

```cpp
tf_buffer_->lookupTransform(
    "odom",
    "camera_link",
    time);
```

本质是在问：

> 在 `time` 这个时间点，`camera_link` 和 `odom` 之间是什么空间关系？

TF2 会沿着 TF Tree 查找：

```text
odom
 ↓
base_link
 ↓
camera_link
```

然后组合这些 Transform，得到最终结果。

---

# 9. 什么是 extrapolation？

Extrapolation 的核心就是：

> **你查询的时间超出了 TF Buffer 当前拥有的时间范围。**

假设 Buffer：

```text
10.0 ───────────── 10.3
oldest             latest
```

查询：

```text
9.5
```

就是：

```text
9.5 < 10.0
```

属于 **past extrapolation**。

查询：

```text
10.8
```

就是：

```text
10.8 > 10.3
```

属于 **future extrapolation**。

所以看到：

```text
Lookup would require extrapolation...
```

第一反应应该是检查：

```text
T_query
T_oldest
T_latest
```

而不是马上修改 TF 代码。

---

# 10. TF delay 最终可能由哪些底层问题导致？

这个问题建议你用**分层排查**回答。

TF delay 不一定就是 TF Publisher 本身慢。

可能来自：

```text
TF Publisher
    ↓
DDS
    ↓
Executor
    ↓
TF Consumer
    ↓
lookupTransform()
```

具体包括：

### ① Publisher 本身

* callback 执行慢
* blocking
* 锁竞争
* CPU 不足
* 调度延迟

### ② DDS

* 网络延迟
* DDS receive/send thread 调度
* 消息拥塞
* QoS 导致的行为变化

### ③ Executor

* callback 排队
* Executor 被其他 callback 占用
* SingleThreadedExecutor 中 callback 相互阻塞
* CPU contention

### ④ 时间戳

例如：

```text
实际发布时间 = 10.2s
TF timestamp = 10.0s
```

这属于 timestamp offset，并不一定代表 DDS 延迟了 200ms。

### ⑤ 系统时间问题

例如不同节点使用的时间源不一致，也可能导致时间判断异常。

所以我会先区分：

> **timestamp offset、transport delay、processing delay。**

---

# 11. 为什么 TF frequency 的 average 正常，但机器人仍可能出问题？

因为：

> **average frequency 只能告诉你平均发布速率，不能描述每一次消息的时间行为。**

例如目标：

```text
10Hz
```

正常情况下：

```text
100ms
100ms
100ms
100ms
```

但是也可能：

```text
10ms
10ms
10ms
370ms
10ms
10ms
370ms
```

平均下来仍然可能接近 10Hz。

但机器人实际看到的是：

```text
TF
 ↓
连续三次快速更新
 ↓
突然 370ms 没有更新
```

这就是 **jitter**。

所以不能只看：

```text
average rate
```

还应该看：

```text
publication interval
timestamp
age
jitter
Buffer time range
```

这也是为什么：

> **平均值正常 ≠ 实时性正常。**

---

# 12. DDS receive thread、Executor delay 和 TF lookup failure 如何形成因果链？

这是这 12 个问题里**最能体现你水平的一道**。

不要回答成：

> DDS 慢，所以 TF lookup 失败。

应该完整地说：

```text
TF Publisher
     │
     │ timestamp = T
     ↓
    DDS
     │
     │ receive thread 延迟
     ↓
ROS2 middleware
     │
     ↓
Executor
     │
     │ callback 排队 / 调度延迟
     ↓
TF Consumer
     │
     ↓
lookupTransform(T_query)
     │
     ↓
TF Buffer
```

例如：

```text
TF timestamp = 10.0s
```

但是由于：

```text
DDS receive delay = 100ms
Executor delay     = 300ms
```

最终 callback 到：

```text
10.4s
```

如果此时查询的是：

```text
T_query = 10.4s
```

而 TF Buffer 最新只有：

```text
T_latest = 10.0s
```

那么：

```text
10.4 > 10.0
```

就可能出现：

```text
Future Extrapolation
```

于是形成完整因果链：

```text
DDS receive thread 调度延迟
              ↓
消息到达 ROS2 后处理不及时
              ↓
Executor callback 排队
              ↓
TF consumer 延迟执行
              ↓
查询时间相对于 TF 最新时间过新
              ↓
T_query > T_latest
              ↓
lookupTransform()
              ↓
Future Extrapolation
```

但这里还有一个很重要的反向情况：

如果 consumer 查询的是**传感器原始 timestamp**：

```text
T_query = 10.0s
```

即使 callback 在：

```text
10.4s
```

才执行，只要 Buffer 还保存 `10.0s` 的 TF：

```text
T_oldest <= 10.0 <= T_latest
```

那么 **lookup 仍然可能成功**。

---

## 最后给你一个总答案

如果面试官连续问这一串，我建议你最终把它收束成：

> **TF 本质上是机器人中不同 Frame 之间带时间信息的空间关系管理机制。Frame 是坐标系，Transform 是坐标系之间的位姿关系，TF2 负责缓存、查询和组合这些关系。**
>
> **`map → odom → base_link → sensor` 这样的 TF Tree，把全局定位、局部运动、机器人本体和各传感器统一到了一个坐标关系中。**
>
> **由于机器人在运动，Transform 必须关联 timestamp，TF Buffer 保存一定时间范围内的 Transform 历史，lookupTransform 则根据 Frame 和时间进行查询。**
>
> **当查询时间超出 Buffer 的时间范围，就会出现 extrapolation。实际工程中，TF 问题不能只看 TF 本身，还要结合 timestamp、发布频率、jitter、DDS 接收线程、Executor 调度和 callback 排队一起分析。**
>
> **所以看到一个 TF lookup failure，我首先会判断是 Frame/Tree 问题，还是 Time/Buffer 问题，然后进一步沿着 Publisher → DDS → Executor → Buffer → lookup 的链路定位真正延迟来源。**

这一段基本已经可以作为你ms **ROS2 TF + 实时性问题** 的主回答框架。



1. 为什么 base_link → camera_link 通常是静态的，而 odom → base_link 是动态的？

base_link → camera_link 是固定安装关系，机器人跑到哪里，相机和机器人本体之间的关系都不变；
odom → base_link 描述的是机器人当前相对于 odom 的位置，机器人一走，它就变了。

2. TransformBroadcaster::sendTransform() 在做什么？

把 Transform 发布到 TF 系统。

3. 为什么我们没有发布 odom → camera_link，却能够查询到它？

Transform 可以沿 Tree 串起来。你想知道 laser → map，不需要有人专门发布：laser → map，只要整个链是连通的即可。

4. 如果 odom → base_link 正常，但 base_link → camera_link 消失，为什么 odom → camera_link 会失败？

Tree 断了，所以 TF 无法推导

5. TF 是否负责计算机器人的 odometry？

TF 本身不负责“算机器人走了多少”，把别人算出来的空间关系组织、发布、查询起来。

