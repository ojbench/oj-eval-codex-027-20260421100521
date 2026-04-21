## Parser 模块

### 职责概览
Parser 接收 `Lexer` 生成的 `TokenStream`，并将词法单元转换为语句 (`Statement`) 与表达式 (`Expression`) 对象，关键点如下：
- 区分带行号的程序语句与立即执行命令；
- 解析 `LET`、`PRINT`、`INPUT`、`GOTO`、`IF`、`REM`、`END` 开头的输入并构造**对应的Statement**；
- 构建算术表达式树，支持变量、整数常量与二元运算；
- 返回解析结果。

### 依赖关系
- `Token` 模块中的 `TokenType`、`Token`、`TokenStream`；
- `Statement` 派生类构造函数（例如 `LetStmt`、`IfStmt`）；
- `Expression` 派生类及其构造函数；
- 标准库 `<memory>`、`<optional>`、`<stdexcept>`。

### 公共接口
```cpp
class ParsedLine {
private:
    std::optional<int> line_number_;
    Statement *statement_;

public:
    ParsedLine();
    ~ParsedLine();

    void setLine(int line);
    std::optional<int> getLine();
    void setStatement(Statement *stmt);
    Statement *getStatement() const;
    Statement *fetchStatement();
};

class Parser {
public:
    ParsedLine parseLine(TokenStream& tokens, const std::string& originLine) const;            // 解析整行输入

private:
    // 语句解析分发：TokenStream 依旧通过引用传递，内部获取 Token 时使用指针
    Statement *parseStatement(TokenStream& tokens, const std::string& originLine) const;
    Statement *parseLet(TokenStream& tokens, const std::string& originLine) const;
    Statement *parsePrint(TokenStream& tokens, const std::string& originLine) const;
    Statement *parseInput(TokenStream& tokens, const std::string& originLine) const;
    Statement *parseGoto(TokenStream& tokens, const std::string& originLine) const;
    Statement *parseIf(TokenStream& tokens, const std::string& originLine) const;
    Statement *parseRem(TokenStream& tokens, const std::string& originLine) const;
    Statement *parseEnd(TokenStream& tokens, const std::string& originLine) const;

    // 表达式解析
    Expression *parseExpression(TokenStream& tokens) const;
    Expression *parseExpression(TokenStream& tokens, int precedence) const;

    int getPrecedence(TokenType op) const;
    int parseLiteral(const Token* token) const;

    mutable int leftParentCount{0};  // 用于括号匹配检测
};
```
- 当 `line_number_` 存在且 `statement_ == nullptr` 时表示"删除该行"，反之代表一个立即执行指令。

### 接口实现

#### parseLine() 实现
1. **行号判定**：首 token 为 `NUMBER` 时解析为行号，记录在 `lineNumber`。若后续无 token，表示删除对应行。
2. **语句解析**：调用 `parseStatement` 解析剩余 token，结合传入的源代码字符串生成对应 `Statement` 对象。
3. **返回结果**：将 `lineNumber` 与 `statement` 封装在 `ParsedLine` 结构体中返回。

#### parseStatement() 实现
整体使用递归下降的思路，根据首 token 关键字分派至对应的解析函数，具体步骤如下：
1. **关键字分派**：读取下一个 token 判断语句类型，不匹配任意一条支持规则则抛出 `BasicError("SYNTAX ERROR")`。
2. **参数解析，函数调用**：
   - `LET`：变量名 → `=` → 表达式；
   - `PRINT`：单表达式；
   - `INPUT`：单变量名；
   - `GOTO`：行号；
   - `IF`：表达式 → 比较符 → 表达式 → `THEN` → 行号；
   - `REM`：剩余 token 拼接为注释；
   - `END`：无参数。
  同样的，如果遇到Token不匹配规则的情况立即抛出错误。
3. **终结校验**：解析完成后若仍有剩余 token，抛出 `BasicError("SYNTAX ERROR")`。

#### parseExpression() 实现
- 无参版本调用有参版本，初始优先级设为 0。
- 采用递归下降 + 优先级爬升：
  1. 读取左操作数：数字 → `ConstExpression`，标识符 → `VariableExpression`，左括号 → 递归解析，同时将括号层数加一；
  2. 查看下一个 token 是否为运算符（`+ - * /`），依据 `getPrecedence` 判断是否展开；如果是右括号检测是否与左括号匹配，若是则终止解析，同时消费该右括号，括号层数减一；若否则报错；
  3. 满足条件则消费运算符，解析右操作数（带更高优先级），生成 `CompoundExpression`；
  4. 重复直到遇到更低优先级运算符或流结束。
  5. 解析出表达式后若括号层数不为零，抛出 `BasicError("MISMATCHED PARENTHESIS")`。
- `parseLiteral` 将数字 token 文本转换为 `int`，内部先调用一个轻量的范围检查（仅依赖标准库，避免额外状态），若超出 32 位有符号整型范围则抛出`BasicError("INT LITERAL OVERFLOW")`。

### 测试关键点
- Parser 抛出 `BasicError`，包含错误信息与位置（依赖 `Token.column`）；
- 常见错误：缺少关键字或符号（如 `=`、`THEN`）、行号/常量格式不合法、IF 比较符无效；
- 由 Parser 统一负责整数字面量的溢出判定，避免词法阶段重复逻辑；
- `IF X < Y THEN 100` 生成 `IfStmt`，目标行号正确；
- `10 REM comment` 返回 `lineNumber=10` 且语句为 `RemStmt`；
- 单独 `10` 表示删除；
- 无效语句（如缺参数的 `GOTO`）抛出 `BasicError`；
- 表达式括号嵌套与空白处理。

### 待确认问题
1. 比较符是否允许在表达式中出现，还是仅限 IF？