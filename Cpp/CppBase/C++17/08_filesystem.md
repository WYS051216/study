# C++17 现代特性笔记（八）：原生文件系统 `<filesystem>`

---

## 一、 核心概念与命名空间

在使用文件系统前，必须引入专属的头文件。由于 `std::filesystem` 这个名字比较长，在业界和实际开发中，极其推荐给它起一个简短的**命名空间别名 (Alias)**。

```cpp
#include <filesystem>

// 业界标准惯例：使用 fs 作为别名，极大地提高代码可读性
namespace fs = std::filesystem; 
```

---

## 二、 三大核心类与常用接口（函数）

`<filesystem>` 的设计非常模块化，主要由以下三个核心类以及一系列全局函数构成。

### 1. `path` 类 (路径处理专家)
过去用字符串拼接路径（经常漏写或者多写斜杠 `/` 或 `\`），现在 `path` 类完美解决了跨平台路径拼接和解析的问题。

| 常用操作/函数 | 功能说明 | 亮点 |
| :--- | :--- | :--- |
| **`operator /`** | **路径拼接运算符** | 极其优雅！`path1 / "test.txt"` 会自动补全斜杠。 |
| **`filename()`** | 获取完整的文件名 | 例如从 `/dir/a.txt` 中提取出 `"a.txt"`。 |
| **`extension()`** | 获取文件扩展名 | 直接提取出 `".txt"`。 |
| **`parent_path()`**| 获取父级目录路径 | 提取出 `/dir`。 |
| **`string()`** | 转换为标准字符串 | 方便用于老旧的 C 风格接口或直接打印。 |

### 2. `directory_iterator` 类 (目录迭代器)
这是一个极其好用的容器迭代器，专门用来获取某个文件夹里到底有哪些东西（文件或子目录）。

| 常用操作/函数 | 功能说明 | 亮点 |
| :--- | :--- | :--- |
| **`path()`** | 获取当前遍历到的项目路径 | 返回一个 `path` 对象。 |
| **`is_directory()`**| 判断当前项目是否为文件夹 | 自动根据底层属性判断，无需手动传参。 |
| **`is_regular_file()`**| 判断当前项目是否为普通文件| 排除隐藏系统文件、快捷方式等特殊类型。 |
| *进阶容器* | `recursive_directory_iterator`| 如果你想**连同子文件夹里面的文件一起遍历**，直接换用这个递归版本，连递归函数都不用自己写！ |

### 3. `file_status` 类与全局操作函数
`file_status` 主要用于获取和修改文件（或目录）的底层属性。但在实际开发中，我们很少直接操作 `file_status` 对象，而是配合 `<filesystem>` 提供的**全局便捷函数**一起使用：

| 全局/状态函数 | 功能说明 |
| :--- | :--- |
| **`fs::status(path)`** | 获取路径的 `file_status` 对象（包含权限、类型等底层信息）。 |
| **`fs::exists(path)`** | **极常用**：判断该路径（文件或文件夹）是否存在。 |
| **`fs::file_size(path)`** | **极常用**：获取文件的字节大小（返回 `uintmax_t`）。 |
| **`fs::create_directory()`**| 创建单层新文件夹。 |
| **`fs::create_directories()`**| **极强大**：如果路径是 `a/b/c`，它会一口气把 `a`、`b` 和 `c` 嵌套创建出来！ |
| **`fs::remove(path)`** | 删除空文件夹或单个文件。 |
| **`fs::remove_all(path)`**| **极强大**：连同文件夹里的所有文件一起，彻底强制删除（相当于 `rm -rf`）。 |

---

## 三、 代码示例：全流程文件操作演示

这段代码展示了如何优雅地创建目录、拼接路径、写入文件、遍历目录以及最后的清理工作。

```cpp
#include <iostream>
#include <fstream>
#include <filesystem>

// 使用简写别名
namespace fs = std::filesystem;

int main() {
    // 1. 定义一个目录路径
    fs::path dir_path = "./my_test_dir";

    // 2. 判断是否存在，不存在则创建
    if (!fs::exists(dir_path)) {
        fs::create_directory(dir_path);
        std::cout << "文件夹创建成功: " << dir_path.string() << std::endl;
    }

    // 3. 优雅的路径拼接 (使用 / 运算符)
    fs::path file_path = dir_path / "hello.txt";
    
    // 写入一些内容创建出实体文件
    std::ofstream(file_path) << "Modern C++ Filesystem is Awesome!";

    // 4. 使用 path 类的内置函数解析路径
    std::cout << "\n--- 解析文件路径 ---" << std::endl;
    std::cout << "完整路径: " << file_path << std::endl;
    std::cout << "仅文件名: " << file_path.filename() << std::endl;
    std::cout << "仅扩展名: " << file_path.extension() << std::endl;
    std::cout << "父级目录: " << file_path.parent_path() << std::endl;

    // 5. 使用 directory_iterator 遍历目录
    std::cout << "\n--- 遍历目录内容 ---" << std::endl;
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        
        std::cout << "发现项目: " << entry.path().filename();
        
        // 判断属性
        if (entry.is_regular_file()) {
            // 获取文件大小
            std::cout << " [普通文件] 大小: " << fs::file_size(entry) << " 字节" << std::endl;
        } else if (entry.is_directory()) {
            std::cout << " [文件夹]" << std::endl;
        }
    }

    // 6. 打扫战场：递归删除整个测试文件夹及其里面的内容
    fs::remove_all(dir_path);
    std::cout << "\n测试目录已清理完毕。" << std::endl;

    return 0;
}
```