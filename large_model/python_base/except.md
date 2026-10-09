# Python 异常处理核心笔记

## 1. 完整异常处理语法
Python 异常处理的完整结构包含 `try`、`except`、`else` 和 `finally` 四个部分：

```python
try:
    # 尝试执行的代码块（可能引发异常的代码）
    pass
except ExceptionType as e:
    # 捕获到指定异常时执行的处理逻辑
    # 变量 e 保存了异常实例对象，可通过 print(e) 查看具体错误信息
    pass
else:
    # 仅当 try 块中没有任何异常发生时，才会执行的代码（可选）
    pass
finally:
    # 无论是否发生异常，最终都会强制执行的代码（可选）
    # 极其适合用于资源的清理与释放工作（如：关闭文件、断开数据库连接等）
    pass
```

## 2. 异常捕获的进阶方式
Python 中几乎所有的标准常规异常都继承自 `Exception`（其父类是 `BaseException`）。
*   **捕获特定单个异常**：`except ValueError:`
*   **捕获多个指定的异常（合并处理）**：`except (ValueError, TypeError) as e:`
*   **捕获所有常规异常（兜底处理）**：`except Exception as e:`
*   **多分支捕获（按顺序匹配）**：
    ```python
    try:
        pass
    except ValueError:
        pass # 处理值错误
    except TypeError:
        pass # 处理类型错误
    except Exception as e:
        pass # 兜底处理其他所有错误
    ```

## 3. 异常的传递机制
**异常传递**是指异常在函数调用栈中层层上报的过程：
1. 当底层函数内部发生异常且没有被就地捕获（没有 `try-except`）时，异常会被直接抛给调用它的上层函数。
2. 如果上层函数依然没有捕获，该异常会继续向上抛出，直到抵达主程序（全局作用域）。
3. 如果一直传递到最顶层依然无人处理，Python 解释器就会接管，直接强行终止程序，并在控制台打印 Traceback（错误堆栈轨迹）。

## 4. 主动抛出异常 (`raise`)
除了程序运行出错自动触发异常外，开发者可以在满足某些特定业务条件时，使用 `raise` 关键字主动中断程序并抛出异常。
```python
def set_age(age):
    if age < 0 or age > 150:
        raise ValueError(f"提供的年龄 {age} 不合法！") # 强制触发异常并附带信息
    return age
```

## 5. 自定义异常
当内置的异常类型无法准确描述业务逻辑错误时（比如：余额不足、密码太弱），可以自定义异常类。**自定义异常类必须直接或间接地继承 `Exception` 类**。
```python
# 定义自定义异常
class BalanceNotEnoughError(Exception):
    def __init__(self, message):
        self.message = message

# 抛出自定义异常
raise BalanceNotEnoughError("支付失败，账户余额不足！")
```

## 6. Python 常见内置异常对照表
| 异常类型 | 触发场景与原因 |
| :--- | :--- |
| `ZeroDivisionError` | 尝试进行除法或取余运算时，除数为 `0` |
| `IndexError` | 试图访问序列（如列表、元组）中不存在的索引位置（越界） |
| `KeyError` | 试图在字典中访问或操作一个不存在的键（Key） |
| `ValueError` | 传入函数的参数类型正确，但具体的值不合法（例如 `int("abc")`） |
| `TypeError` | 对对象执行了其类型不支持的操作（例如 `1 + "2"`） |
| `FileNotFoundError` | 试图读取或打开一个不存在的文件路径 |
| `AttributeError` | 尝试访问某个对象不存在的属性或方法 |
| `NameError` | 尝试访问一个未声明/未初始化的局部或全局变量 |