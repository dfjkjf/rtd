---
layout: default
title: c++
nav_order: 1
---

## C++

### 一、源代码

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

### 二、优化点详解

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

### 三、新代码

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

### 四、使用 noexcept 的时机


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


