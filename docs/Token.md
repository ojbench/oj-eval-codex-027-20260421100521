## Token 模块设计

### 职责概览

Token 系列定义词法分析阶段的统一数据结构，供 `Lexer` 生成、`Parser` 与 `Program` 消费。主要组成：

- `TokenType`：枚举所有词法类型；
- `Token`：封装类型与原始文本；
- `TokenStream`：顺序访问 token 的轻量容器。

### TokenType

```cpp
enum class TokenType {
    // 关键字
    LET, PRINT, INPUT, END, REM, GOTO, IF, THEN,
    RUN, LIST, CLEAR, QUIT, HELP,
    // 基础语法单元
    IDENTIFIER, NUMBER, REMINFO,
    // 运算与符号
    PLUS, MINUS, MUL, DIV,
    EQUAL, GREATER, LESS,
    LEFT_PAREN, RIGHT_PAREN,
    COMMA,
    // 保留占位
    UNKNOWN
};
```

### Token

```cpp
struct Token {
    TokenType type;
    std::string text;   // 源代码片段
    std::size_t column; // 可选：在原行中的列号，用于错误定位
};
```
- `text` 保留原始大小写，便于错误提示或 `LIST` 回显；
- `column` 可选，若不需要精确定位可省略。

### TokenStream

```cpp
class TokenStream {
public:
    const Token* peek() const;      // 查看当前 token，未结束则返回指针
    const Token* get();             // 取出并前进
    bool empty() const;             // 是否耗尽
    void reset();                   // 游标回到起点

    std::size_t position() const;   // 返回当前游标位置
    std::size_t size() const;       // token 总数

private:
    std::vector<Token> tokens;
    std::size_t cursor {0};
};
```

- TokenStream 不修改 token 内容，仅负责游标管理。

### 与其他模块交互

- `Lexer`：构造 Token 和 TokenStream；
- `Parser` ：消费 TokenStream，解析语句/表达式；
- 解释器主循环：消费TokenStream 进行逻辑的分流；

