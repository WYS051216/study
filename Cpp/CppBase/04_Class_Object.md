# 📝 C++ 面向对象核心：类与对象、封装与生命周期

## 1. 面向对象基础与封装
C++ 认为万事万物皆为对象，对象上有其**属性**（变量）和**行为**（函数）。
* **面向对象三大特性**：封装、继承、多态。

### 1.1 封装的意义
1. 将属性和行为作为一个整体，表现生活中的事物。
   * 语法：`class 类名 { 访问权限: 属性 / 行为 };`
   * 实例化：通过类去创建具体的对象，再用 `.` 运算符给其赋值或调用函数。
2. 将属性和行为加以权限控制。

| 访问权限 | 关键字 | 类内访问 | 类外访问 | 子类访问父类 |
| :--- | :--- | :--- | :--- | :--- |
| **公共权限** | `public` | ✅ 可以 | ✅ 可以 | ✅ 可以 |
| **保护权限** | `protected` | ✅ 可以 | ❌ 不可 | ✅ 可以 |
| **私有权限** | `private` | ✅ 可以 | ❌ 不可 | ❌ 不可 |

### 1.2 `struct` 和 `class` 的唯一区别
* `struct` 默认访问权限为 **公有 (`public`)**。
* `class` 默认访问权限为 **私有 (`private`)**。

### 1.3 成员属性私有化的优点
将所有成员属性设置为 `private` 后，外部要想访问，必须通过类提供的公有接口（Getter/Setter 函数）。
* **优点 1**：可以自己精确控制读写权限（只读、只写、或读写）。
* **优点 2**：对于写权限，可以在函数内部检测数据的有效性，防止乱填数据。

```cpp
#include <iostream>
#include <string>

class Student {
private:
    // 成员属性全部私有化，外界无法直接访问
    std::string name;
    int age;          // 年龄必须限制在 0~150 之间

public:
    // --- 优点 1：精确控制读写权限 ---

    // Getter：提供“只读”权限
    std::string getName() const { return name; }

    // Setter：提供“写”权限，并增加“防错”机制
    void setAge(int a) {
        // --- 优点 2：数据有效性检测 ---
        if (a < 0 || a > 150) {
            std::cout << "错误：年龄设置不合法！数据保持不变。" << std::endl;
            return;
        }
        age = a; // 只有合法的年龄才允许修改
    }

    // Getter：提供读权限
    int getAge() const { return age; }
};

int main() {
    Student s;

    // s.age = 200; // ❌ 编译报错！无法直接访问私有属性

    // ✅ 通过 Getter/Setter 进行受控访问
    s.setAge(20);    // 合法
    std::cout << "年龄: " << s.getAge() << std::endl;

    s.setAge(999);   // ❌ 非法！触发内部检测机制，保护了对象的状态
    
    return 0;
}
```

---

## 2. 对象的初始化和清理 (生命周期)
对象的诞生与死亡由**构造函数**和**析构函数**全权负责，它们由编译器自动调用。

### 2.1 构造函数（出生）
* **作用**：创建对象时为对象的成员属性赋值。
* **语法**：`类名() { }`
* **规则**：
  1. 没有返回值，也不写 `void`。
  2. 函数名称必须与类名完全相同。
  3. **可以有参数，因此可以发生重载**。
  4. 创建对象时系统自动调用，且只会调用一次。
   
### 2.2 析构函数（死亡）
* **作用**：对象销毁前系统自动调用，执行清理工作（比如 `delete` 堆区开辟的内存）。
* **语法**：`~类名() { }`
* **规则**：
  1. 没有返回值，也不写 `void`。
  2. 函数名称与类名相同，但在名称前加波浪号 `~`。
  3. **不可以有参数，因此绝对不可以发生重载**。
  4. 对象生命周期结束时自动调用，且只调用一次。

### 💻 综合代码示例 1：封装、私有化与构造/析构
```cpp
#include <iostream>
#include <string>
using namespace std;

class Person {
private: // 私有属性
    string m_Name;
    int m_Age;

public: // 公有行为
    // 1. 构造函数
    Person() {
        cout << "Person 的无参构造函数调用！" << endl;
    }
    
    // 2. 析构函数
    ~Person() {
        cout << "Person 的析构函数调用！" << endl;
    }

    // 3. 供外部使用的写接口 (Setter)，附带数据有效性检测
    void setAge(int age) {
        if (age < 0 || age > 150) {
            cout << "年龄输入有误，强制设为 0！" << endl;
            m_Age = 0;
            return;
        }
        m_Age = age;
    }
};
```

---

## 3. 构造函数的分类与调用

### 3.1 两种分类方式
* **按参数分类**：无参构造（默认构造）、有参构造。
* **按类型分类**：普通构造、拷贝构造。
  * **拷贝构造标准写法**：`Person(const Person &p) { ... }`

