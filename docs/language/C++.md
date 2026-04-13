---
layout: default
title: c++
nav_order: 1
---

## C++

高性能C++的关键因素：

1. 算法与数据结构的合理选择
2. 内存访问模式与缓存友好性：确保数据连续存储，优化缓存局部性
3. 避免不必要的动态分配：使用对象池减少分配次数重置而不释放、预分配内存、
4. 并发与并行编程
5. 语言特性优化：避免热点路径上使用虚函数、使用constexpr将计算移至编译期、合理使用模板而非运行时多态、充分利用移动语义避免不必要的复制
6. IO操作优化：合并小IO请求、使用缓冲机制、异步IO实现
7. 编译器优化与内联策略

---

## 一、内存布局与缓存友好性（最重要）

**考察点**：是否理解数据在内存中的真实排布，能否写出缓存友好的代码。

### 典型问题

1. 伪共享（False Sharing）

现代 CPU 并不是按字节读取内存的，而是按 **缓存行（Cache Line）** 读取，通常一行是 **64 字节**。

当两个独立的变量（例如两个线程各自维护的计数器 `A` 和 `B`）恰好落在**同一个缓存行**内时：
1.  线程 1 修改变量 `A`。
2.  为了保证缓存一致性，硬件会使线程 2 所在核心的该缓存行**失效**。
3.  线程 2 即使只修改变量 `B`，也必须重新从内存加载这一整行。

这种由于“邻居”修改数据导致自己缓存频繁失效的现象，就叫**伪共享**。它没有逻辑错误，但会产生巨大的性能开销。

如何解决？

利用 `alignas` 将变量拉开距离，确保它们不在同一个缓存行内。

```cpp
struct KeepApart {
    alignas(64) std::atomic<int> thread1_counter; // 强制占据新的缓存行
    alignas(64) std::atomic<int> thread2_counter; // 避免与上一个变量挤在一起
};
```

| 术语 | 作用 | 关注点 |
| :--- | :--- | :--- |
| **`alignof`** | 查询对齐要求 | 获取类型的对齐规则 `alignof(char); // 通常返回 1` |
| **`alignas`** | 强制对齐 | 改变变量在内存中的排列方式。 |
| **伪共享** | 现象 | 多个变量因挤在同一 Cache Line 导致的并发性能下降。 |


2. **遍历二维数组**  
   ```cpp
   // 哪个快？为什么？
   for (int i=0; i<N; i++) 
       for (int j=0; j<N; j++) sum += a[i][j];
   
   for (int j=0; j<N; j++) 
       for (int i=0; i<N; i++) sum += a[i][j];
   ```
   - 期望：第一个快（行优先），能解释缓存行、预取、miss rate。

3. std::unique_ptr 与 std::shared_ptr 的实现原理及开销

| 特性 | std::unique_ptr | std::shared_ptr |
| :--- | :--- | :--- |
| **所有权** | 独占 (Exclusive) | 共享 (Shared) |
| **内存大小** | 等同于原始指针 (8B) | 2倍原始指针大小 (16B) |
| **控制块** | 无 | 有 (在堆上分配) |
| **原子操作** | 无 | 有 (增减计数时) |
| **性能** | 几乎零开销 | 较明显的运行时开销 |
| **拷贝/移动** | 仅支持移动 | 支持拷贝和移动 |

`shared_ptr` 的内部结构比 `unique_ptr` 复杂得多，它包含两个指针：
1.  **指向被管理对象的指针**。
2.  **指向控制块（Control Block）的指针**。

**控制块**是一个动态分配的对象，包含：
- **强引用计数 (Strong Reference Count)**：当前持有该对象的 `shared_ptr` 数量。
- **弱引用计数 (Weak Reference Count)**：指向该对象的 `weak_ptr` 数量。

- 空间开销：
    - 指针本身：占用 **16 字节**（2 个指针：对象指针 + 控制块指针）。
    - 内存分配：控制块通常是在堆上动态分配的。如果使用 `new` 创建对象再传给 `shared_ptr`，会产生**两次**内存分配（一次对象，一次控制块）。
    - *优化*：使用 `std::make_shared` 可以将对象和控制块合并到一块连续内存中，减少一次分配开销。
