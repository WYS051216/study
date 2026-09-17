# C++11 新特性：nullptr 与 NULL 的区别

## 1. 核心概念
* **`nullptr` 的作用**：专门用来初始化空指针，代表一个“空指针常量”。
* **引入背景**：为了解决 C++98/03 中 `NULL` 带来的二义性问题（Ambiguity）。
* **类型**：`nullptr` 的类型是 `std::nullptr_t`，它可以隐式转换成任意类型的指针，但**不能**转换成整数。

---

## 2. 为什么弃用 NULL？(NULL vs nullptr)

| 特性 | NULL (旧 C++) | nullptr (C++11) |
| :--- | :--- | :--- |
| **本质定义** | 通常被定义为整数 `0` (`#define NULL 0`) | 关键字，类型为 `std::nullptr_t` |
| **重载安全性** | **不安全**。重载函数时会被视为 `int` | **安全**。明确匹配指针类型的函数 |
| **可读性** | 容易与整数 0 混淆 | 语义明确，一眼看出是空指针 |

> **关键点**：在 C 语言中，`NULL` 通常定义为 `(void*)0`；但在 C++ 中，由于不允许 `void*` 隐式转为其他指针，所以 `NULL` 只能定义为整数 `0`。这就导致了下面的函数重载问题。

---

## 3. 代码实战与深度解析

```cpp
#include <iostream>
using namespace std;

// 重载函数 1：接收 char* 指针
void func(char *p) {
    cout << "void func(char *p)" << endl;
}

// 重载函数 2：接收 int 整数
void func(int p) {
    cout << "void func(int p)" << endl;
}

int main() {
    // 1. 初始化空指针 (推荐使用 nullptr)
    int* ptr1 = nullptr;
    char* ptr2 = nullptr;
    double* ptr3 = nullptr;
    void* ptr4 = nullptr;

    // 2. 调用整数参数的函数
    func(10); // 输出: void func(int p)

    // 3. NULL 的二义性问题 (老版本 C++ 的坑)
    // func(NULL); 
    // ❌ 原因：在 C++ 中 NULL 被定义为 0。编译器看到 0 是整数，
    // 会优先匹配 func(int p)，而不是 func(char *p)，这违背了我们要传空指针的初衷。
    // 在现代编译器中，这可能会直接报错“二义性”。

    // 4. nullptr 的正确性
    func(nullptr); 
    // ✅ 输出: void func(char *p)
    // 原因：nullptr 是指针类型，它会自动匹配参数为指针的函数。
    // 注意：如果同时存在 func(char*) 和 func(int*)，nullptr 会报错二义性，
    // 因为它能转成任意指针，编译器不知道你想转成哪一个。

    // 5. 关于 void* 的类型转换 (C vs C++)
    // C语言：void* 可以隐式转换为 int* (不安全)
    // C++  ：void* 不能隐式转换为 int* (更严格，更安全)
    
    // int* ptr5 = ptr4;      // ❌ 报错：无法从 void* 转换为 int*
    int* ptr5 = (int *)ptr4;  // ✅ 正确：必须进行显式强制转换

    return 0;
}
```
## 4. 学习总结 (Takeaway)

1.  **初始化习惯**：在 C++11 及以后，凡是需要空指针的地方，**一律使用 `nullptr`**，彻底忘掉 `NULL`
2.  **函数重载**：`nullptr` 能够精确区分“整数 0”和“空指针”，避免调用错误的重载函数。
3.  **类型转换**：
    * `nullptr` $\rightarrow$ 任意指针类型 (OK)
    * `nullptr` $\rightarrow$ 整数 (Error)
    * `void*` $\rightarrow$ 具体类型指针 (需要显式强转，如 `(int*)`)