### 3.2 三种调用方式与代码示例
```cpp
class Person {
public:
    int age;
    Person() { cout << "无参构造" << endl; }               // 无参构造
    Person(int a) { age = a; cout << "有参构造" << endl; } // 有参构造
    Person(const Person &p) { age = p.age; cout << "拷贝构造" << endl; } // 拷贝构造
};

void test01() {
    // 1. 括号法 (最常用)
    Person p1;      // ✅ 默认构造。⚠️ 注意：千万别写成 Person p1(); 编译器会当成函数声明！
    Person p2(10);  // ✅ 有参构造
    Person p3(p2);  // ✅ 拷贝构造

    // 2. 显示法
    Person p4 = Person(10); // ✅ 有参构造
    Person p5 = Person(p4); // ✅ 拷贝构造
    // Person(10);          // 匿名对象。特点：当前行执行结束后，系统立即回收此对象。
    // ⚠️ 注意：不要用拷贝构造函数初始化匿名对象，如 Person(p5); 编译器会认为是 Person p5; 的对象声明导致重定义报错。

    // 3. 隐式转换法 (代码非常简练)
    Person p6 = 10; // ✅ 相当于 Person p6 = Person(10); 有参构造
    Person p7 = p6; // ✅ 相当于 Person p7 = Person(p6); 拷贝构造
    Person p2(10);

    // ❌ 这是拷贝构造（你的截图）：开辟新空间，内容复制
    Person p3(p2); 

    // ✅ 这是引用（代名词）：不开辟新空间，p4 和 p2 指向同一具肉体
    Person& p4 = p2;
}
```

---

## 4. 拷贝构造函数的调用时机
共有以下三种绝对会触发拷贝构造的情况：

```cpp
// 示例类同上
void doWork(Person p) { } // 形参是普通对象（非引用）
Person doWork2() { Person p1; return p1; } // 返回局部对象

void test02() {
    // 时机 ①：使用一个已经创建完毕的对象来初始化一个新对象
    Person p1(20);
    Person p2(p1); 
    
    // 时机 ②：以值传递的方式给函数参数传值
    Person p3;
    doWork(p3); // 将实参 p3 传给形参 p 时，会调用拷贝构造克隆出一个临时的 p

    // 时机 ③：以值方式返回局部对象
    Person p4 = doWork2(); // doWork2 内部的 p1 被拷贝一份返回给外部的 p4
}
```
> 🚨 **避坑指南**：在现代 C++ 编译器（如 VS 或 GCC）中，针对**时机③**，编译器通常会默认开启“返回值优化（RVO）”，导致你可能看不到拷贝构造的打印信息。这是正常的编译器底层优化，但理论考点依然认为它触发了拷贝行为。

---

## 5. 构造函数调用规则
默认情况下，C++ 编译器至少给一个类添加 3 个函数：
1. **默认构造函数**（无参，函数体为空）
2. **默认析构函数**（无参，函数体为空）
3. **默认拷贝构造函数**（对属性进行值拷贝，即咱们第一天讲的“浅拷贝”）

**🔥 核心调用规则（背诵级重点）：**
1. **写了有参构造，丢了无参构造**：如果用户定义了有参构造函数，C++ **不再提供默认无参构造**，但是依然会提供默认拷贝构造。
2. **写了拷贝构造，全家被端**：如果用户定义了拷贝构造函数，C++ **不会再提供任何其他默认构造函数**（必须手动把无参和有参构造补齐）。

## 6. 💣 致命考点：深拷贝与浅拷贝
当类中存在指针并且在堆区开辟了内存时，拷贝行为就会变得极其危险！

* **浅拷贝**：编译器默认提供的拷贝构造函数做的就是浅拷贝，即简单的**值拷贝**（原样复制）。
  * **灾难后果**：如果原对象中有个指针指向堆区的一块内存，浅拷贝后，新对象的指针也会指向**同一块内存**。当这两个对象被销毁时，析构函数会被调用两次，导致同一块堆内存被 `delete` 两次，触发 **Double Free（重复释放）段错误崩溃**！
* **深拷贝**：为了解决浅拷贝的致命缺陷，必须手动重写拷贝构造函数，在堆区**重新申请一块新空间**，将原空间的数据拷贝过来。这样两个对象就各自拥有一块独立的内存，互不干扰。

```cpp
#include <iostream>
using namespace std;

class Person {
public:
    int m_Age;
    int* m_Height; // 重点在这里！这是一个指针，我们将让它指向堆区内存

    // 1. 构造函数
    Person(int age, int height) {
        m_Age = age;
        // 在堆区开辟一块内存，存放身高数据
        m_Height = new int(height); 
        cout << "调用有参构造函数，开辟堆区内存" << endl;
    }

    // 2. 析构函数（对象销毁前自动调用，用于清理资源）
    ~Person() {
        if (m_Height != nullptr) {
            delete m_Height;   // 释放堆区内存
            m_Height = nullptr; // 防野指针
            cout << "调用析构函数，释放了堆区内存" << endl;
        }
    }
};

void test() {
    // 第一步：创建 p1
    Person p1(18, 180); 

    // 第二步：浅拷贝灾难发生！
    // 编译器默认提供的拷贝构造，只会做简单的值拷贝。
    // 它会把 p1.m_Height 里的“内存地址”原样抄写给 p2.m_Height。
    // 结果：p1 和 p2 的 m_Height 指向了堆区里的【同一块】内存！
    Person p2(p1); 

} // 第三步：test() 函数结束，局部变量 p1 和 p2 被销毁，触发析构函数

int main() {
    test(); // 运行测试
    return 0;
}
```
解决方案
```cpp
// 在 Person 类中手动添加深拷贝构造函数：
Person(const Person& p) {
    m_Age = p.m_Age;
    // 核心解法：重新 new 一块新内存！不要用同一个地址。
    m_Height = new int(*p.m_Height); 
}
```