- 时间开销：
    - 原子操作：为了保证线程安全，引用计数的增减必须使用**原子操作 (Atomic Operations)**。原子操作比普通的自增操作慢得多，特别是在高并发环境下。
    - 间接访问：访问对象需要经过一层控制块的检查（虽然主要成本在原子计数上）。

---

## 二、现代C++特性与零开销抽象

**考察点**：是否真正理解 C++11/14/17/20 的特性是如何映射到汇编层面的。

### 典型问题

1. 右值引用 (Rvalue Reference)

在理解右值引用前，先看这两个概念：
* **左值 (lvalue)**：有名字、可以取地址的对象（比如变量名）。
* **右值 (rvalue)**：临时对象、字面量、即将销毁的值（比如 `10` 或 `a + b` 的结果）。

**右值引用**（符号是 `&&`）专门用来绑定到这些“临时对象”上。它通过“续命”的方式，让你能够直接利用临时对象的资源。

```cpp
int a = 10;
int& lref = a;       // 左值引用，绑定到 a
int&& rref = 10;      // 右值引用，绑定到临时值 10
```

2. std::move：强转为右值

`std::move` 其实**并不移动任何东西**。它的本质是一个**强制类型转换**。

* **作用**：告诉编译器：“这个左值变量我以后不用了，你可以把它看作一个右值。”
* **结果**：转换后，编译器可以匹配到接收右值参数的函数（比如**移动构造函数**），从而直接“接管”原有的内存资源，而不是重新拷贝一份。

**代码示例：**
```cpp
std::string str1 = "Hello";
std::string str2 = std::move(str1); 
// 此时 str1 变为空（被掏空了），str2 拿到了 "Hello" 的内存地址。
// 整个过程没有发生字符串拷贝，只有指针交换。
```

3. std::forward：完美转发

`std::forward` 通常只出现在**模板函数**中，配合“万能引用”（Universal Reference）使用。

* **问题所在**：在函数模板中，即便你传入的是个右值，在函数内部该参数本身也会变成一个“有名字的左值”。如果你再把它传给下一个函数，它就失去“右值”的身份了。
* **作用**：根据传入参数的原始类型，将其原封不动地转发给下一个函数。如果原来是左值就转发为左值，原来是右值就转发为右值。

**代码示例：**
```cpp
template<typename T>
void wrapper(T&& arg) {
    // 这里的 arg 是万能引用
    foo(std::forward<T>(arg)); // 完美转发：保持 arg 的原始左右值属性
}
```

总结与对比

| 工具 | 核心本质 | 主要用途 |
| :--- | :--- | :--- |
| **右值引用 (`&&`)** | 一种类型声明 | 允许我们识别并操作临时对象。 |
| **`std::move`** | 强制类型转换 | 将左值标记为右值，以触发**移动语义**（减少拷贝）。 |
| **`std::forward`** | 条件类型转换 | 在模板中实现**完美转发**，保留参数的原始属性。 |

形象理解
* **拷贝**：我要你的房子，于是我在旁边盖了一座一模一样的，家具全重买。
* **移动 (`std::move`)**：我要你的房子，你直接把钥匙给我，你搬出去，我直接住进去。
* **转发 (`std::forward`)**：我帮别人带话，他说什么我就传什么，不加任何改动。


**下面代码会发生几次拷贝/移动？**
   ```cpp
   std::vector<int> create() {
       std::vector<int> v{1,2,3};
       return v;   // 注意：不是 std::move(v)
   }
   auto v2 = create();
   ```
   - 虽然从语法上看，v 是一个局部变量，返回时似乎应该发生移动或拷贝，但实际上编译器执行了NRVO (Named Return Value Optimization)：编译器直接在 v2 的内存地址上构造 v。
   - 千万别加 std::move：std::move 会将变量转换为右值引用。一旦你显式写了 move，编译器就无法实施 NRVO（返回值优化）了。**这反而会导致性能下降！**

