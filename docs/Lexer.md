## Lexer 模块

### 职责概览
Lexer 负责把一行 BASIC 源码转换成 `TokenStream`，供 `Parser` 和解释器主循环消费。核心职责：
- 按字符扫描输入行，识别关键字、标识符、数字、运算符与括号；
- 生成 `Token` 序列并封装为 `TokenStream`；
- 遇到非法字符时抛出词法错误。

### 依赖模块
- `Token.md` 中的 `TokenType`、`Token`、`TokenStream`；
- 标准库 `<string>`、`<vector>`、`<optional>`；
- 与 `Program`、`Statement`、`Expression` 无直接耦合，仅与 `Parser` 交互。

### 核心接口
```cpp
class Lexer {
public:
    TokenStream tokenize(const std::string& line) const;
};
```
- 返回的 `TokenStream` 拥有独立数据拷贝，可在 Parser 中安全保存或回溯。

### tokenize 流程
1. 逐字符读取输入，跳过空白字符（空格、Tab）。
2. 识别字母开头的片，区分大小写：
   - 与关键字表比对生成关键字 `Token`；
   - 否则生成 `IDENTIFIER`。
3. 识别数字序列生成 `NUMBER`。
4. 识别单字符符号：`+ - * / = < > ( ) ,` 等，映射至相应 `TokenType`。
5. 若遇 `REM`，立即将余下文本作为单一 `REM` Token；
6. 遇到无法识别的字符，抛出错误，包含原始字符与列号。
7. 将所有 Token 推入 `TokenStream` 并返回。

