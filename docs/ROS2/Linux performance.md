---
layout: default
title: Linux Performance
nav_order: 3
---

# Linux Performance

## CPU 性能定位实验

> 制造 CPU 高 → 判断是哪个进程 → 判断是哪个线程 → 定位线程在执行什么 → 得出结论。

```text
CPU 高
 ↓
top
 ↓
Process PID
 ↓
top -H
 ↓
High CPU TID
 ↓
pidstat -t -p 12345 1
 ↓
持续？
周期？
Spike？
 ↓
sudo perf top -p 12345      # sudo perf top -t 12346
 ↓
实时热点
 ↓
sudo perf record -p 12345 -g -- sleep 10     # sudo perf record -t 12346 -g -- sleep 10
 ↓
sudo perf report
 ↓
Function / Call Stack
 ↓
Root Cause
```

## 内存增长定位实验

```text
内存异常
↓
Process
↓
RSS / VSZ
↓
增长趋势
↓
内存来源
↓
valgrind --leak-check=full --show-leak-kinds=all ./leak_lab
↓

==290078== 
==290078== HEAP SUMMARY:
==290078==     in use at exit: 1,048,576,000 bytes in 100 blocks
==290078==   total heap usage: 102 allocs, 2 frees, 1,048,649,728 bytes allocated
==290078== 
==290078== 209,715,200 bytes in 20 blocks are possibly lost in loss record 1 of 2
==290078==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==290078==    by 0x109228: process_message() (memory_lab.cpp:11)
==290078==    by 0x109262: main (memory_lab.cpp:23)
==290078== 
==290078== 838,860,800 bytes in 80 blocks are definitely lost in loss record 2 of 2
==290078==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==290078==    by 0x109228: process_message() (memory_lab.cpp:11)
==290078==    by 0x109262: main (memory_lab.cpp:23)
==290078== 
==290078== LEAK SUMMARY:
==290078==    definitely lost: 838,860,800 bytes in 80 blocks
==290078==    indirectly lost: 0 bytes in 0 blocks
==290078==      possibly lost: 209,715,200 bytes in 20 blocks
==290078==    still reachable: 0 bytes in 0 blocks
==290078==         suppressed: 0 bytes in 0 blocks
==290078== 
==290078== For lists of detected and suppressed errors, rerun with: -s
==290078== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)

↓
Heap / mmap / Stack
↓
Allocation
↓
Root Cause
```

大部分不是 Leak，而是消息堆积

## Thread / Scheduler

表面上 callback 的执行时间：

```text
Callback start
      ↓
执行计算
      ↓
Callback end
```
但实际上：
```text
Callback Start
      ↓
等待 Scheduler
      40 ms
      ↓
真正运行
      1 ms
      ↓
Callback End
```

查看等待 Scheduler 时间
```bash
taskset -c 0 ./scheduler_lab
sudo perf sched record -p $(pidof -s scheduler_lab) -g -- sleep 10
sudo perf sched latency    # 查看调度延迟
sudo perf sched timehist   # 
```

