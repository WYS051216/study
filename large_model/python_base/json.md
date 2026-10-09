# JSON 与 Python `json` 标准库基础笔记

## 1. JSON 核心概念
- **本质**：JSON 是一种轻量级的文本数据交换格式。
- **特点**：完全独立于编程语言，具有极强的自我描述性，易于人阅读和编写，也易于机器解析和生成。
- **基本结构**：
  - **对象（类似于字典）**：使用大括号 `{}` 将要描述的东西框起来，内部使用 `"key": "value"`（键值对）来描述信息。**注意：JSON 中的字符串和键必须使用双引号。**
  - **数组（类似于列表）**：使用中括号 `[]` 可以将多个独立的值或键值对（对象）括起来形成列表。

## 2. Python `json` 标准库常用方法
在 Python 中，导入 `json` 模块后即可实现 JSON 数据与 Python 对象之间的相互转换。

| 方法 | 作用 | 核心记忆 |
| :--- | :--- | :--- |
| `json.loads(text)` | 字符串转化为对象 | **s** 代表 **s**tring，从 JSON **字符串**解析为 Python 对象（如 dict） |
| `json.dumps(obj)` | 对象转化为字符串 | **s** 代表 **s**tring，将 Python 对象转化为 JSON **字符串** |
| `json.load(file)` | 文件读取为对象 | 操作对象是**文件（file）**，从文件中读取 JSON 并转化为 Python 对象 |
| `json.dump(obj, file)`| 对象写入到文件 | 操作对象是**文件（file）**，将 Python 对象转换为 JSON 并直接写入文件 |

## 3. JSON 与 Python 数据类型对照表
在进行数据转换时，JSON 的数据类型会自动与 Python 的数据类型进行映射匹配：

| JSON 类型 | Python 类型 | 对比示例 |
| :--- | :--- | :--- |
| **Object** `{...}` | **dict** (字典) | `{"name": "Tom"}` ↔ `{'name': 'Tom'}` |
| **Array** `[...]` | **list** (列表) | `[1, 2, 3]` ↔ `[1, 2, 3]` |
| **String** `"abc"` | **str** (字符串) | `"abc"` ↔ `'abc'` |
| **Number** `123` | **int/float** (数字) | `123` ↔ `123` |
| **Boolean** `true/false`| **bool** (布尔值) | `true` / `false` ↔ `True` / `False` (**注意大小写**) |
| **Null** `null` | **None** (空值) | `null` ↔ `None` |