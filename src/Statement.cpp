#include "Statement.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

#include "Program.hpp"
#include "VarState.hpp"
#include "utils/Error.hpp"

Statement::Statement(std::string source) : source_(std::move(source)) {}

const std::string& Statement::text() const noexcept { return source_; }

// Concrete statement implementations

class LetStatement : public Statement {
 public:
  LetStatement(std::string var, Expression* expr, std::string source)
      : Statement(std::move(source)), var_(std::move(var)), expr_(expr) {}
  ~LetStatement() override { delete expr_; }
  void execute(VarState& state, Program&) const override {
    int v = expr_->evaluate(state);
    state.setValue(var_, v);
  }

 private:
  std::string var_;
  Expression* expr_;
};

class PrintStatement : public Statement {
 public:
  PrintStatement(Expression* expr, std::string source)
      : Statement(std::move(source)), expr_(expr) {}
  ~PrintStatement() override { delete expr_; }
  void execute(VarState& state, Program&) const override {
    std::cout << expr_->evaluate(state) << "\n";
  }

 private:
  Expression* expr_;
};

class InputStatement : public Statement {
 public:
  InputStatement(std::string var, std::string source)
      : Statement(std::move(source)), var_(std::move(var)) {}
  void execute(VarState& state, Program&) const override {
    while (true) {
      // print prompt without newline and without echoing input
      std::cout << " ? ";
      std::cout.flush();
      std::string raw;
      if (!std::getline(std::cin, raw)) {
        std::cout << "INVALID NUMBER\n";
        continue;
      }
      // prepare trimmed copy for validation
      std::string line = raw;
      auto ltrim = [](std::string& s) {
        size_t i = 0;
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        s.erase(0, i);
      };
      auto rtrim = [](std::string& s) {
        size_t i = s.size();
        while (i > 0 && std::isspace(static_cast<unsigned char>(s[i - 1]))) --i;
        s.erase(i);
      };
      ltrim(line);
      rtrim(line);
      if (line.empty()) {
        std::cout << "INVALID NUMBER\n";
        continue;
      }
      bool neg = false;
      size_t pos = 0;
      if (line[0] == '+' || line[0] == '-') {
        neg = (line[0] == '-');
        pos = 1;
        if (pos >= line.size()) {
          std::cout << "INVALID NUMBER\n";
          continue;
        }
      }
      long long val = 0;
      bool ok = true;
      for (; pos < line.size(); ++pos) {
        if (!std::isdigit(static_cast<unsigned char>(line[pos]))) {
          ok = false;
          break;
        }
        int d = line[pos] - '0';
        val = val * 10 + d;
        if ((!neg && val > std::numeric_limits<int>::max()) ||
            (neg && -val < std::numeric_limits<int>::min())) {
          ok = false;
          break;
        }
      }
      if (!ok) {
        std::cout << "INVALID NUMBER\n";
        continue;
      }
      int iv = static_cast<int>(neg ? -val : val);
      state.setValue(var_, iv);
      break;
    }
  }

 private:
  std::string var_;
};

class GotoStatement : public Statement {
 public:
  GotoStatement(int line, std::string source)
      : Statement(std::move(source)), line_(line) {}
  void execute(VarState&, Program& program) const override {
    program.changePC(line_);
  }

 private:
  int line_;
};

class IfStatement : public Statement {
 public:
  IfStatement(Expression* left, char op, Expression* right, int target,
              std::string source)
      : Statement(std::move(source)),
        left_(left),
        right_(right),
        op_(op),
        target_(target) {}
  ~IfStatement() override {
    delete left_;
    delete right_;
  }
  void execute(VarState& state, Program& program) const override {
    int l = left_->evaluate(state);
    int r = right_->evaluate(state);
    bool cond = false;
    switch (op_) {
      case '=':
        cond = (l == r);
        break;
      case '<':
        cond = (l < r);
        break;
      case '>':
        cond = (l > r);
        break;
    }
    if (cond) program.changePC(target_);
  }

 private:
  Expression* left_;
  Expression* right_;
  char op_;
  int target_;
};

class RemStatement : public Statement {
 public:
  explicit RemStatement(std::string text) : Statement(std::move(text)) {}
  void execute(VarState&, Program&) const override {}
};

class EndStatement : public Statement {
 public:
  EndStatement() : Statement("END") {}
  void execute(VarState&, Program& program) const override {
    program.programEnd();
  }
};

class IndentStatement : public Statement {
 public:
  IndentStatement() : Statement("INDENT") {}
  void execute(VarState& state, Program&) const override { state.pushScope(); }
};

class DedentStatement : public Statement {
 public:
  DedentStatement() : Statement("DEDENT") {}
  void execute(VarState& state, Program&) const override { state.popScope(); }
};

// Factory function definitions
Statement* makeLet(std::string var, Expression* expr, std::string source) {
  return new LetStatement(std::move(var), expr, std::move(source));
}
Statement* makePrint(Expression* expr, std::string source) {
  return new PrintStatement(expr, std::move(source));
}
Statement* makeInput(std::string var, std::string source) {
  return new InputStatement(std::move(var), std::move(source));
}
Statement* makeGoto(int line, std::string source) {
  return new GotoStatement(line, std::move(source));
}
Statement* makeIf(Expression* left, char op, Expression* right, int target,
                  std::string source) {
  return new IfStatement(left, op, right, target, std::move(source));
}
Statement* makeRem(std::string text) { return new RemStatement(std::move(text)); }
Statement* makeEnd() { return new EndStatement(); }
Statement* makeIndent() { return new IndentStatement(); }
Statement* makeDedent() { return new DedentStatement(); }
