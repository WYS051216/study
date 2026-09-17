# C++17 现代特性笔记：Lambda 进阶与新增属性

---

## 一、 Lambda 表达式捕获 `*this` (按值捕获当前对象)

### 1. 痛点与解决
在 C++17 之前，当我们在类的成员函数中写 Lambda 表达式并捕获 `[this]` 时，实际上捕获的是 **当前对象的指针**。
如果在多线程或异步编程中，这个 Lambda 被放到了另一个线程执行，而原对象在主线程中已经被销毁了，此时 Lambda 内部再去访问 `this` 指针指向的成员变量，就会导致**悬空指针错误（未定义行为，程序崩溃）**。

C++17 引入了 `[*this]` 捕获方式。它会将当前对象的 **完整副本（按值拷贝）** 传递进 Lambda 中。这样 Lambda 就拥有了自己独立的对象数据，哪怕原对象销毁了，Lambda 依然可以安全运行。由于是副本，默认情况下它是只读的（除非加 `mutable`）。

### 2. 代码示例
```cpp
#include <iostream>
#include <thread>
#include <chrono>

class Task {
public:
    int value = 100;

    void executeAsync() {
        // C++17 之前的 [this] 捕获：极其危险！如果 Task 对象被销毁，这里会崩溃
        // auto dangerous_lambda = [this]() { std::cout << value << std::endl; };

        // C++17 的 [*this] 捕获：安全！它拷贝了整个 Task 对象
        auto safe_lambda = [*this]() {
            // 这里的 value 是拷贝过来的副本的 value
            std::cout << "异步任务执行中，捕获的副本值为: " << value << std::endl;
        };

        std::thread t(safe_lambda);
        t.detach(); // 分离线程，让它在后台跑
    }
};

int main() {
    {
        Task myTask;
        myTask.executeAsync();
    } // 离开这个作用域，myTask 对象立刻被销毁了！

    // 给后台线程一点时间打印内容
    std::this_thread::sleep_for(std::chrono::seconds(1)); 
    return 0;
}
```

---

## 二、 简化重复命名空间的属性列表

### 1. 概念补充
在 C++11 中引入了属性（Attributes），写在 `[[ ]]` 中。有些属性是由特定的编译器（如 GCC 或 MSVC）提供的，因此它们带有命名空间。
如果在一个地方要写多个同一个命名空间下的属性，以前必须重复写命名空间，代码很冗长。C++17 允许使用 `using` 提取公共的命名空间，简化写法。

### 2. 代码示例
```cpp
// C++17 之前的繁琐写法：必须重复写 gnu::
// [[gnu::always_inline, gnu::const, gnu::hot]]
// void myOptimizedFunction() {}

// C++17 简化后的写法：提取公共命名空间
[[using gnu: always_inline, const, hot]]
void myOptimizedFunction() {
    // 高度优化的函数体
}
```

---

## 三、 `__has_include` 预处理宏

### 1. 概念补充
这是一个专门为**跨平台、跨编译器、跨版本兼容**设计的宏。
在过去，我们要判断系统里有没有某个头文件非常困难。现在，通过 `#if __has_include(<头文件>)`，编译器会在预处理阶段去检查这个头文件是否存在。如果存在，就包含它；如果不存在，就使用替代方案。

### 2. 代码示例
```cpp
#include <iostream>

// 1. 问仓库管理员：咱们有 C++17 纯正版的“文件系统工具箱”吗？
#if __has_include(<filesystem>)
    #include <filesystem>               // 如果有，就把它拿出来用
    namespace fs = std::filesystem;     // 给它起个简短的绰号叫 fs，以后省得打全名
    #define HAS_FILESYSTEM 1            // 贴一张大字报，向全公司宣布：【工具已有，状态为 1】

// 2. 如果没有纯正版的，退而求其次：那有老版本的“测试版工具箱”吗？
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>  // 如果有测试版的，拿出来用
    namespace fs = std::experimental::filesystem; // 同样给它起名叫 fs，这样底下的代码就不需要改了！
    #define HAS_FILESYSTEM 1            // 贴大字报：【凑合能用，状态也为 1】

// 3. 最惨的情况：正版和测试版都没有！
#else
    #define HAS_FILESYSTEM 0            // 贴大字报：【啥都没有，状态为 0】
#endif

int main() {
    if (HAS_FILESYSTEM) {
        std::cout << "当前环境支持文件系统库！" << std::endl;
    } else {
        std::cout << "当前环境太旧，不支持文件系统库！" << std::endl;
    }
    return 0;
}
```

---

## 四、 C++17 新增的三大实用属性

这三个属性是 C++17 标准自带的，没有任何命名空间，主要用来向编译器传达程序员的“真实意图”，从而消除不必要的编译器警告，或者在容易出错的地方强制给出警告。

### 1. `[[fallthrough]]` (显式贯穿)
* **作用**：在 `switch-case` 语句中，如果我们故意不写 `break`，想让代码继续往下执行（贯穿），现代编译器通常会报警告（怀疑你漏写了 `break`）。加上 `[[fallthrough]];` 就能明确告诉编译器：“我是故意的，别烦我”。
```cpp
#include <iostream>

void processState(int state) {
    switch (state) {
        case 1:
            std::cout << "执行步骤 1" << std::endl;
            // 故意不写 break，直接进入步骤 2
            [[fallthrough]]; 
        case 2:
            std::cout << "执行步骤 2" << std::endl;
            break;
        default:
            break;
    }
}
```

### 2. `[[nodiscard]]` (不可丢弃返回值)
* **作用**：修饰函数。当调用该函数且**没有接收/处理它的返回值**时，编译器会强制给出一个警告。极其适用于那些返回“错误码”或者“申请的内存指针”的函数，防止开发者粗心忽略。
```cpp
#include <iostream>

// 这个函数的返回值极其重要，绝对不能被忽略
[[nodiscard]] bool checkSystemHealth() {
    // 假设进行了一系列检查
    return false; // 系统不健康
}

int main() {
    // 错误调用：返回值被丢弃了！编译器会在这里报警
    checkSystemHealth(); 

    // 正确调用：接收了返回值，编译器闭嘴
    bool isHealthy = checkSystemHealth();
    if (!isHealthy) {
        std::cout << "系统故障！" << std::endl;
    }
    return 0;
}
```

### 3. `[[maybe_unused]]` (可能未使用)
* **作用**：用于描述暂时没有被使用的变量、函数或参数。有些变量可能只在 Debug 模式（如 `assert` 中）起作用，到了 Release 模式就不参与编译了，此时编译器会报“声明了但未使用”的警告。加上这个属性可以消除警告。
```cpp
#include <iostream>
#include <cassert>

void doMath(int a, [[maybe_unused]] int b) {
    // 参数 b 被标记为可能不使用，如果函数体里真没用到 b，编译器也不会报警
    
    [[maybe_unused]] int debug_flag = 1; 
    // 这个变量如果在某些宏配置下没有被用到，也不会产生烦人的警告
    assert(debug_flag == 1); 

    std::cout << "计算结果: " << a * 10 << std::endl;
}

int main() {
    doMath(5, 10);
    return 0;
}
```