### Expression 类设计

`Expression` 类用于表示和计算表达式。表达式可以是整数常量、变量或二元运算。我们使用树结构来表示表达式，其中每个节点都是一个 `Expression` 对象。

Expression 类的继承关系如下：

```
Expression (抽象基类)
├── ConstExpression (表示整数常量)
├── VariableExpression (表示变量)
└── CompoundExpression (表示二元运算)
```

#### Expression 类
```cpp
class Expression {
public:
    virtual ~Expression() = default;
    
    // 计算表达式的值
    virtual int evaluate(const VarState& state) const = 0; 
};
```

#### ConstExpression 类
```cpp
class ConstExpression : public Expression {
private:
    int value_; // 整数常量的值
public:
    explicit ConstExpression(int value);
    ~ConstExpression() = default;
    int evaluate(const VarState& state) const override;
}
```

#### VariableExpression 类
```cpp
class VariableExpression : public Expression {
private:
    std::string name_; // 变量名
public:
    explicit VariableExpression(std::string name);
    ~VariableExpression() = default;
    int evaluate(const VarState& state) const override;
}
```

#### CompoundExpression 类
```cpp
class CompoundExpression : public Expression {
private:
    Expression *left_; // 左子表达式
    Expression *right_; // 右子表达式
    char op_; // 运算符（+、-、*、/）
public:
    CompoundExpression(Expression *left, char op, Expression *right);
    ~CompoundExpression();
    int evaluate(const VarState& state) const;
}
```