## 7. ⚡ 性能利器：初始化列表
* **作用**：用来初始化类的成员属性。底层运行效率比在构造函数内部写 `=` 赋值更高，且是初始化 `const` 属性的唯一方法。
* **语法**：`构造函数() : 属性1(值1), 属性2(值2)... {}`
* **💻 代码示例**：
```cpp
class Person {
public:
    int m_A;
    int m_B;
    int m_C;

    // 传统方式：在函数体内赋值
    // Person(int a, int b, int c) { m_A = a; m_B = b; m_C = c; }

    // 初始化列表方式：极其简洁高效
    Person(int a, int b, int c) : m_A(a), m_B(b), m_C(c) {
        // 函数体通常为空
    }
};
```

## 8. 🪆 嵌套结构：类对象作为类成员 (对象成员)
C++ 类中的成员可以是另一个类的对象，我们称该成员为**对象成员**。
* **构造顺序（先造零件，再造整体）**：当其他类对象作为本类成员时，先调用对象成员的构造函数，再调用本类的构造函数。
* **析构顺序（先拆整体，再拆零件）**：析构的顺序和构造的顺序是**绝对相反**的。
* 
```cpp
#include <iostream>
#include <string>
using namespace std;

// ==========================================
// 1. 零件类：发动机 (Engine)
// ==========================================
class Engine {
public:
    Engine() {
        cout << "  -> 🔧 发动机 (零件) 制造完毕！ (调用 Engine 构造函数)" << endl;
    }
    ~Engine() {
        cout << "  -> 💥 发动机 (零件) 已被销毁！ (调用 Engine 析构函数)" << endl;
    }
};

// ==========================================
// 2. 整体类：汽车 (Car)
// ==========================================
class Car {
public:
    // 💡 重点在这里：将 Engine 类的对象作为 Car 类的成员
    // 这就是图片里说的“对象成员” (嵌套结构)
    Engine m_engine; 

    Car() {
        cout << "🚗 汽车 (整体) 组装完毕！ (调用 Car 构造函数)" << endl;
    }
    ~Car() {
        cout << "🚗 汽车 (整体) 开始报废拆解！ (调用 Car 析构函数)" << endl;
    }
};

// ==========================================
// 测试函数
// ==========================================
void test() {
    cout << "--- 🏭 工厂接单：开始生产一辆汽车 ---" << endl;
    Car myCar; // 创建汽车对象
    
    cout << "\n--- ⏳ 时光飞逝，车开到报废年限了，准备强制销毁 ---" << endl;
} // 当代码执行到这个右括号时，局部变量 myCar 的生命周期结束，触发析构函数

int main() {
    test();
    return 0;
}
```

## 9. 🔗 共享空间：静态成员 (`static`)
在成员变量和成员函数前加上关键字 `static`，称为静态成员。
### 9.1 静态成员变量
* 所有对象**共享同一份数据**（一改全改）。
* 在**编译阶段**就已经分配了内存（存在全局区）。
* **核心语法限制**：必须在**类内声明，类外初始化**。
* 两种访问方式：通过对象访问（`p.m_Age`），或直接通过类名访问（`Person::m_Age`）。

### 9.2 静态成员函数
* 所有对象**共享同一个函数**。
* **核心权限限制**：静态成员函数**只能访问静态成员变量**（因为它没有 `this` 指针，不知道该去访问哪个具体对象的非静态变量）。
* 静态成员函数也有访问权限控制，类外无法访问 `private` 静态成员函数。
* 静态成员函数是属于整个类的，而不是属于某个对象的，哪怕不创建对象，也可以通过类名调用

## 10. 🧭 C++对象模型和 `this` 指针
### 10.1 成员的存储模型
在 C++ 中，类内的成员变量和成员函数是**分开存储**的。
* 只有**非静态成员变量**才属于类的对象（占用对象的内存空间）。
* 静态成员变量、非静态成员函数、静态成员函数 统统都不占用具体对象的内存。

* **空对象的内存**：空对象占用空间为 **1 字节**。C++ 编译器会给每个空对象分配一个字节的空间，是为了给它分配一个独一无二的内存地址（证明它存在过）。

