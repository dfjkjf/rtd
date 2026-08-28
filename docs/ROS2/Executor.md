---
layout: default
title: Executor
nav_order: 2
---

# Executor Wait Set Callback 都是rclpp 的概念吗？如何理解？以及 为什么 ROS2 需要 Executor？

## 一、它们都是 rclcpp 的概念吗？——不全是，三者所处的层次不同

| 概念 | 所在层次 | 说明 |
|---|---|---|
| **Executor** | 纯 rclcpp 概念 | rcl 里没有 executor。rclpy 也有自己的 executor 实现，两者互不依赖——它是**客户端库层**的概念，不是 ROS 2 核心层概念 |
| **Wait Set** | 三层都有 | 它是一个贯穿栈各层的机制（见下文） |
| **Callback** | 用户级回调是 rclcpp 概念 | `std::function` 形式的 `topic_callback` 只存在于 rclcpp；rcl/rmw 层只有低级 C 事件回调 `rmw_event_callback_t` |

### Wait Set 的三层形态

```
rclcpp::WaitSet / Executor 内部 wait_set_     ← C++ 封装 + 策略（ThreadSafe/Sequential...）
        rcl_wait_set / rcl_wait (rcl/wait.c)  ← C 原语：收集实体、阻塞等待
              rmw_wait → Fast DDS WaitSet     ← 真正的条件等待（StatusCondition）
```

第一轮追代码时你已经见过整条链：`Executor::wait_for_work` → `rcl_wait`→ `rmw_wait` → `fastdds_wait_set->wait`

---

## 二、如何理解这三个概念

用 Unix 网络编程类比，一下就通了：

**WaitSet ≈ select/epoll**
"同时监视一批实体，任何一个就绪就返回"。只不过监视的对象不是 socket，而是 subscription、timer、service、client、guard condition、event。