4. **实现一个简单的 `String` 类**  
   - 要求：正确的拷贝构造、拷贝赋值、移动构造、移动赋值、析构。
   - 考察：资源管理、异常安全、自赋值处理。

源代码
```cpp
#include <iostream>

class CharBuffer
{
public:
    CharBuffer():m_buff(nullptr),m_Size(0){
        std::cout<<"默认构造函数"<<std::endl;
    }

    CharBuffer(std::size_t nSize):m_buff(new char[nSize]),m_Size(nSize){
        std::cout<<"普通构造函数"<<std::endl;
    }

    CharBuffer(const CharBuffer& other):m_buff(new char[m_Size]),m_Size(other.m_Size)
    {
        memcpy(m_buff,other.m_buff,m_Size);
        std::cout<<"拷贝构造函数"<<std::endl;
    }

    CharBuffer& operator=(const CharBuffer& other){
        if(this==&other)
            return *this;

        m_Size = other.m_Size;
        delete[] m_buff;
        m_buff = new char[m_Size];
        memcpy(m_buff,other.m_buff,m_Size);
        std::cout<<"拷贝赋值运算符"<<std::endl;
        return *this;
    }

    CharBuffer(CharBuffer&& other):m_Size(other.m_Size),m_buff(other.m_buff)
    {
        other.m_buff = nullptr;
        other.m_Size = 0;
        std::cout<<"移动构造函数"<<std::endl;
    }

    CharBuffer& operator=(CharBuffer&& other){
        if(this==&other)
            return *this;

        delete[] m_buff;
        m_Size = other.m_Size;
        m_buff = other.m_buff;

        other.m_Size = 0;
        other.m_buff = nullptr;
        std::cout<<"移动赋值运算符"<<std::endl;
        return *this;
    }

    ~CharBuffer(){ 
        delete[] m_buff; 
        std::cout<<"析构函数"<<std::endl;
    }
private:
    char* m_buff;
    std::size_t m_Size;
};
```

优化点详解

​- ​内存管理​​
   - 用 std::unique_ptr<char[]> 替代原始指针 char*，自动处理内存释放，避免内存泄漏。
   - 析构函数无需手动 delete[]（RAII 原则）。

​- ​异常安全​​
   - 所有不抛异常的操作（如移动构造、移动赋值）标记为 noexcept，提升与 STL 容器的兼容性。
   - 使用 std::make_unique 替代 new，避免内存分配失败时的资源泄漏。

​- ​API 改进​​
   - 单参数构造函数声明为 explicit，防止隐式转换。
   - 默认构造函数标记为 = default，明确意图。

​- ​性能优化​​
   - 移动操作通过 std::exchange 原子性地“读取旧值 + 写入新值”，高效转移资源所有权。
   - 使用 std::copy 替代 memcpy，类型更安全。

新代码

```cpp
#include <iostream>
#include <cstddef>   // 包含 std::size_t 的定义
#include <memory>    // 使用 std::unique_ptr 管理内存
#include <utility>   // 使用 std::exchange
#include <algorithm> // 使用 std::copy

class CharBuffer {
public:
    // 默认构造函数（初始化空缓冲区）
    CharBuffer() noexcept = default;

    // 普通构造函数（分配指定大小的缓冲区）
    explicit CharBuffer(std::size_t size) 
        : m_buff(std::make_unique<char[]>(size)), m_size(size) {
        std::cout << "普通构造函数\n";
    }

    // 拷贝构造函数
    CharBuffer(const CharBuffer& other) 
        : m_buff(std::make_unique<char[]>(other.m_size)), m_size(other.m_size) {
        std::copy(other.m_buff.get(), other.m_buff.get() + m_size, m_buff.get());
        std::cout << "拷贝构造函数\n";
    }

    // 拷贝赋值运算符
    CharBuffer& operator=(CharBuffer &&other) {
        if (this != &other) {
            auto new_buff = std::make_unique<char[]>(other.m_size);
            std::copy(other.m_buff.get(), other.m_buff.get() + other.m_size, new_buff.get());
            m_buff = std::move(new_buff);
            m_size = other.m_size;
        }
        std::cout << "拷贝赋值运算符\n";
        return *this;
    }

    // 移动构造函数（使用 std::exchange 原子操作）
    // （标记为 noexcept 以支持 STL 容器优化）
    CharBuffer(CharBuffer&& other) noexcept 
        : m_buff(std::exchange(other.m_buff, nullptr)),
          m_size(std::exchange(other.m_size, 0)) {
        std::cout << "移动构造函数\n";
    }

    // 移动赋值运算符
    CharBuffer& operator=(CharBuffer&& other) noexcept {
        if (this != &other) {
            m_buff = std::exchange(other.m_buff, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        std::cout << "移动赋值运算符\n";
        return *this;
    }

    // 析构函数（无需手动释放，unique_ptr 自动处理）
    ~CharBuffer() = default;

private:
    std::unique_ptr<char[]> m_buff; // 使用智能指针管理内存
    std::size_t m_size = 0;        // 缓冲区大小
};
```

