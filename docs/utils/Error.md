## BasicError

### 职责概览
`BasicError` 是一个轻量级的错误类型封装，基于 `std::runtime_error`，用于在解释器内部传递带有描述信息的异常。它提供一个简洁的接口来获取原始错误消息。

### 关键接口
```cpp
class BasicError : public std::runtime_error {
public:
    explicit BasicError(std::string message);
    const std::string& message() const noexcept;
};
```

- 构造函数接收一条错误描述，并在异常被捕获时通过 `what()` 或 `message()` 再次访问。
- `message()` 返回构造时保存的字符串引用，便于与现有日志或提示系统整合。

### 使用示例
```cpp
throw BasicError("Invalid line number");
```

### 最小测试建议
- 创建并捕获 `BasicError`，验证 `message()` 与 `what()` 输出是否匹配原始文本。
- 确认异常在 `catch (const std::runtime_error&)` 块中同样可被捕获。
