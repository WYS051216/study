# C++17 现代特性笔记（四）：`<charconv>` 高性能字符转换

---

## 十四、 极致性能的字符转换 `<charconv>`

### 1. 核心优势与概念补充
`<charconv>` 是 C++17 新增的标准库头文件，主要用于**字符串与整数/浮点数之间的双向转换**。
相比于传统的 `atoi`、`to_string`、`sprintf` 或 `stringstream`，它拥有极其严苛的性能追求：
* **不分配内存**：完全在用户提供的已有缓冲区（Buffer）上进行原地操作。
* **不抛出异常**：遇到错误时不抛出任何异常，而是通过返回错误码（`std::errc`）让开发者自行处理。
* **独立于本地环境 (Locale-independent)**：不受操作系统的语言/地区设置影响（例如某些国家小数点用逗号 `,`），永远采用标准格式，极其适合 JSON/XML 等标准化文本的网络传输。

---

### 2. 格式控制枚举：`std::chars_format`

**概念**：
`std::chars_format` 是定义在 `<charconv>` 中的一个**枚举类（enum class）**，专门用来精细控制**浮点数**转换时的格式，或者指定按哪种基底（如 16 进制）来解析。
它主要包含以下几种标志（可以通过按位或 `|` 组合使用）：
* `scientific`：科学计数法（例如 `3.14e+00`）
* `fixed`：定点表示法（例如 `3.14`）
* `hex`：十六进制表示法
* `general`：根据数值大小，编译器自动在 `fixed` 和 `scientific` 之间选择最简短的形式。

**代码示例**：
```cpp
#include <iostream>
#include <charconv>
#include <array>
#include <system_error>

int main() {
    std::array<char, 50> buffer;
    double pi = 3.1415926;

    // 使用 chars_format::scientific 强制以科学计数法转换为字符串
    auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), 
                                   pi, std::chars_format::scientific);
    
    if (ec == std::errc()) {
        *ptr = '\0'; // 添加字符串结束符以便打印
        std::cout << "科学计数法格式: " << buffer.data() << std::endl;
    }
    return 0;
}
```

---

### 3. 字符串转数字：`std::from_chars`

**概念**：
将一段字符缓冲区（不可变）中的文本，高速解析为数字。
* **返回值**：它返回一个 `std::from_chars_result` 结构体，里面包含两个成员：
  1. `ptr`：指向第一个**未能被解析**的字符的指针（方便你知道解析到哪里停止了）。
  2. `ec`：错误码。如果转换成功，`ec == std::errc()`；如果溢出则是 `std::errc::result_out_of_range`；如果格式不对则是 `std::errc::invalid_argument`。

**代码示例**：
```cpp
#include <iostream>
#include <charconv>
#include <string_view>

int main() {
    std::string_view str = "2026is the year";
    int result = 0;

    // 将前缀的 "2026" 解析为整数，遇到 'i' 停止
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);

    if (ec == std::errc()) {
        std::cout << "成功解析出数字: " << result << std::endl;
        std::cout << "未解析的剩余部分: " << ptr << std::endl; // 输出 "is the year"
    } else {
        std::cout << "解析失败！" << std::endl;
    }

    return 0;
}
```

---

### 4. 数字转字符串：`std::to_chars`

**概念**：
将数字高速格式化并写入到你提前准备好的字符数组（Buffer）中。
* **返回值**：它返回一个 `std::to_chars_result` 结构体，同样包含两个成员：
  1. `ptr`：指向写入完成后的**最后一个字符的下一个位置**（通常可以直接用来做字符串结尾符 `\0` 的填充）。
  2. `ec`：错误码。如果提供的缓冲区太小装不下，会返回 `std::errc::value_too_large`。

**代码示例**：
```cpp
#include <iostream>
#include <charconv>
#include <array>
#include <string_view>

int main() {
    std::array<char, 20> buffer; // 提前分配好一块足够大的内存
    int secret_code = 1024;

    // 将整数转换为字符串写入 buffer，并指定使用 16 进制 (基底为 16)
    auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), 
                                   secret_code, 16);

    if (ec == std::errc()) {
        // 利用返回的 ptr 构造一个 string_view 来安全打印，不需要手动加 '\0'
        std::string_view result_str(buffer.data(), ptr - buffer.data());
        std::cout << "十进制 1024 的十六进制字符串为: " << result_str << std::endl; 
        // 预期输出: 400
    }

    return 0;
}
```