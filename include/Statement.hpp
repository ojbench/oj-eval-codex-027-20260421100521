#pragma once

#include <memory>
#include <string>

#include "Expression.hpp"

class Program;
class VarState;

class Statement {
 public:
  explicit Statement(std::string source);
  virtual ~Statement() = default;

  virtual void execute(VarState& state, Program& program) const = 0;

  const std::string& text() const noexcept;

 private:
  std::string source_;
};

// Factory functions for concrete statements
class Expression;
Statement* makeLet(std::string var, Expression* expr, std::string source);
Statement* makePrint(Expression* expr, std::string source);
Statement* makeInput(std::string var, std::string source);
Statement* makeGoto(int line, std::string source);
Statement* makeIf(Expression* left, char op, Expression* right, int target,
                  std::string source);
Statement* makeRem(std::string text);
Statement* makeEnd();
