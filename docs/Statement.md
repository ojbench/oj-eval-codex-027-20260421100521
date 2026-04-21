### Statement 类设计

`Statement` 类是项目中用于表示和处理各种语句的核心类。它定义了所有支持的语句类型的基类和派生类，每个派生类对应一种具体的语句类型，封装了该类型语句的相关数据和行为。

`Statement` 类的继承结构如下：
```cpp
// TODO
```

每个派生类都实现了基类 `Statement` 中的纯虚函数 `execute(VarState &varState, Program &program)`，用于执行该语句的具体逻辑。