# 📝 C++ 进阶：文件操作 (File I/O)

程序运行时产生的数据都属于临时数据（存放在内存中），程序一旦运行结束都会被释放。通过文件可以将数据**持久化**（存入硬盘）。
C++ 中对文件操作需要包含头文件：`#include <fstream>`



## 1. 文件类型与三大操作类
* **文件类型**：
  1. **文本文件**：文件以 ASCII 码形式存储在计算机中。
  2. **二进制文件**：文件以文本的二进制形式存储在计算机中，用户一般不能直接读懂它们。
* **操作文件的三大巨头类**：
  1. `ofstream`：写操作 (Output File Stream)
  2. `ifstream`：读操作 (Input File Stream)
  3. `fstream`：读写操作 (File Stream)

---

## 2. 文本文件的读写

### 2.1 写文件步骤与打开方式
**写文件 5 步曲**：
1. 包含头文件：`#include <fstream>`
2. 创建流对象：`ofstream ofs;`
3. 打开文件：`ofs.open("文件路径", 打开方式);`
4. 写数据：`ofs << "写入的数据";`
5. 关闭文件：`ofs.close();`

**🚀 核心补全：文件打开方式**
* `ios::in` ：为读文件而打开文件
* `ios::out` ：为写文件而打开文件（如果文件存在，会清空原有内容）
* `ios::app` ：追加方式写文件（在文件尾部追加内容，不覆盖原有内容）
* `ios::trunc` ：如果文件存在，先删除再创建（`ios::out` 默认包含此模式）
* `ios::binary`：以二进制方式打开文件
* `ios::ate` ：初始位置：文件尾

*PS：打开方式可以配合使用，利用按位或 `|` 操作符。例如：用二进制方式写文件 `ios::binary | ios::out`*

#### 💻 代码示例：写文本文件
```cpp
#include <iostream>
#include <fstream> // 1. 包含头文件
using namespace std;

void testWriteText() {
    ofstream ofs; // 2. 创建写流对象
    ofs.open("test.txt", ios::out); // 3. 打开文件 (默认在当前项目目录下创建)

    // 4. 写数据
    ofs << "姓名：张三" << endl;
    ofs << "性别：男" << endl;
    ofs << "年龄：20" << endl;

    ofs.close(); // 5. 关闭文件
}
```

### 2.2 读文件步骤与 4 种读取方法
**读文件 5 步曲**：
1. 包含头文件：`#include <fstream>`
2. 创建流对象：`ifstream ifs;`
3. 打开文件并**判断是否成功**：`ifs.open("文件路径", ios::in);`
4. 读数据：**(共有 4 种常见方法)**
5. 关闭文件：`ifs.close();`

#### 💻 代码示例：读文本文件（补全 4 种读法）
```cpp
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void testReadText() {
    ifstream ifs;
    ifs.open("test.txt", ios::in);

    // 3. 判断文件是否打开成功（非常重要！防止读取不存在的文件导致崩溃）
    if (!ifs.is_open()) {
        cout << "文件打开失败！" << endl;
        return;
    }

    // 4. 读数据（这里展示最常用的 3 种，第 4 种逐字符读取效率低，实战极少用）
    
    // 方法一：把文件内容输入到字符数组中 (遇到空格和换行会结束)
    /*
    char buf[1024] = {0};
    while (ifs >> buf) {
        cout << buf << endl;
    }
    */

    // 方法二：利用 getline 读取到字符数组 (推荐，能读一整行)
    /*
    char buf[1024] = {0};
    while (ifs.getline(buf, sizeof(buf))) {
        cout << buf << endl;
    }
    */

    // 方法三：利用全局函数 getline 读取到 C++ 的 string 中 (最推荐，不需要考虑数组越界)
    string line;
    while (getline(ifs, line)) {
        cout << line << endl;
    }

    // 5. 关闭文件
    ifs.close();
}
```

---

## 3. 二进制文件的读写 (硬核重点)
二进制读写可以将我们在内存中自定义的复杂对象（类的实例）直接“生吞活剥”地存入硬盘，非常适合游戏存档、网络数据包序列化。

### 3.1 用二进制写文件
* **函数原型**：`ostream& write(const char * buffer, int len);`
* **参数解释**：字符指针 `buffer` 指向内存中一段存储空间（需要强制转换），`len` 是读写的字节数（通常用 `sizeof` 计算）。

### 3.2 用二进制读文件
* **函数原型**：`istream& read(char *buffer, int len);`
* **参数解释**：同上。利用流对象调用成员函数 `read`。

#### 💻 代码示例：二进制存取自定义对象
```cpp
#include <iostream>
#include <fstream>
using namespace std;

class Person {
public:
    // 💣 致命大坑：存入二进制文件的数据，绝对不能用 C++ 的 string 类！
    // 因为 string 内部包含指向堆区的指针。如果存入硬盘再读出来，
    // 指针指向的内存早就失效了（浅拷贝危机）！必须用 C 风格的字符数组！
    char m_Name[64]; 
    int m_Age;
};

// 二进制写文件 (存档)
void testWriteBinary() {
    // 可以在构造函数里直接打开文件，一行搞定前 3 步
    ofstream ofs("person.dat", ios::out | ios::binary);
    
    Person p = {"李四", 28};

    // 将对象 p 的地址强转为 const char*，写入 sizeof(Person) 大小的字节
    ofs.write((const char*)&p, sizeof(Person)); 
    ofs.close();
    cout << "二进制存档完毕！" << endl;
}

// 二进制读文件 (读档)
void testReadBinary() {
    ifstream ifs("person.dat", ios::in | ios::binary);
    if (!ifs.is_open()) {
        cout << "存档文件读取失败！" << endl;
        return;
    }

    Person p; // 准备一个空壳对象接收数据
    ifs.read((char*)&p, sizeof(Person)); 

    cout << "读档成功！姓名: " << p.m_Name << " 年龄: " << p.m_Age << endl;
    ifs.close();
}
```