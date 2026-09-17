# Python 文件操作

## 1. 打开文件

Python 使用 `open()` 函数打开文件，其基本语法为：

```python
文件对象 = open(name, mode, encoding)
```

参数说明：

- `name`：需要打开的文件名称或文件路径。
- `mode`：文件的打开模式，例如只读、写入或追加。
- `encoding`：文件的编码格式，通常使用 `utf-8`。

示例：

```python
f = open("test.txt", "r", encoding="utf-8")
```

该语句以只读方式打开 `test.txt`，并将返回的文件对象赋值给变量 `f`。

## 2. 文件打开模式

| 模式 | 含义 | 文件不存在 | 文件已存在 |
|---|---|---|---|
| `r` | 只读模式 | 报错 | 从头读取 |
| `w` | 写入模式 | 创建文件 | 清空原内容后写入 |
| `a` | 追加模式 | 创建文件 | 在文件末尾追加 |
| `x` | 创建模式 | 创建文件 | 报错 |
| `b` | 二进制模式 | 根据组合模式决定 | 用于图片、音频等文件 |
| `+` | 读写模式 | 根据组合模式决定 | 同时支持读取和写入 |

常用组合模式：

```python
"rb"   # 以二进制方式读取
"wb"   # 以二进制方式写入
"r+"   # 同时支持读取和写入
"a+"   # 同时支持读取和追加
```

## 3. 读取文件

### 3.1 `read()` 方法

`read()` 用于读取文件内容：

```python
内容 = f.read()
```

示例：

```python
f = open("test.txt", "r", encoding="utf-8")

content = f.read()
print(content)

f.close()
```

`read()` 也可以指定读取的字符数量：

```python
content = f.read(10)
```

这表示读取接下来的 10 个字符。

注意：在文本模式下，参数表示读取的字符数量；在二进制模式下，参数表示读取的字节数量。

### 3.2 `readline()` 方法

`readline()` 每次读取一行内容：

```python
f = open("test.txt", "r", encoding="utf-8")

line = f.readline()
print(line)

f.close()
```

连续调用可以继续读取下一行：

```python
first_line = f.readline()
second_line = f.readline()
```

### 3.3 `readlines()` 方法

`readlines()` 会一次性读取文件中的所有行，并返回一个列表：

```python
f = open("test.txt", "r", encoding="utf-8")

lines = f.readlines()
print(lines)

f.close()
```

假设文件内容为：

```text
hello
python
world
```

返回结果类似于：

```python
["hello\n", "python\n", "world\n"]
```

列表中的每个元素对应文件中的一行，行末通常包含换行符 `\n`。

当文件较大时，不建议使用 `readlines()` 一次性读取全部内容，因为这会占用较多内存。

### 3.4 使用 `for` 循环逐行读取

可以直接遍历文件对象，逐行读取文件：

```python
f = open("test.txt", "r", encoding="utf-8")

for line in f:
    print(line, end="")

f.close()
```

其中：

- `f` 表示文件对象。
- `line` 表示当前读取的一行内容。
- `end=""` 用于避免 `print()` 再次添加换行。

这种方法不会一次性将全部内容加载到内存中，更适合读取较大的文件。

## 4. 关闭文件

文件使用完毕后，应调用 `close()` 方法关闭文件：

```python
f = open("test.txt", "r", encoding="utf-8")

content = f.read()
print(content)

f.close()
```

关闭文件可以释放程序占用的文件资源，并保证尚未写入的数据得到妥善处理。

## 5. `with open` 语法

推荐使用 `with open` 管理文件：

```python
with open("test.txt", "r", encoding="utf-8") as f:
    content = f.read()
    print(content)
```

当 `with` 代码块执行结束后，Python 会自动关闭文件，不需要手动调用：

```python
f.close()
```

即使读取或写入过程中出现异常，文件通常也能被正确关闭，因此 `with open` 是更加安全、规范的文件操作方式。

## 6. 写入文件

### 6.1 `w` 写入模式

使用 `w` 模式打开文件：

