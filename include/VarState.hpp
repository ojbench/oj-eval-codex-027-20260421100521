#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class VarState {
 public:
  void setValue(const std::string& name, int value);
  int getValue(const std::string& name) const;
  void clear();
  // scope control
  void pushScope();
  void popScope();

 private:
  std::vector<std::unordered_map<std::string, int>> scopes_{{}}; // start with global
};