使用 noexcept 的时机

| 操作类型               | 是否标记 `noexcept` | 原因                           |
|------------------------|---------------------|-----------------------------|
| ​**​移动构造/移动赋值​**​  | ✅ 必须              | 标准库优化依赖，且实现通常不抛异常  |
| ​**​析构函数​**​           | ✅ 隐式默认          | 析构函数默认不应抛异常              |
| ​**​`swap` 函数​**​        | ✅ 推荐              | 通常只交换指针或基本类型           |
| ​**​内存分配/IO 操作​**​   | ❌ 禁止              | 可能抛 `std::bad_alloc` 或系统错误 |
| ​**​复杂计算​**​           | ❌ 禁止              | 可能因数值错误抛异常（如除以零）     |

1. ​**​移动操作​**​：若未标记 `noexcept`，STL 容器（如 `std::vector`）会降级为拷贝操作，导致性能损失。
2. ​**​析构函数​**​：C++ 标准规定析构函数默认 `noexcept`，手动抛出异常会触发 `std::terminate`。
3. ​**​`swap`​**​：推荐与移动操作协同实现，例如 `Copy-and-Swap` 惯用法。

将计算从“运行时”转移到“编译时”是 C++ 性能优化的最高境界之一。其核心思想是：**既然结果在编译阶段就能确定，就不要让 CPU 在程序运行时重复计算。**

以下是实现这一目标的主要技术手段：

---

5. 如何将运行时计算转移到编译时，从而减少运行时开销？

`constexpr`：常数表达式

`constexpr` 告诉编译器，这个函数或变量在给定常量输入时，可以在编译期直接算出结果。(对变量必须在编译期确定，对函数是建议)

* **变量**：`constexpr int max_val = 10 * 5;` 保证该值被存放在只读内存或直接嵌入指令。
* **函数**：
    ```cpp
    constexpr int fibonacci(int n) {
        return (n <= 1) ? n : fibonacci(n - 1) + fibonacci(n - 2);
    }
    // 编译后，val 直接变成了 55，没有任何递归调用开销
    constexpr int val = fibonacci(10); 
    ```

模板元编程 (TMP)

在 `constexpr` 普及之前，模板是编译期计算的唯一方式。它利用模板实例化的递归特性来执行逻辑。

* **优点**：可以处理更复杂的类型生成。
* **示例（计算阶乘）**：
    ```cpp
    template<int N>
    struct Factorial {
        static constexpr int value = N * Factorial<N - 1>::value;
    };

    template<>
    struct Factorial<0> {
        static constexpr int value = 1;
    };

    int main() {
        int x = Factorial<5>::value; // 编译时已替换为 120
    }
    ```

`static_assert`：编译期校验

虽然它不直接“计算”数值，但它将逻辑校验从运行时提前到了编译期。如果某些条件不满足（如硬件不支持 64 位），程序根本无法编译通过。

```cpp
static_assert(sizeof(void*) == 8, "64-bit system required!");
```

`consteval` (C++20)

`constexpr` 函数依然可以在运行时被调用，但 `consteval` 声明的是 **立即函数 (Immediate Function)**。它**强制**要求该函数必须在编译期求值，否则报错。

