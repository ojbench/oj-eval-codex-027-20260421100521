## Recorder 模块

### 职责概览

Recorder 负责维护解释器中“带行号的程序”，负责：
- 用行号索引存储语句对象，保证插入、覆盖与删除符合 BASIC 语义；
- 为 `Program` 提供按行号升序遍历能力，支撑 `RUN` 与 `LIST`；
- 提供清空接口，在 `CLEAR` 时清空状态。


### 依赖模块

- `Statement`：以不透明指针形式存放， Recorder 不关心其内部结构。

### 数据结构与核心接口

```cpp
class Recorder {
public:
	~Recorder();

	// 插入或覆盖指定行。
	void add(int line, Statement *stmt);

	// 删除行，不存在则无事发生。
	void remove(int line);   

 	// 读取行号对应 Stmt ，不存在则返回 nullptr。
 	const Statement* get(int line) const noexcept;
	
	// 询问行号对应 Stmt 是否存在。
	bool hasLine(int line) const noexcept;

	// 清空全部行。
	void clear() noexcept;
	
	// 按行号升序输出所有程序。
	void printLines() const; 
	
	// 返回大于 line 的最小行号，不存在则返回 -1。
	int nextLine(int line) const noexcept; 

private:
	// TODO.
};
```


### 与其他模块交互

- `Program`：唯一的直接调用者，负责驱动 Recorder 添加、删除、查找、遍历 Stmt；同时借助 Recorder 实现程序的输出、清除、执行等。