### 10.2 `this` 指针的奥秘
由于非静态成员函数只有一份，所有对象都在共享调用它。那么这个函数怎么知道现在是哪个对象在调用自己呢？**靠的就是 `this` 指针！**
* **本质**：`this` 指针指向**被调用的成员函数所属的对象**。它是隐含在每一个非静态成员函数内的一种指针，不需要定义，直接使用即可。
* **两大核心用途与代码示例**：
```cpp
class Person {
public:
    int age; // 成员变量

    Person(int age) {
        // 用途 1：当形参和成员变量同名时，可用 this 指针来区分
        // 如果只写 age = age; 编译器会认为是形参自己给自己赋值，毫无意义
        this->age = age; 
    }

    // 用途 2：在类的非静态成员函数中返回对象本身 (return *this)
    // ⚠️ 注意：返回值类型必须是引用 (Person&)，否则会触发拷贝构造返回一个新的临时对象！
    Person& addAge(Person &p) {
        this->age += p.age;
        return *this; // 返回调用这个函数的对象本身
    }
};

void test() {
    Person p1(10);
    Person p2(10);
    // 链式编程思想：因为 p2.addAge(p1) 返回的还是 p2 本身，所以可以一直点下去！
    p2.addAge(p1).addAge(p1).addAge(p1); 
}
```

## 11. 空指针访问成员函数
在 C++ 中，空指针也是可以调用成员函数的。
* **原理解析**：因为成员函数和成员变量是分开存储的，函数代码只有一份，属于类而不属于某个具体的对象。所以即使指针是空的，也能找到函数代码。
* **⚠️ 致命危险**：如果调用的成员函数内部用到了 `this` 指针（比如访问了成员变量），因为此时 `this` 是空指针，访问变量就等于“解引用空指针”，程序会立刻段错误崩溃！
* **防坑规范**：在用到 `this` 的函数开头，加上判断 `if (this == NULL) return;` 来保证代码的健壮性。

---

## 12. `const` 修饰成员函数 (常函数与常对象)
`this` 指针的本质是一个**指针常量**（`Type* const this`），它的指向是不可以修改的。如果在成员函数后面加 `const`，修饰的就是 `this` 指针，让指针指向的值也不可以修改（变成 `const Type* const this`）。

### 12.1 常函数
* **语法**：成员函数括号后加 `const`。
* **特点**：常函数内**不可以修改**普通的成员属性。
* **后门（例外）**：如果成员属性声明时加上 `mutable` 关键字，那么在常函数中依然可以无视 `const` 进行修改。

### 12.2 常对象
* **语法**：声明对象前加 `const`（如 `const Person p;`）。
* **特点**：常对象**只能调用常函数**。（因为普通函数有修改属性的风险，常对象绝对不允许这种风险存在）。

### 💻 代码示例：常函数与常对象
```cpp
class Person {
public:
    int m_A;
    mutable int m_B; // 特殊变量，即使在常函数中，也可以修改这个值

    // this 指针的本质是 Person* const this;
    // 在函数后面加 const，相当于 const Person* const this;
    void showPerson() const {
        // m_A = 100; // ❌ 报错！常函数不可修改普通属性
        m_B = 100;    // ✅ 正确！因为 m_B 有 mutable 修饰
    }

    void func() { } // 普通函数
};

void testConst() {
    const Person p; // 常对象
    // p.m_A = 100; // ❌ 报错！常对象不能修改普通属性
    p.m_B = 100;    // ✅ 正确！mutable 属性随时可改

    p.showPerson(); // ✅ 正确！常对象只能调用常函数
    // p.func();    // ❌ 报错！常对象不可以调用普通成员函数
}
```

---

## 13. 友元 (`friend`)
* **目的**：让一个函数或者类，能够合法地访问另一个类中的 `private` 私有成员。
* **关键字**：`friend`
* **三种实现方式**：
  1. 全局函数做友元
  2. 类做友元
  3. 成员函数做友元

### 💻 代码示例：全局函数与类做友元
```cpp
class Building {
    // 1. 告诉编译器，全局函数 goodGay 是好朋友，可以访问私有成员
    friend void goodGay(Building *building); 
    // 2. 告诉编译器，类 GoodBoy 是好朋友，可以访问私有成员
    friend class GoodBoy; 

public:
    string m_SittingRoom; // 客厅
private:
    string m_BedRoom;     // 卧室
public:
    Building() {
        m_SittingRoom = "客厅";
        m_BedRoom = "卧室";
    }
};

// 全局函数
void goodGay(Building *building) {
    cout << "好基友正在访问： " << building->m_SittingRoom << endl;
    cout << "好基友正在访问： " << building->m_BedRoom << endl; // ✅ 有了友元，可以访问私有
}
```

---

## 14. 运算符重载 (Operator Overloading)
* **概念**：对已有的运算符重新进行定义，赋予其另一种功能，以适应不同的数据类型（让自定义的对象也能像普通数字一样进行 `+`、`-`、`<<` 等操作）。

