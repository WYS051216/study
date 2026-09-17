# 📚 C++ 智能指针学习笔记 (Modern C++ Smart Pointers)

## 1. 核心概述 (Overview)

* **头文件**：`#include <memory>`
* **本质**：
    * 智能指针是原始指针（Raw Pointer）的**封装类**（模板类）。
    * 核心机制是 **RAII (资源获取即初始化)**：利用栈对象的生命周期（析构函数）来自动释放堆内存。
* **解决的问题**：
    * 忘记释放 -> 内存泄漏 (Memory Leak)。
    * 提早释放 -> 悬空指针/程序崩溃 (Dangling Pointer)。
* **与 Rust 对比**：
    * 智能指针极大提升了安全性，解决了大部分所有权问题。
    * 但没有从根本上解决所有内存安全问题（例如：通过 `get()` 拿到裸指针后依然可能误操作）。

---

## 2. 独占指针 `unique_ptr` (The Solo Player)

### 📝 核心特性
1.  **独占所有权**：同一时间内，只能有一个 `unique_ptr` 指向该对象。
2.  **不可复制 (No Copy)**：`unique_ptr<T> p2 = p1;` ❌ 报错（拷贝构造函数被删除了）。
3.  **可以移动 (Move Only)**：`unique_ptr<T> p2 = std::move(p1);` ✅ 合法（所有权转移）。
4.  **轻量级**：性能和裸指针几乎一样，没有额外的内存消耗。

### 🛠️ 创建方式
1.  **`make_unique` (推荐)**：`auto p = std::make_unique<int>(10);` (C++14 引入，异常安全)。
2.  **原始指针接管**：`unique_ptr<int> p(new int(10));`。

### 💡 补充知识点 (Key Points)
> * **作为函数参数**：`void func(unique_ptr<int> p)` 意味着**所有权转移**，调用者必须用 `std::move` 传参，调用后原指针失效。
> * **常用 API**：
>     * `p.release()`: 放弃控制权，返回裸指针，**但不释放内存**（需要手动 delete，小心使用）。
>     * `p.reset()`: 释放当前指向的对象，并将指针置空

---

## 3. 共享指针 `shared_ptr` (The Team Player)

### 📝 核心特性
1.  **共享所有权**：多个 `shared_ptr` 可以指向同一块内存。
2.  **引用计数 (Reference Counting)**：
    * 每多一个指针指向它，计数 +1。
    * 每少一个指针（析构或 reset），计数 -1。
    * **计数归 0 时，自动释放内存**。

### 🛠️ 创建方式与内存布局
1.  **`make_shared` (强烈推荐)**：
    * `auto p = make_shared<int>(10);`
    * **优点**：`make_shared` 只分配**一次**内存（对象本身 + 控制块在一起），而 `new` 写法需要分配**两次**内存。`make_shared` 更快且减少内存碎片。
2.  **构造函数**：`shared_ptr<int> p(new int(10));`

### 💡 补充知识点
> * **线程安全性**：引用计数本身是线程安全的（原子操作），但**对对象的读写不是**（仍需加锁）。
> * **自定义删除器**：不仅用于释放内存，还可用于关闭文件句柄、Socket 等。

---

## 4. 弱指针 `weak_ptr` (The Observer)

### 📝 核心特性
1.  **旁观者**：指向 `shared_ptr` 管理的对象，但**不增加引用计数**。
2.  **不能直接使用**：没有重载 `->` 和 `*` 操作符，必须转化为 `shared_ptr` 才能访问数据。

### 🎯 核心作用
1.  **解决循环引用 (Circular Reference)**：打破 `shared_ptr` 互相持有导致的死锁。
2.  **观察资源是否存在**：如果资源还在就用，不在了就算了。

### 💡 常用 API
> * `use_count()`: 查看有多少个 `shared_ptr` 指向该对象。
> * `expired()`: 检查资源是否已经释放（true 表示已死）。
> * `lock()`: **核心函数**。尝试将 `weak_ptr` 升级为 `shared_ptr`。如果对象还在，返回有效的 `shared_ptr`；否则返回 `nullptr`。

---

## 5. 实战代码示例

### 场景一：自定义删除器 (Lambda 写法)

```cpp
#include <iostream>
#include <memory>
using namespace std;

void test_deleter() {
    // 使用 Lambda 表达式定义删除规则
    // 适用于：数组释放、关闭文件、关闭 Socket
    shared_ptr<int> p(new int[10], [](int* p) {
        cout << "【Lambda Delete】释放 int 数组内存" << endl;
        delete[] p;
    });

    cout << "指针正在使用中..." << endl;
} // 退出作用域，自动调用 Lambda
```

### 场景二：解决循环引用 (weak_ptr)

```cpp
#include <iostream>
#include <memory>
using namespace std;

class CB; // 前置声明

class CA {
public:
    shared_ptr<CB> m_ptr_b; // 强引用：我要拥有 B
    ~CA() { cout << "~CA 析构" << endl; }
};

class CB {
public:
    weak_ptr<CA> m_ptr_a;   // <--- 关键点：弱引用！只看着 A，不占有
    ~CB() { cout << "~CB 析构" << endl; }
};

void test_cycle() {
    auto ca = make_shared<CA>();
    auto cb = make_shared<CB>();

    ca->m_ptr_b = cb;
    cb->m_ptr_a = ca; // 此时 CA 的引用计数不会增加，打破死锁

    cout << "CA use count: " << ca.use_count() << endl; // 输出 1
    cout << "CB use count: " << cb.use_count() << endl; // 输出 2
} // 正常析构，无内存泄漏
```
## 6. 总结速查表 (Cheatsheet)

| 指针类型 | 拥有权 | 复制 | 移动 | 引用计数 | 典型场景 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **raw pointer** | 不明确 | ✅ | ✅ | 无 | 底层操作，或作为不涉及所有权的观察者 |
| **unique_ptr** | **独占** | ❌ | ✅ | 无 | 90% 的场景，函数内部变量，工厂模式返回 |
| **shared_ptr** | **共享** | ✅ | ✅ | 有 (原子) | 资源共享，图结构，复杂的生命周期 |
| **weak_ptr** | **无** (观察) | ✅ | ✅ | 不增加 | 解决循环引用，缓存，观察者 |