```bash
$ sudo perf sched latency -p

 -------------------------------------------------------------------------------------------------------------------------------------------
  Task                  |   Runtime ms  | Switches | Avg delay ms    | Max delay ms    | Max delay start           | Max delay end          |
 -------------------------------------------------------------------------------------------------------------------------------------------
  :238022:238022        |      0.000 ms |        1 | avg:   6.075 ms | max:   6.075 ms | max start: 20979.414893 s | max end: 20979.420969 s
  main:322704           |      0.045 ms |        2 | avg:   5.051 ms | max:   6.659 ms | max start: 20984.013464 s | max end: 20984.020123 s
  main:322707           |      4.611 ms |       44 | avg:   3.000 ms | max:  10.304 ms | max start: 20986.923416 s | max end: 20986.933720 s
  main:322710           |     67.132 ms |     1352 | avg:   2.295 ms | max: 100.515 ms | max start: 20986.820121 s | max end: 20986.920635 s
  main:322706           |      0.301 ms |        9 | avg:   0.812 ms | max:   1.546 ms | max start: 20983.018813 s | max end: 20983.020359 s
  main:322708           |      0.653 ms |        4 | avg:   0.083 ms | max:   0.147 ms | max start: 20981.526128 s | max end: 20981.526275 s
  cb_calc:322716        |   2058.319 ms |      161 | avg:   0.012 ms | max:   0.045 ms | max start: 20982.507891 s | max end: 20982.507936 s
  cb_A:322714           |    248.009 ms |      800 | avg:   0.008 ms | max:   0.039 ms | max start: 20984.208869 s | max end: 20984.208907 s
  cb_B:322715           |   1018.193 ms |      400 | avg:   0.007 ms | max:   0.019 ms | max start: 20987.610188 s | max end: 20987.610207 s
  :131:131              |      0.000 ms |        1 | avg:   0.005 ms | max:   0.005 ms | max start: 20980.516516 s | max end: 20980.516521 s
 -----------------------------------------------------------------------------------------------------------------
  TOTAL:                |   3397.265 ms |     2774 |
 ---------------------------------------------------
  INFO: 13.468% context switch bugs (193 out of 1433)


$ sudo perf sched timehist -g
           time    cpu  task name                       wait time  sch delay   run time
                        [tid/pid]                          (msec)     (msec)     (msec)
--------------- ------  ------------------------------  ---------  ---------  ---------
   20979.257962 [0007]  main[322710/322703]                 0.000      0.000      0.000    syscall_exit_to_user_mode <- do_syscall_64 <- entry_SYSCALL_64_after_hwframe <- [unknown]
   20979.257973 [0007]  cb_A[322714/322703]                 0.000      0.016      0.010    futex_wait_queue <- __futex_wait <- futex_wait <- do_futex <- __x64_sys_futex
   20979.257981 [0007]  main[322710/322703]                 0.010      0.000      0.007    syscall_exit_to_user_mode <- do_syscall_64 <- entry_SYSCALL_64_after_hwframe <- [unknown] <- [unknown]
   20979.257999 [0007]  cb_A[322714/322703]                 0.007      0.004      0.017    futex_wait_queue <- __futex_wait <- futex_wait <- do_futex <- __x64_sys_futex
   20979.258005 [0007]  main[322710/322703]                 0.017      0.000      0.006    syscall_exit_to_user_mode <- do_syscall_64 <- entry_SYSCALL_64_after_hwframe <- [unknown] <- [unknown]
   20979.258014 [0007]  cb_A[322714/322703]                 0.006      0.003      0.008    futex_wait_queue <- __futex_wait <- futex_wait <- do_futex <- __x64_sys_futex
   20979.258021 [0007]  main[322710/322703]                 0.008      0.000      0.007    syscall_exit_to_user_mode <- do_syscall_64 <- entry_SYSCALL_64_after_hwframe <- [unknown] <- [unknown]
   20979.259271 [0007]  cb_A[322714/322703]                 0.007      0.003      1.250    futex_wait_queue <- __futex_wait <- futex_wait <- do_futex <- __x64_sys_futex
   20979.259318 [0007]  main[322710/322703]                 1.250      0.000      0.046    futex_wait_queue <- __futex_wait <- futex_wait <- do_futex <- __x64_sys_futex
   20979.261964 [0007]  main[322709/322703]                 0.000      0.000      2.646    schedule_timeout <- __skb_wait_for_more_packets <- __skb_recv_udp <- udp_recvmsg <- inet_recvmsg
   20979.261991 [0007]  main[322708/322703]                 0.000      0.000      0.026    schedule_timeout <- __skb_wait_for_more_packets <- __skb_recv_udp <- udp_recvmsg <- inet_recvmsg
   20979.262175 [0007]  main[322708/322703]                 0.000      0.000      0.183    schedule_timeout <- __skb_wait_for_more_packets <- __skb_recv_udp <- udp_recvmsg <- inet_recvmsg
   20979.307971 [0007]  cb_calc[322716/322703]              0.000      0.000     45.796    futex_wait_queue <- __futex_wait <- futex_wait <- do_futex <- __x64_sys_futex
   20979.307999 [0007]  main[322710/322703]                48.653      0.000      0.028    futex_wait_queue <- __futex_wait <- futex_wait <- do_futex <- __x64_sys_futex
   20979.308662 [0007]  cb_calc[322716/322703]              0.028      0.012      0.663    futex_wait_queue <- __futex_wait <- futex_wait <- do_futex <- __x64_sys_futex
   20979.308874 [0007]  main[322710/322703]                 0.663      0.000      0.211    syscall_exit_to_user_mode <- do_syscall_64 <- entry_SYSCALL_64_after_hwframe <- [unknown]
   20979.308890 [0007]  cb_A[322714/322703]                49.602      0.012      0.015    futex_wait_queue <- __futex_wait <- futex_wait <- do_futex <- __x64_sys_futex
   20979.308908 [0007]  main[322710/322703]                 0.015      0.000      0.018    syscall_exit_to_user_mode <- do_syscall_64 <- entry_SYSCALL_64_after_hwframe <- [unknown] <- [unknown]
```

解读

从 perf sched latency 看，cb_A、cb_B 和 cb_calc 的平均调度延迟只有 7～12 微秒，最大也只有几十微秒，说明线程在进入 runnable 状态以后基本能够立即获得 CPU。因此之前 perf sched timehist 中看到的几十毫秒 wait time 不能直接理解为 CPU 调度抖动。