```cpp
consteval int square(int n) { return n * n; }
int x = 10;
// int s = square(x); // 错误！x 是运行时的，无法在编译期确定
```

---

## 三、多线程与无锁编程

**考察点**：内存模型、原子操作、避免死锁的能力。

### 1. 解释 `std::atomic` 与 `volatile` 的区别
   - 期望：`volatile` 不提供原子性，只防止编译器优化；`atomic` 保证原子性和内存顺序。
   - 通常情况下，编译器为了加速会把变量缓存在寄存器里。`volatile` 强制要求每次读写都必须直接访问**内存地址**。
   - 现代 CPU 和编译器为了性能，会乱序执行指令。`std::memory_order` 决定了这种重排的边界。

常见的内存顺序模式：
1.  **`memory_order_relaxed` (松散顺序)**：
    * 只保证原子性，不保证任何指令重排的约束。
    * *场景*：简单的计数器（如统计点击量），不涉及线程间的逻辑依赖。
2.  **`memory_order_acquire` / `release` (获取/释放语义)**：
    * **Release**：确保在此之前的写操作不会被重排到此之后。
    * **Acquire**：确保在此之后的读操作不会被重排到此之前。
    * *场景*：实现互斥锁或线程间的“信号”传递。
3.  **`memory_order_seq_cst` (顺序一致性)**：
    * **默认模式**。最严格，保证所有线程看到的执行顺序完全一致。
    * *代价*：性能开销最大。

总结对比

| 特性 | `std::atomic` | `volatile` | `std::memory_order` |
| :--- | :--- | :--- | :--- |
| **解决的问题** | 数据竞争、原子性 | 变量被意外修改（硬件/信号） | 指令重排、内存可见性 |
| **多线程安全** | **是** | **否** | **是**（配合 atomic 使用） |
| **编译器优化** | 禁止对原子操作重排 | 禁止将变量缓存到寄存器 | 约束编译器/CPU 重排指令 |
| **典型应用** | 共享计数器、无锁队列 | 嵌入式硬件寄存器映射 | 高性能并发库（如 Boost） |

形象理解
* **`std::atomic`**：给操作加了一个“不可拆分”的保险箱。
* **`volatile`**：给编译器贴了个纸条：“这个数随时会变，别想偷懒存寄存器，去内存里读！”
* **`std::memory_order`**：交警指挥交通，规定哪些车（指令）可以超车，哪些必须按顺序排队。

### 2. 锁的优化

1. 普通锁 (Mutex / Mutex Lock)

2. 读写锁 (Read-Write Lock)

3. 自旋锁 (Spinlock)
当获取不到锁时，线程不挂起，而是在一个循环里**不停地尝试**（占用 CPU）。
* **实现原理**：利用 CPU 的原子操作（如 CAS）不断轮询。
* **使用时机**：
    * 临界区代码**极其短小**（例如只改一个变量）。
    * 锁竞争不激烈。
    * **优势**：避免了线程上下文切换（Context Switch）的昂贵开销。
    * **警告**：如果持有锁的线程长时间不释放，自旋锁会白白浪费大量 CPU 资源。

4. 可重入锁 (Recursive Lock / Reentrant Lock)
允许同一个线程多次获取同一个锁而不被阻塞。
* **实现原理**：内部维护一个计数器和持有者 ID。
* **使用时机**：
    * **递归调用**中涉及加锁逻辑。
    * 同一个类中的多个同步方法互相调用。
    * 避免在同一个线程中产生“死锁”。

5. 乐观锁 (Optimistic Locking / CAS)
严格来说，这是一种并发策略而非物理锁。
* **实现原理**：不加锁，直接操作。在更新时检查数据是否被改动（通常通过版本号或 `std::atomic` 的 `compare_exchange`）。
* **使用时机**：
    * **冲突极少**发生。
    * 需要极高的响应速度。
    * 不希望线程被挂起。

6. 死锁预防锁 (Timed Lock / Try Lock)
支持超时机制的锁（如 `std::unique_lock` 配合 `std::timed_mutex`）。
* **使用时机**：
    * 对实时性要求高，不能接受无限期等待。
    * 为了检测或预防死锁，尝试获取锁，失败则退回执行其他逻辑。