```python
with open("test.txt", "w", encoding="utf-8") as f:
    f.write("Hello Python")
```

注意：

- 文件不存在时，会自动创建文件。
- 文件已经存在时，会先清空原有内容，再写入新内容。

例如，原文件内容为：

```text
原来的内容
```

执行下面的代码：

```python
with open("test.txt", "w", encoding="utf-8") as f:
    f.write("新的内容")
```

执行后，文件内容变为：

```text
新的内容
```

原来的内容会被清空。

### 6.2 `write()` 方法

`write()` 用于向文件中写入字符串：

```python
f.write("要写入的内容")
```

写入多行内容时，需要手动添加换行符 `\n`：

```python
with open("test.txt", "w", encoding="utf-8") as f:
    f.write("第一行\n")
    f.write("第二行\n")
    f.write("第三行\n")
```

### 6.3 `writelines()` 方法

`writelines()` 可以将一个字符串序列写入文件：

```python
lines = ["第一行\n", "第二行\n", "第三行\n"]

with open("test.txt", "w", encoding="utf-8") as f:
    f.writelines(lines)
```

`writelines()` 不会自动添加换行符，因此需要在每个字符串末尾手动添加 `\n`。

## 7. 文件缓冲区与 `flush()`

调用 `write()` 后，数据通常会先进入程序的文件缓冲区，不一定立即写入文件。

```python
f = open("test.txt", "w", encoding="utf-8")

f.write("Hello Python")
f.flush()
```

`flush()` 用于刷新缓冲区，将缓冲区中的内容交给操作系统处理。

调用 `close()` 时，也会自动刷新缓冲区：

```python
f.close()
```

使用 `with open` 时，代码块结束后会自动关闭文件并处理缓冲区，因此一般不需要手动调用 `flush()`：

```python
with open("test.txt", "w", encoding="utf-8") as f:
    f.write("Hello Python")
```

## 8. 追加文件

使用 `a` 模式可以在文件末尾追加内容：

```python
with open("test.txt", "a", encoding="utf-8") as f:
    f.write("追加的内容\n")
```

`a` 模式的特点：

- 文件不存在时，会自动创建文件。
- 文件已经存在时，不会清空原内容。
- 新内容会被写入文件末尾。

假设原文件内容为：

```text
第一行
```

执行：

```python
with open("test.txt", "a", encoding="utf-8") as f:
    f.write("第二行\n")
```

执行后文件内容为：

```text
第一行
第二行
```

## 9. 文件中字符串出现次数的统计

可以先读取文件内容，再使用字符串的 `count()` 方法统计某个字符串出现的次数：

```python
with open("111.txt", "r", encoding="utf-8") as f:
    content = f.read()

count = content.count("itheima")
print(count)
```

注意：

```python
f.count("itheima")
```

是错误的，因为 `f` 是文件对象，文件对象没有 `count()` 方法。

正确方式是：

```python
content = f.read()
content.count("itheima")
```

## 10. 推荐写法总结

### 读取整个文件

```python
with open("test.txt", "r", encoding="utf-8") as f:
    content = f.read()

print(content)
```

### 逐行读取文件

```python
with open("test.txt", "r", encoding="utf-8") as f:
    for line in f:
        print(line, end="")
```

### 覆盖写入文件

```python
with open("test.txt", "w", encoding="utf-8") as f:
    f.write("新的内容\n")
```

### 追加写入文件

```python
with open("test.txt", "a", encoding="utf-8") as f:
    f.write("追加的内容\n")
```

## 11. 注意事项

1. 使用 `r` 模式时，如果文件不存在，程序会报错。
2. 使用 `w` 模式时，原文件内容会被清空。
3. 使用 `a` 模式时，新内容会追加到文件末尾。
4. 读写文本文件时，建议明确指定 `encoding="utf-8"`。
5. 推荐使用 `with open`，让 Python 自动关闭文件。
6. `read()` 读取的是文件内容，`open()` 返回的是文件对象。
7. 文件对象本身不能直接调用字符串的 `count()` 等方法。
8. 读取大文件时，推荐使用 `for line in f` 逐行读取。