结合调用栈中的 futex_wait_queue，我判断这些 wait time 主要是线程在 futex 上等待，也就是 Executor 或线程同步造成的阻塞。真正需要重点关注的是 callback 从消息到达，到 Executor 开始执行之间的等待，而不是 Linux Scheduler 的 runqueue delay。

所以当前证据表明：问题更可能在 Executor/ROS2/DDS 的等待和唤醒链路，而不是 CPU 绑核后的 Scheduler 性能。


## “为什么线程多了以后不一定更快？”

面试中可以这样回答，控制在 **30 秒左右**：

> **线程多不一定更快，因为性能最终受 CPU 核数和任务特性限制。**
>
> 在线程数量小于可用 CPU 核数，并且任务之间可以真正并行时，增加线程可以提高吞吐量。
>
> 但当线程数超过 CPU 的实际处理能力后，多个线程会竞争 CPU，产生更多的上下文切换。同时如果线程之间存在锁、共享数据或同步关系，还会出现锁竞争和等待。此外，频繁的线程切换和 CPU 迁移也可能影响 Cache 命中率。
>
> **所以线程数需要根据 CPU 核数、任务 CPU 占用、任务是否阻塞以及锁竞争情况来确定，而不是越多越好。**

如果面试官继续问：

#### “那你实际怎么判断线程是不是太多？”

可以回答：

> 我会先看 CPU 利用率，然后通过 `top -H` 或 `pidstat -t` 看线程级 CPU 使用情况，再用 `perf sched` 分析线程的 wait time 和 scheduler delay。如果发现 runnable 线程很多、上下文切换频繁，或者线程大部分时间在锁和 futex 上等待，就说明增加线程可能已经没有收益，甚至会增加抖动。

这个回答比较符合你现在做的 **ROS2 Executor + 绑核 + perf sched** 实验，因为你不仅是在说理论，而是真的可以用实验验证。

---


## strace / I/O

这一天的目标是建立一条新的性能分析因果链：

```text
Application Latency
        ↓
线程为什么慢？
        ↓
是否卡在 syscall？
        ↓
是否卡在 I/O？
        ↓
CPU / Memory / Disk 哪个资源导致？
```

你今天要回答一个问题：

> **如果 ROS2 callback 的 latency 变大，怎么判断它是因为 file I/O、blocking syscall，还是 CPU 调度造成的？**

我们制造：

```text
Callback
   │
   ├── 正常处理
   │
   └── file I/O
           │
           ▼
         write()
           │
           ▼
         fsync()
           │
           ▼
        blocking
           │
           ▼
    callback latency ↑
```


```bash
sudo strace -c -f -p <PID> # 直接统计最慢的 syscall
```

它会输出类似：

```text
% time     seconds  usecs/call     calls syscall
------ ----------- ----------- --------- -------
 60.21    1.203421        8022       150 fsync
 20.13    0.402123          35     11000 write
 10.45    0.208123         120      1700 futex
```


同时开另一个终端：

```bash
iostat -xz 1
```

重点看：

```text
await = 30 ms # I/O request 平均延迟
%util = 95%   # 磁盘忙碌程度
```

说明：

> Disk I/O 已经可能成为瓶颈。


用 vmstat 看系统整体

运行：

```bash
vmstat 1
```

重点看：

```text
r
b
si
so
bi
bo
cs
```

含义：

```text
r   runnable tasks
b   blocked tasks

si  swap in
so  swap out

bi  blocks received
bo  blocks sent

cs  context switches
```

面试时可以说：

> 当发现应用延迟增加时，我不会直接判断是 CPU 问题。我会先使用 `strace -T` 查看线程是否长时间停留在某个 syscall，例如 `read`、`write`、`fsync` 或 `futex`。如果发现 I/O syscall 耗时异常，再使用 `iostat` 查看磁盘的 I/O 延迟和利用率，并结合 `vmstat` 的 blocked task、I/O 和上下文切换指标判断系统资源状态。

---

## Linux 故障实验

每个故障场景都可以填这张表：

```text
故障场景：
制造方法：
观察时间：

现象：
- 系统表现：
- 业务表现：

指标：
- CPU：
- 内存：
- IO：
- 负载：
- 线程状态：

工具：
- 使用命令：
- 使用工具：

证据：
- 关键输出：
- 异常进程 PID：
- 异常线程 TID：
- 异常文件/磁盘：

根因：
- 直接原因：
- 深层原因：

修复/验证：
- 临时处理：
- 长期修复：
- 验证结果：
```