### 14.1 加号运算符重载 (`+`)
* **作用**：实现两个自定义数据类型相加的运算。
* **两种重载方式**：成员函数重载、全局函数重载。
* **特点**：运算符重载也可以发生函数重载（比如 `对象 + 对象`，也可以重载一个 `对象 + 整数`）。

### 💻 代码示例：重载加号 (`+`)
```cpp
class Person {
public:
    int m_A;
    int m_B;
    
    // 成员函数重载 + 号
    Person operator+(const Person& p) {
        Person temp;
        temp.m_A = this->m_A + p.m_A;
        temp.m_B = this->m_B + p.m_B;
        return temp;
    }
};

void testAdd() {
    Person p1; p1.m_A = 10; p1.m_B = 10;
    Person p2; p2.m_A = 20; p2.m_B = 20;
    Person p3 = p1 + p2; // 本质相当于：p1.operator+(p2)
}
```

### 14.2 左移运算符重载 (`<<`)
* **作用**：可以直接用 `cout << 对象` 输出自定义数据类型。
* **⚠️ 核心考点**：**不要利用成员函数重载 `<<`！** 因为如果写在成员函数里，调用起来会变成 `p << cout`，非常反人类。只能利用**全局函数**重载。

### 💻 代码示例：重载左移 (`<<`)
```cpp
class Person {
    friend ostream& operator<<(ostream& cout, const Person& p); // 配合友元输出私有属性
private:
    int m_A = 10;
    int m_B = 20;
};

// 只能利用全局函数重载左移运算符
// ⚠️ 必须返回 ostream& (输出流引用)，这样才能支持链式追加输出 (cout << p << " endl")
ostream& operator<<(ostream& cout, const Person& p) {
    cout << "m_A = " << p.m_A << " m_B = " << p.m_B;
    return cout; 
}
```

### 14.3 递增运算符重载 (`++`)
* **作用**：通过递增运算符，实现自定义的整型数据递增。
* **⚠️ 核心区别**：
  * **前置 `++`**：先递增，再返回。必须**返回引用**（`&`），以便对同一个对象持续操作。
  * **后置 `++`**：先记录原值，再递增，最后返回原值。必须**返回值**（不可返回局部临时对象的引用），且函数参数列表中必须加一个 `int` 作为占位符，用来和前置 `++` 做区分！

### 💻 代码示例：重载递增 (`++`)
```cpp
class MyInteger {
    friend ostream& operator<<(ostream& cout, const MyInteger& myInt);
private:
    int m_Num = 0;
public:
    // 1. 重载前置 ++运算符 (返回引用)
    MyInteger& operator++() {
        m_Num++; // 先递增
        return *this; // 再返回自身
    }

    // 2. 重载后置 ++运算符 (返回值)
    // 括号里的 int 代表占位参数，专门用于区分前置和后置
    MyInteger operator++(int) {
        MyInteger temp = *this; // 先记录当时的结果
        m_Num++;                // 再递增
        return temp;            // 最后将记录的原值返回
    }
};
```
### 14.4 赋值运算符重载 (`operator=`)
* **致命陷阱**：C++ 编译器会给一个类添加默认的赋值运算符 `=`，但它做的是**简单的值拷贝（浅拷贝）**。如果类中有指针并在堆区开辟了内存，直接使用 `a = b` 会导致堆区内存重复释放（Double Free）和内存泄漏！
* **解决办法**：必须重写赋值运算符，利用**深拷贝**解决问题。

```cpp
class Person {
public:
    int* m_Age;
    Person(int age) { m_Age = new int(age); }
    ~Person() {
        if (m_Age != NULL) {
            delete m_Age;
            m_Age = NULL;
        }
    }

    // 重载赋值运算符
    Person& operator=(const Person& p) {
        // 1. 先判断自身是否有在堆区的数据，如果有，先释放干净
        if (m_Age != NULL) {
            delete m_Age;
            m_Age = NULL;
        }
        // 2. 重新开辟内存，进行深拷贝
        m_Age = new int(*p.m_Age);
        // 3. 返回对象本身，以支持连等操作 (a = b = c)
        return *this; 
    }
};
```

### 14.5 关系运算符重载 (`==` 和 `!=`)
* **作用**：让自定义的类对象也能像基本数据类型一样，进行相等或不等的条件判断。

```cpp
class Person {
public:
    string m_Name;
    int m_Age;
    Person(string name, int age) : m_Name(name), m_Age(age) {}

    // 重载 == 运算符
    bool operator==(const Person& p) {
        return (this->m_Name == p.m_Name && this->m_Age == p.m_Age);
    }
    // 重载 != 运算符
    bool operator!=(const Person& p) {
        return (this->m_Name != p.m_Name || this->m_Age != p.m_Age);
    }
};
```

### 14.6 函数调用运算符重载 (`operator()`)
* **概念**：重载了括号 `()` 之后，对象使用起来非常像一个普通的函数调用，因此被称为**仿函数 (Functor)**。
* **特点**：仿函数没有固定写法，非常灵活。

