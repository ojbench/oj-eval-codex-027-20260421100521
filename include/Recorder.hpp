#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Statement.hpp"

class Recorder {
 public:
  ~Recorder();

  void add(int line, Statement* stmt);
  void remove(int line);
  const Statement* get(int line) const noexcept;
  bool hasLine(int line) const noexcept;
  void clear() noexcept;
  void printLines() const;
  int nextLine(int line) const noexcept;

 private:
  // TODO.
};