总结：如何选型？

| 场景需求 | 推荐锁类型 |
| :--- | :--- |
| **标准并发，执行逻辑稍长** | **普通锁 (Mutex)** |
| **执行极快（几个指令），不想切换内核态** | **自旋锁 (Spinlock)** |
| **大部分时间在读，偶尔更新** | **读写锁 (RW Lock)** |
| **复杂的嵌套调用、递归** | **可重入锁 (Recursive Lock)** |
| **高并发、极低冲突、追求极致吞吐** | **乐观锁 (CAS)** |

💡 一个性能小贴士
在现代 C++ 中，如果你的临界区只是简单的数值加减，**优先使用 `std::atomic`**。它的底层通常会根据硬件自动选择最快的自旋或指令级原子操作，比手动加锁要快得多。


2. 实现一个无锁队列
   - 不要求完全写对，但考察思路：CAS、ABA问题（`std::atomic<std::shared_ptr<T>>`或带tag的指针）。

### 3. 解决ABA问题用哪个锁？ 
   - ABA 问题是“无锁编程”引入的复杂性。如果你的应用场景没有遇到严重的锁竞争瓶颈，普通互斥锁依然是防止 ABA 问题最稳妥的选择。  

### 4. **`std::shared_ptr` 线程安全吗？**  
   - 期望：引用计数是原子的，但指向的对象**不是**线程安全的。

---

## 四、数据结构与缓存亲和性

**考察点**：高性能不仅仅是算法复杂度（ O(N) ），更是常数项的优化。

### 1. 容器底层原理：
- 考察点：std::vector 的扩容策略，std::unordered_map 的哈希冲突解决。
    - 经历“重新申请、搬迁、释放”的过程

### 2. 分支预测
1. 算法级优化：消除分支
    - 查表法 (Lookup Table): 将逻辑判断转换为索引访问。
    - 使用逻辑计算代替分支: 利用位运算或算术运算代替 if。
    - 合并判定条件: 将多个嵌套的 if 合并为一个复杂的逻辑表达式，减少分支指令的数量。
2. 数据重构：让数据更有序
    - 数据排序: 这是最经典的案例。对数组进行排序后，if (arr[i] < threshold) 的判断从“随机抽奖”变成了“一串 False 后跟一串 True”，预测准确率接近 100%。
    - 数据对齐与分区: 将数据分为“必须处理”和“无需处理”两组，通过指针或索引跳转，而不是在循环内部判断。
---

## 五、性能分析与优化实战

**考察点**：会用什么工具、如何定位瓶颈。

### 典型问题
1. **你的程序CPU占用高，如何定位热点？**  
   - 期望：perf、Intel VTune、火焰图、`std::chrono` 打点。

2. **内存占用持续增长，如何排查？**  
   - 期望：valgrind、AddressSanitizer、heapprofile。

3. **伪代码优化**  
   给出一个明显低效的循环（如频繁`push_back`未预分配），让候选人指出问题并优化。

---

## 六、异常安全与RAII

**考察点**：资源管理的严谨性。

### 典型问题
1. **RAII 如何管理锁、文件、内存？**  
   - 期望：`std::lock_guard`、`std::unique_ptr`、`fstream` 析构释放。

2. **实现一个强异常安全的 `vector::push_back`**  
   - 考察：拷贝/移动时抛出异常如何回滚。

3. **析构函数能不能抛出异常？为什么？**  
   - 期望：不能，否则可能导致`std::terminate`。

---

## 实战面试设计（1小时）

| 阶段 | 内容 | 时间 |
|------|------|------|
| 热身 | 解释 `virtual` 表、`sizeof` 空类 | 5min |
| 核心 | 内存布局题（结构体对齐 + 缓存行） | 10min |
| 深入 | 实现简易 `String` 类（移动语义） | 15min |
| 多线程 | 无锁队列思路 + `std::atomic` | 10min |
| 优化 | 分析一段低效代码，提出改进 | 10min |
| 总结 | 用过哪些性能工具？如何做profiling？ | 10min |

---