**Callback ≈ 就绪后的处理函数**
用户逻辑的单位。但注意回调本身不知道自己何时该跑——它只被**注册**进一个 `CallbackGroup`（[callback_group.hpp#L46-L50](file:///home/user/workspace/ros2_debug_ws/src/rclcpp/rclcpp/include/rclcpp/callback_group.hpp#L46-L50)），组类型决定并发语义：

- `MutuallyExclusive`（默认）：组内同一时刻只跑一个回调——保证你的 `topic_callback` 不会重入；
- `Reentrant`：允许并发——适合想并行处理多个 topic 的场景。

**Executor ≈ 事件循环（event loop）本身**
把上面两者粘起来：收集各 callback group 的实体 → 组装等待集 → 阻塞等就绪 → 找出就绪实体 → take 消息 → 执行回调 → 循环。`rclcpp::spin(node)` 只是"创建一个 `SingleThreadedExecutor` 并 spin"的语法糖。

还有个容易忽略的细节印证这个模型：executor 自己持有一个 `interrupt_guard_condition_`（[executor.cpp#L48](file:///home/user/workspace/ros2_debug_ws/src/rclcpp/rclcpp/src/rclcpp/executor.cpp#L48)）。当节点/回调组被加入或移除时，需要**中断正在阻塞的 `rcl_wait`**，让它重新收集实体——这就是 guard condition 作为"人工事件源"的典型用途。

---

## 三、为什么 ROS 2 需要 Executor？

核心原因一句话：**把"IO 就绪"和"回调执行"解耦**。

### 1. 吸取 ROS 1 的教训

ROS 1 里回调是消息一到就执行的（由 AsyncSpinner/网络线程直接驱动），回调跑在哪个线程、和谁并发，开发者基本无法控制，多线程程序里竞态条件层出不穷。ROS 2 反过来：DDS 接收线程**只发信号**（`on_data_available` / StatusCondition），**绝不在中间件线程里执行用户回调**；回调一律由 executor 线程取出执行。何时跑、按什么顺序跑、能否并发，全部由 executor 决定。

### 2. 统一异构实体的调度

一个节点里同时有订阅、定时器、服务、客户端、事件。没有 executor，你就得手写一个"检查哪些实体就绪、再分发处理"的循环——这正是一个易写错、易漏实体的活。Executor 把这层样板代码标准化了：所有实体在 `AnyExecutable` 里被统一表达（[executor.cpp#L509-L520](file:///home/user/workspace/ros2_debug_ws/src/rclcpp/rclcpp/src/rclcpp/executor.cpp#L509-L520)：`any_exec.timer` / `subscription` / `service` / `client`）。

### 3. 并发语义可声明、可组合

- 单线程执行器：所有回调串行，最简单的心智模型；
- `MultiThreadedExecutor` + `Reentrant` 组：显式声明哪些回调可以并行；
- 多个节点共享一个执行器（组件容器 `component_container_mt` 就是这么跑的），或把不同回调组分派给不同执行器。

这种"节点可以自由拼装进不同调度域"的组合能力，是 ROS 1 完全没有的。

### 4. 策略可扩展

Executor 是个抽象基类，官方就有三种实现（单线程 / 多线程 / 静态单线程），社区还有 `EventsExecutor`（用你在第一轮见过的 `on_new_message_cb_` 机制，完全绕过等待集轮询）。调度策略和实体管理也被拆成了可替换的 `MemoryStrategy` 和等待集策略。

---

## 总结

- **Executor**：rclcpp 独有，事件循环/调度器——回答"**何时、在哪个线程、以什么顺序**执行回调"。
- **WaitSet**：跨三层的就绪多路复用机制——回答"**谁就绪了**"。
- **Callback**：用户逻辑单位，靠 CallbackGroup 获得并发语义——回答"**执行什么**"。

ROS 2 需要 Executor，本质上是把并发控制权从中间件手里拿回到用户框架手里：**DDS 只管"通知"，executor 负责"决定"**。


# 实验

## SingleThreadedExecutor 中，一个耗时 Callback 是否会阻塞其他 Callback？

系统设计：

```text
                    SingleThreadedExecutor
                           │
              ┌────────────┼────────────┐
              ↓            ↓            ↓
         Callback A    Callback B    Callback C
             1ms           10ms          100ms
```

三个 Topic：

```text
/topic_a
/topic_b
/topic_c
```

Publisher 分别发布消息：

```text
A → 20 Hz
B → 10 Hz
C → 2 Hz
```

Subscriber Callback 模拟：

```text
A → 工作 1ms
B → 工作 10ms
C → 工作 100ms
```

观察：

```text
C Callback
   │
   ├── 是否阻塞 A？
   │
   └── 是否阻塞 B？
```

---

注意最关键的一段：

```cpp
rclcpp::executors::
  SingleThreadedExecutor executor;
```

这意味着：

```text
整个 Node
       │
       ↓
一个 Executor
       │
       ↓
一个执行线程
```

所以：

```text
Callback A
Callback B
Callback C
```

不能同时执行。

---

## 记录实验数据

建议建立：

```text
experiments/executor/
```

每个实验一份数据：

```text
experiment_1.csv

callback,
work_time_ms,
avg_latency_ms,
p95_latency_ms,
p99_latency_ms
```

例如：

| Callback |  Work | Avg Latency |  P99 |  xx |
| -------- | ----: | ----------: | ---: | ---: |
| A        |   1ms |         2ms |  5ms |  xx |
| B        |  10ms |         5ms | 15ms | xx |
| C        | 100ms |         1ms |  3ms |  xx |

然后：

```text
C = 500ms
```

| Callback |  Work | Avg Latency |   P99 |
| -------- | ----: | ----------: | ----: |
| A        |   1ms |       150ms | 400ms |
| B        |  10ms |       120ms | 350ms |
| C        | 500ms |         1ms |  10ms |

这样你就真正有：

> **性能数据。**

---

### 最后换 MultiThreadedExecutor

第一版：

```cpp
rclcpp::executors::
SingleThreadedExecutor executor;
```

改成：

```cpp
rclcpp::executors::
MultiThreadedExecutor executor;
```

重新运行。

但是这里你很可能发现：

> **结果没有你想象中那么好。**

原因是：

```text
默认 Callback Group
=
MutuallyExclusive
```

所以后面就自然进入：

### Callback Group 实验

结构变成：

```text
Executor
     │
     ├── Thread 1
     │
     ├── Thread 2
     │
     └── Thread 3

Callback Groups
     │
     ├── MutuallyExclusive
     │
     └── Reentrant
```

---

## 这个实验最终要得到的结论

你最后不是写：

> “SingleThreadedExecutor 是单线程。”

> “为什么 MultiThreadedExecutor 不能解决 Callback Blocking？”

而应该形成：

> 当多个 Subscription Callback 共享 SingleThreadedExecutor 时，任意一个耗时 Callback 都可能占用唯一的执行线程，导致其他已经 Ready 的 Callback 无法及时执行。消息传输完成时间与 Callback 实际开始时间之间因此出现 Executor Scheduling Delay；当输入速率持续超过系统处理能力时，会进一步产生队列堆积，并表现为端到端延迟和抖动增加。

> MultiThreadedExecutor 只是提供多个执行线程，并不意味着所有 Callback 都可以并发执行。Callback 是否能够同时执行还受到 Callback Group 的约束。如果多个 Subscription 使用同一个 MutuallyExclusive Callback Group，即使 Executor 有多个线程，同一时间仍然只能执行一个 Callback。因此优化 ROS2 Runtime 时，需要同时分析 Executor 线程数量、Callback Group 划分、Callback 执行时间以及消息到达速率，而不能简单地通过增加 Executor 线程解决延迟问题。

# 完整的系统模型

## "真多线程"的完整真值表

| | MutuallyExclusive（默认） | Reentrant |
|---|---|---|
| **SingleThreadedExecutor** | 串行 | 串行（只有 1 个线程） |
| **MultiThreadedExecutor** | 组内串行 ≈ 伪并行 | **真并行** |

所以准确说法是：**并行 = 多线程 Executor + Reentrant 组，缺一不可**。

## 代价（真实系统里必须知道的三件事）

1. **回调真的并发了**：所有共享状态必须自己加锁（本实验我已给样本向量加了 `samples_mutex_`；真实节点里指令、地图、状态机等都要重新审视）。大量现成节点是按"串行执行"的隐含假设写的，直接换 Reentrant 可能引入竞态。
2. **不再保证顺序**：同一话题的 msg k+1 可能比 msg k 先执行完。依赖顺序语义（如轨迹、积分）就不能用 Reentrant。
3. **饱和依然存在，只是条件变了**：当 并发需求 > 线程数时，排队和丢包照样回来。


```text
                     Network
                        │
                        ↓
                      DDS
                        │
                        ↓
                 RMW Subscription
                        │
                        ↓
                    Wait Set
                        │
                        ↓
                 Executor Ready
                        │
                        ↓
                 Callback Group
                        │
             ┌──────────┴──────────┐
             ↓                     ↓
       Execution Allowed?       Blocked
             │
             ↓
       Executor Thread
             │
             ↓
         Callback Start
             │
             ↓
         Callback End



                DDS DataReader 收到数据
                        │
                        │ ① SubListener::on_data_available 触发
                        ↓
              RMW wait set 被唤醒（被动方）
                        │
                        ↓
           Executor::wait_for_work() 返回
           订阅进入就绪列表 subscription_handles_   ← "Executor Ready"
                        │
                        ↓
        get_next_ready_executable_from_map()
        按就绪列表顺序逐个检查（A→B→C 的来源）
                        │
                        ↓
              Callback Group 闸门
             group->can_be_taken_from()?
             ┌──────────┴──────────┐
             ↓                     ↓
        ME 且占用中：跳过      放行（ME 则置标志为 false）
        （留在就绪列表，              │
          下轮循环重试）              │
             │                     ↓
             └──────────→  execute_subscription()
                                 │
                                 ↓
                    ② rcl_take → rmw_take（此刻才出队 + 反序列化）
                                 │
                                 ↓
                           Callback Start ──→ Callback End
                                   （结束时恢复组的 can_be_taken_from）
```

真正的 Callback Latency：

```
publish() 调用
   │
   │ ① Transport Delay        网络/共享内存传输，数据进 DataReader 队列
   ↓
   │ ② Wakeup Delay           DDSWaitSet 返回 → rcl_wait 返回 → executor 本轮循环
   ↓
   │ ③ Waiting（大头，见下文分解）  在"就绪但没被选中"状态停留
   ↓
   │ ④ Take Delay             rmw_take 出队 + 反序列化（~几十µs）
   ↓
Callback Start  ← 你的实验测到这里为止
```


### ③ Waiting 按运行模式分解——这才是关键

③ 是唯一会暴涨的项，它的构成取决于调度模式：

| 模式 | ③ 的构成 | 你的实测证据 |
|---|---|---|
| Single / ME | 前面所有就绪回调的**执行时间之和**（单线程 FIFO）＋ 组锁 | C 的 12ms = 排在 A(1ms)+B(10ms) 后面；A Max 63ms = 排在突发队尾 |
| Multi + Reentrant，线程够 | ≈ 0 | 全线 0.7~0.9ms |
| Multi + Reentrant，线程不够 | 线程池排队（新瓶颈） | 你还没测到这个区 |


- **基线 ≈ 0.7~0.9ms**（Reentrant 全表）：这就是 ①+②+④+OS 调度的总和，是你这套测量能看到的**下限**，无法再细分（除非在 rmw listener 里加时间戳）。
- **ME 下 C 的额外 11ms**：纯③，= 前序 A、B 的执行时间。
- **串行饱和区（440/500）的数百毫秒**：纯③，且随时间线性增长；此时①②④仍然只有亚毫秒——**延迟爆炸 100% 来自调度等待，与传输无关**。