```cpp
class MyPrint {
public:
    // 重载 () 运算符
    void operator()(string text) {
        cout << text << endl;
    }
};

class MyAdd {
public:
    int operator()(int v1, int v2) {
        return v1 + v2;
    }
};

void test() {
    MyPrint myPrint;
    myPrint("Hello 仿函数!"); // 看起来像调用函数，其实是对象在调用重载的 operator()

    // 匿名函数对象调用：类名() 产生匿名对象，紧接着 (10, 20) 调用仿函数
    cout << "相加结果: " << MyAdd()(10, 20) << endl; 
    // 特点：当前行执行完毕后，匿名对象立即被销毁
}
```

---

## 15. 继承 (Inheritance) 基础
* **核心好处**：抽取共性，减少重复的代码。
* **基本语法**：`class 子类名 : 继承方式 父类名 { };` 
  * (子类也叫**派生类**，父类也叫**基类**)

### 15.1 继承方式与权限映射

无论哪种继承方式，父类中的 `private` 私有成员，子类统统**访问不到**（被隐藏了）。
* **公共继承 (`public`)**：父类的 `public` 变子类的 `public`，父类的 `protected` 变子类的 `protected`。
* **保护继承 (`protected`)**：父类的 `public` 和 `protected`，到了子类中统统变成 `protected`（类外无法访问）。
* **私有继承 (`private`)**：父类的 `public` 和 `protected`，到了子类中统统变成 `private`。

```cpp
class Base {
public:    int m_A;
protected: int m_B;
private:   int m_C; // 私有成员，子类绝对拿不到
};

// 以 Public 方式继承
class Son : public Base {
public:
    void func() {
        m_A = 10; // ✅ 依然是 public
        m_B = 20; // ✅ 依然是 protected
        // m_C = 30; // ❌ 报错！父类私有成员不可访问
    }
};
```

---

## 16. 继承中的底层机制

### 16.1 继承中的对象模型

* **核心考点**：父类中**所有**的非静态成员属性（哪怕是 `private` 的）都会被子类继承下去！子类创建对象所占的内存字节数，包含了父类所有的属性。只是编译器把父类的私有成员隐藏了，不让你直接调用而已。
* **开发者工具查看底层**：
  打开 VS 开发人员命令提示符，跳转到对应盘符和目录，输入命令：
  `cl /d1 reportSingleClassLayout类名 文件名.cpp`

### 16.2 构造和析构的调用顺序
子类继承父类后，创建和销毁子类对象时，会牵连父类的构造与析构。
* **口诀**：**先有父再有子，先送子再送父**。（就像建房子，先打地基再盖楼；拆房子，先拆楼再挖地基）。
* **顺序**：父类构造 ➡️ 子类构造 ➡️ 子类析构 ➡️ 父类析构。

---

## 17. 继承中的同名处理机制 (重名隐藏规则)
当子类和父类出现了同名的成员（变量或函数）时，如何区分到底调用的是谁？

### 17.1 同名非静态成员处理
* **访问子类自身**：直接访问即可。（默认就近原则）
* **访问父类成员**：必须加上**父类的作用域** (`父类名::`)。
* **🔥 隐藏规则大坑**：如果子类中出现和父类同名的成员函数，子类会**隐藏**掉父类中**所有**同名成员函数（包括重载的版本）。想访问被隐藏的父类函数，必须加作用域！

```cpp
class Base {
public:
    int m_A = 100;
    void func() { cout << "Base 的 func()" << endl; }
};

class Son : public Base {
public:
    int m_A = 200;
    void func() { cout << "Son 的 func()" << endl; }
};

void test() {
    Son s;
    cout << "子类的 m_A: " << s.m_A << endl;       // 输出 200
    cout << "父类的 m_A: " << s.Base::m_A << endl; // 输出 100，加作用域穿透！
    
    s.func();       // 调用子类的 func
    s.Base::func(); // 调用父类的 func
}
```

### 17.2 同名静态成员处理
静态成员处理方式和非静态完全一样，只是因为静态成员全类共享，所以多了一种**“通过类名访问”**的方式。

```cpp
class Base {
public:
    static int m_A;
};
int Base::m_A = 100;

class Son : public Base {
public:
    static int m_A;
};
int Son::m_A = 200;

void testStatic() {
    // 方式一：通过对象访问 (和非静态一样)
    Son s;
    cout << s.m_A << endl;       // 200
    cout << s.Base::m_A << endl; // 100

    // 方式二：通过类名直接访问 (因为不需要实例化对象)
    cout << Son::m_A << endl;       // 200
    // 🔥 极其酷炫的连续作用域写法：
    // 第一个 Son:: 代表通过子类类名访问，第二个 Base:: 代表指明是父类作用域下的 m_A
    cout << Son::Base::m_A << endl; // 100
}
```
# 📝 C++ 面向对象终极篇：多继承、菱形继承与多态

