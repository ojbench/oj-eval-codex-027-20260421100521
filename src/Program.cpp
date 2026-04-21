// Implement Program interfaces declared in Program.hpp
#include "Program.hpp"

#include <iostream>

#include "Recorder.hpp"
#include "Statement.hpp"
#include "utils/Error.hpp"

Program::Program() : programCounter_(0), programEnd_(false) {}

void Program::addStmt(int line, Statement* stmt) { recorder_.add(line, stmt); }

void Program::removeStmt(int line) { recorder_.remove(line); }

void Program::run() {
  // set PC to the first line
  programEnd_ = false;
  int pc = recorder_.nextLine(-1);
  programCounter_ = pc;
  while (pc != -1 && !programEnd_) {
    programCounter_ = pc;
    const Statement* stmt = recorder_.get(pc);
    if (!stmt) {
      // should not happen; skip to next line
      pc = recorder_.nextLine(pc);
      continue;
    }
    try {
      stmt->execute(vars_, *this);
    } catch (const BasicError&) {
      throw;  // propagate to main
    }
    if (!programEnd_) {
      // if PC unchanged by GOTO/IF, move to next line
      if (programCounter_ == pc) {
        pc = recorder_.nextLine(pc);
        programCounter_ = pc;
      } else {
        // programCounter_ might be set to any line; find next from it
        pc = programCounter_;
        if (pc != -1 && !recorder_.hasLine(pc)) {
          // if target line not present, continue to next higher line
          pc = recorder_.nextLine(pc);
        }
      }
    }
  }
  resetAfterRun();
}

void Program::list() const { recorder_.printLines(); }

void Program::clear() {
  recorder_.clear();
  vars_.clear();
  programCounter_ = 0;
  programEnd_ = false;
}

void Program::execute(Statement* stmt) {
  // Execute a single statement in immediate mode
  stmt->execute(vars_, *this);
}

int Program::getPC() const noexcept { return programCounter_; }

void Program::changePC(int line) {
  if (!recorder_.hasLine(line)) {
    throw BasicError("LINE NUMBER ERROR");
  }
  programCounter_ = line;
}

void Program::programEnd() { programEnd_ = true; }

void Program::resetAfterRun() noexcept {
  programCounter_ = 0;
  programEnd_ = false;
}
