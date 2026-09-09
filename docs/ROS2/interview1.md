---
layout: default
title: Interview
nav_order: 4
---

这 6 个问题其实可以组成一套非常好的 **ROS2 性能优化面试回答框架**。核心原则是：

> **不要只说“可能是 CPU、锁、线程”。要按照数据流和执行链路回答。**

---

## 1. ROS2 callback 为什么会延迟？

### 面试回答

> ROS2 callback 延迟通常可以从几个阶段分析：首先是消息是否及时从 DDS 接收到，然后消息是否及时进入 ROS2 的处理链路，接着是 Executor 是否及时发现并选择这个 callback，最后线程进入 runnable 状态后是否能够及时获得 CPU。
>
> 所以我通常把 callback 延迟拆成：**通信延迟、Executor 排队延迟、线程调度延迟和 callback 自身的阻塞时间**。
>
> 定位时会结合应用时间戳、Executor 行为和 `perf sched` 来判断。如果 `sch delay` 很高，说明可能是 CPU 调度竞争；如果线程调度延迟很低但 callback latency 很高，则更可能是 Executor 排队、callback 阻塞或者 DDS 接收链路的问题。

一句话总结：

> **先把 callback 延迟拆成通信、Executor 和 Scheduler 三段，而不是直接认为 CPU 不够。**


## 2. MultiThreadedExecutor 为什么可能更快，也可能更慢？

### 面试回答

> MultiThreadedExecutor 在 callback 可以真正并行、CPU 核数足够的情况下，可以减少 callback 之间的串行等待，提高吞吐量并降低延迟。
>
> 但是线程增加后也会带来 CPU 竞争、上下文切换、锁竞争和 Cache 失效。如果线程数量超过实际 CPU 处理能力，或者 callback 本身存在大量共享资源和同步，那么增加线程可能反而增加调度开销和抖动。
>
> 所以 MultiThreadedExecutor 并不是线程越多越快，需要根据 CPU 核数、callback 执行时间、阻塞情况以及 Callback Group 的并发关系来设置线程数。

一句话：

> **它解决的是 callback 串行化问题，但可能引入线程竞争问题。**


## 3. QoS 为什么会导致消息行为变化？

### 面试回答

> 因为 ROS2 的 QoS 最终会影响 DDS 的通信和资源行为。例如 Reliable 和 Best Effort 会影响丢包情况下是否重传；Keep Last 和 Keep All 会影响消息缓存；History depth 会影响队列长度。
>
> 如果消费者处理速度低于生产者，Reliable 配合较大的队列可能导致消息积压，从而表现为延迟增加；而 Best Effort 可能直接丢弃部分消息，但延迟更低。
>
> 所以 QoS 不只是通信配置，它实际上是在可靠性、延迟、吞吐量和资源占用之间做权衡。

一句话：

> **QoS 改变的是消息在 DDS 中的可靠性、缓存和资源管理策略，因此会直接改变丢包、积压和延迟行为。**


## 4. CPU 高怎么定位？

### 面试回答

> 我一般分三步。
>
> 第一步，先判断是进程高还是线程高，用 `top`、`htop` 或 `pidstat -t` 找到具体的高 CPU 线程。
>
> 第二步，把线程级 CPU 使用率和 Linux TID 对应起来，用 `top -H` 或 `ps -L` 确认是哪一个线程。
>
> 第三步，用 `perf top` 或 `perf record` 查看 CPU 时间实际花在哪些函数上，判断是业务计算、序列化、锁竞争、自旋还是系统调用。
>
> 如果涉及调度问题，我会继续用 `perf sched` 看 runnable delay 和上下文切换。

一句话：

> **CPU 高的定位路径是：进程 → 线程 → 函数 → 调度原因。**


## 5. 一个线程为什么一直占 CPU？

### 面试回答

> 首先要区分它是真的在做有效计算，还是在空转。
>
> 如果线程持续占 CPU，我会用 `perf record` 查看热点函数。如果热点集中在业务计算或数据处理，说明是 CPU-bound；如果热点集中在循环检查、原子操作或者反复获取锁失败，则可能是 busy loop 或 spin。
>
> 还需要看线程是否长期处于 runnable/running 状态，以及是否存在优先级或 affinity 导致其他线程竞争 CPU。

可以补充：

> **高 CPU 不等于高吞吐，有时候 CPU 高是因为线程一直在空转。**


## 6. 一个 callback blocking 会影响谁？

这个问题非常容易被问。

### SingleThreadedExecutor

> 如果是 SingleThreadedExecutor，一个 callback blocking，那么这个 Executor 当前线程无法处理其他 callback，因此同一个 Executor 中等待执行的其他 subscription、timer、service 等都会受到影响。

你的实验就是典型例子：

```text
C callback blocking
        ↓
Executor thread occupied
        ↓
A callback waiting
B callback waiting
Timer waiting
```

你之前的实验代码本身就是用 A、B、C 共享 Executor 来观察 callback 排队延迟的。

### MultiThreadedExecutor

> MultiThreadedExecutor 下影响范围取决于线程数量和 Callback Group。
>
> 如果还有空闲 worker，并且 callback group 允许并发，其他 callback 可以继续执行。
>
> 但如果线程全部被 blocking callback 占用，或者 callback 位于 MutuallyExclusive Callback Group 中，那么其他 callback 仍然可能受到阻塞。

一句话：

> **blocking 影响谁，取决于 Executor 线程资源和 Callback Group 的并发约束。**


## 7. DDS receive thread 和 ROS2 Executor 是什么关系？

这是一个非常好的中间件问题。

### 面试回答

> 可以把 DDS receive thread 和 ROS2 Executor 看成两个不同阶段。
>
> DDS receive thread 负责接收网络数据，并将数据交给 DDS 的接收和缓存机制；而 ROS2 Executor 负责检查哪些 subscription、timer 或 service 已经 ready，然后执行对应的 callback。
>
> 所以 DDS 接收到消息，并不等于 ROS2 callback 立即执行，中间还可能经历 DDS 缓存和 Executor 调度。

可以画成：

```text
Network
   ↓
DDS receive thread
   ↓
DDS Reader / History Cache
   ↓
ROS2 Subscription ready
   ↓
Executor
   ↓
Worker Thread
   ↓
Callback
```

因此一个非常重要的判断是：

> **DDS receive 正常 ≠ callback 一定低延迟。**

消息可能已经到了 DDS，但：

```text
Executor busy
        ↓
callback queue
        ↓
等待几十毫秒
        ↓
callback 才开始
```


## 最后：面试中最值得记住的总框架

如果面试官连续问你 ROS2 性能问题，我建议统一使用：

```text
               ROS2 Performance
                      │
        ┌─────────────┼─────────────┐
        │             │             │
       CPU         Executor        DDS
        │             │             │
      perf        Callback       QoS
        │             │             │
     Thread        Queue        Buffer
        │             │             │
        └─────────────┼─────────────┘
                      ↓
                  Latency
```

然后回答任何问题时，都遵循：

> **先定位问题发生在哪一层，再分析资源竞争，最后用工具验证。**

这是比背 ROS2 API 更像“性能优化工程师”的回答方式。