## 18. 多继承语法
* **语法**：`class 子类 : 继承方式 父类1, 继承方式 父类2 { ... };`
* **⚠️ 实际开发避坑**：多继承极易引发父类中有同名成员出现，导致调用时的二义性问题。如果出现同名，必须加**作用域**区分。因此在实际工程架构中，**极其不建议使用多继承**（一般用单继承 + 多个纯虚接口来代替）。

### 💻 代码示例：多继承与作用域区分
```cpp
class Base1 {
public:
    int m_A = 100;
};

class Base2 {
public:
    int m_A = 200; // 与 Base1 同名
};

// 多继承语法
class Son : public Base1, public Base2 {
public:
    int m_C = 300;
};

void testMultiInheritance() {
    Son s;
    // cout << s.m_A << endl; // ❌ 报错：二义性，编译器不知道你调哪个 m_A
    cout << "Base1 的 m_A: " << s.Base1::m_A << endl; // ✅ 必须加作用域
    cout << "Base2 的 m_A: " << s.Base2::m_A << endl; // ✅ 必须加作用域
}
```

---

## 19. 菱形继承 (钻石继承)

* **概念**：有两个派生类继承同一个基类，同时又有某个类同时继承着这两个派生类（比如：羊继承动物，驼继承动物，羊驼继承了羊和驼）。
* **问题**：羊驼会继承两份动物的数据（比如 `m_Age`），导致数据冗余和二义性。
* **终极解决办法**：利用**虚继承 (`virtual`)** 可以解决菱形继承的问题。在继承之前加上 `virtual` 关键字，最顶层的基类就变成了**虚基类**。底层会通过虚基类指针（vbptr）指向唯一的共享数据。

### 💻 代码示例：虚继承解决菱形继承
```cpp
class Animal {
public:
    int m_Age;
};

// 关键步骤：加上 virtual 变成虚继承
class Sheep : virtual public Animal {}; 
class Tuo : virtual public Animal {};   

// 羊驼多继承羊和驼
class SheepTuo : public Sheep, public Tuo {};

void testDiamondInheritance() {
    SheepTuo st;
    st.Sheep::m_Age = 18;
    st.Tuo::m_Age = 28;

    // 因为是虚继承，现在 Animal 里的 m_Age 只有一份共享数据！
    // 最后一次赋值是 28，所以现在年龄就是 28。
    cout << "羊驼的年龄是: " << st.m_Age << endl; // 不再有二义性，直接访问即可！
}
```

---

## 20. 多态 (Polymorphism) —— 面向对象的核心灵魂

多态分为两类：
* **静态多态**：函数重载和运算符重载。（**早绑定**：在编译阶段就已经确定了函数的地址）。
* **动态多态**：派生类和虚函数实现运行时多态。（**晚绑定**：在运行阶段才去确定具体调用哪个函数的地址）。

### 20.1 多态满足条件与使用原则
* **满足条件**：
  1. 必须有继承关系。
  2. 子类必须**重写**父类中的虚函数（`virtual`）。
     * *注：重写是指函数返回值类型、函数名、参数列表完全一致。此时子类中的虚函数表内部会替换成子类的虚函数地址。*
* **使用核心机制**：**父类指针或引用指向子类对象**。
* **多态的好处**：代码组织结构极其清晰；可读性强；符合“开闭原则”（对扩展开放，对修改关闭），后期扩展和维护性极高！

### 💻 代码示例：多态实战 (卡牌游戏出牌逻辑)
```cpp
#include <iostream>
using namespace std;

// 1. 定义一个父类（玩家类）
class Player {
public:
    // 关键：加上 virtual 关键字，实现晚绑定！
    virtual void playCard() {
        cout << "普通玩家出了一张未知牌" << endl;
    }
};

// 2. 子类（地主）重写父类虚函数
class Landlord : public Player {
public:
    void playCard() { // 重写时子类的 virtual 可写可不写
        cout << "【地主】嚣张地打出了一副王炸！" << endl;
    }
};

// 3. 子类（农民）重写父类虚函数
class Peasant : public Player {
public:
    void playCard() {
        cout << "【农民】瑟瑟发抖地出了一个 3..." << endl;
    }
};

// 🌟 多态的威力所在：统一的业务接口！
// 只要形参是父类指针，你传什么子类对象进来，它就执行对应子类的代码 (指哪打哪)
void doPlay(Player* p) {
    p->playCard(); 
}

void testPolymorphism() {
    Landlord boss;
    Peasant worker;

    // 父类指针指向子类对象，触发多态！
    doPlay(&boss);   // 输出：【地主】嚣张地打出了一副王炸！
    doPlay(&worker); // 输出：【农民】瑟瑟发抖地出了一个 3...
}
```

---

## 21. 纯虚函数和抽象类
在多态中，通常父类中虚函数的具体实现是毫无意义的（比如上面 `Player` 里的 `playCard`，主要都是调用子类重写的内容）。因此，我们可以将它改为**纯虚函数**。

