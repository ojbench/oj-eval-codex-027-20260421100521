## Program 模块

### 职责概览

Program 作为解释器的运行核心，负责：

- 接收 `Parser` 解析出的语句并存入 `Recorder`；
- 提供 `run / list / clear` 三个接口给 `main`；
- 在执行期间维护程序计数器（PC）与变量状态 (`VarState`)；
- 捕获语句执行产生的运行时错误并终止当前 RUN。


### 依赖模块

- `Recorder`：保存非立即执行的语句集合，Program 在 RUN/LIST/CLEAR 时调用。
- `VarState`：保存变量表，RUN 时读写，CLEAR 时重置。
- `Statement`：通过多态接口执行语句。
- `Expression`：通过多态接口求表达式的值。

### 数据结构与核心接口

```cpp
class Program {
public:
	Program();

	// 添加一行程序。
	void addStmt(int line, Statement* stmt); 

	// 删除行号对应的语句。
	void removeStmt(int line);                 

	// 按行号升序执行程序，支持 GOTO/IF 改变 PC。
	void run(); 

	// 输出 `<line> <stmt>`。
	void list() const; 

	// 清空 Recorder 与 VarState
	void clear(); 

	// 执行一条语句
	void execute(Statement* stmt); 

	// 获取当前行号，RUN 期间有效
	int getPC() const noexcept; 

	// 强制改变 PC，用于 GOTO/IF
	void changePC(int line); 

	void programEnd();

private:
	Recorder recorder_;
	VarState vars_;
	// 当前行号；RUN 前设为最小行
	int programCounter_; 
	bool programEnd_;

	void resetAfterRun() noexcept;
};
```