* **纯虚函数语法**：`virtual 返回值类型 函数名(参数列表) = 0;`
* **抽象类**：当类中只要有**一个或多个**纯虚函数，这个类就直接被称为**抽象类**。
* **抽象类核心特点**：
  1. **绝对无法实例化对象**（不能用它直接创建具体的变量，也不能 `new` 它）。
  2. 子类**必须重写**抽象类中的纯虚函数，否则子类也依然是抽象类，同样无法实例化！

### 💻 代码示例：纯虚函数与抽象类
```cpp
// 抽象类（相当于定义了一套必须遵守的接口规则）
class AbstractCalculator {
public:
    int m_Num1;
    int m_Num2;

    // 纯虚函数：我只定规矩，不写实现。强制要求子类去实现！
    virtual int getResult() = 0; 
};

// 加法计算器类（子类）
class AddCalculator : public AbstractCalculator {
public:
    // 必须重写纯虚函数，否则此类无法使用
    int getResult() {
        return m_Num1 + m_Num2;
    }
};

// 减法计算器类（子类）
class SubCalculator : public AbstractCalculator {
public:
    int getResult() {
        return m_Num1 - m_Num2;
    }
};

void testAbstractClass() {
    // AbstractCalculator ac; // ❌ 报错！抽象类无法实例化对象！

    // 多态应用：父类指针指向子类对象
    AbstractCalculator* calc = new AddCalculator; 
    calc->m_Num1 = 100;
    calc->m_Num2 = 200;
    cout << "加法结果: " << calc->getResult() << endl; // 输出 300
    delete calc; // 释放堆区内存

    calc = new SubCalculator;
    calc->m_Num1 = 100;
    calc->m_Num2 = 200;
    cout << "减法结果: " << calc->getResult() << endl; // 输出 -100
    delete calc;
}
```
## 22. 💣 多态的终极大坑：虚析构和纯虚析构
在多态的使用场景中（父类指针指向子类对象），如果子类中有属性开辟到了堆区（使用了 `new`），当释放父类指针时，**编译器默认只会调用父类的析构函数，而无法调用子类的析构代码**。
* **灾难后果**：子类在堆区申请的内存得不到释放，导致严重的内存泄漏（服务器一直跑，内存一直涨，最后直接宕机崩溃）！
* **终极解法**：将父类中的析构函数改为**虚析构**或者**纯虚析构**。

### 22.1 虚析构与纯虚析构的共性与区别
* **共性**：
  1. 都可以完美解决父类指针释放子类对象时不干净的问题（让子类的析构函数得以顺利执行）。
  2. **都需要有具体的函数实现**。（即使是纯虚析构，也必须在类外写具体的代码块！因为父类自己也可能有需要释放的数据）。
* **区别**：
  1. 如果类中包含了纯虚析构，那么这个类就变成了**抽象类**，绝对无法实例化对象。

### 22.2 语法规范
* **虚析构语法**：`virtual ~类名() { // 具体实现 }`
* **纯虚析构语法**：
  * 类内声明：`virtual ~类名() = 0;`
  * 类外实现：`类名::~类名() { // 具体实现 }`

### 💻 代码示例：虚析构挽救内存泄漏 (斗地主玩家断线回收)
```cpp
#include <iostream>
#include <string>
using namespace std;

// 抽象父类：玩家接口
class Player {
public:
    Player() { cout << "Player 构造函数调用" << endl; }
    
    virtual void playCard() = 0; // 纯虚函数

    // 🌟 核心：纯虚析构声明 (不加 virtual 的话，子类的析构绝对不会被调用！)
    virtual ~Player() = 0; 
};

// ⚠️ 极其容易踩坑的重点：纯虚析构必须在类外有具体的实现！
// 因为父类也有可能在堆区开了内存，析构时需要连父类的逻辑一起清理
Player::~Player() {
    cout << "Player 纯虚析构函数调用" << endl;
}

// 子类：地主玩家
class Landlord : public Player {
public:
    string* m_HandCards; // 假设手牌数据开辟在堆区

    Landlord(string name) {
        cout << "Landlord 构造函数调用" << endl;
        m_HandCards = new string(name); // 在堆区给手牌分配内存
    }

    void playCard() override {
        cout << *m_HandCards << " 打出了王炸！" << endl;
    }

    // 子类的析构函数，专门负责清理地主自己的堆区数据
    ~Landlord() {
        if (m_HandCards != NULL) {
            delete m_HandCards;
            m_HandCards = NULL;
        }
        cout << "Landlord 析构函数调用，成功清理堆区手牌内存！" << endl;
    }
};

void testVirtualDestructor() {
    // 多态：父类指针指向子类对象
    Player* p = new Landlord("嚣张的地主");
    p->playCard();

    // 准备释放内存
    // 如果父类的析构不是虚析构，这里 delete p 只会打印 "Player 析构"，导致 Landlord 内存泄漏！
    // 加了虚析构后，会先调用 Landlord 的析构，再调用 Player 的析构，完美回收！
    delete p; 
}
```
