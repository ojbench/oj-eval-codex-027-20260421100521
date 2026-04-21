#include "VarState.hpp"

#include <algorithm>

#include "utils/Error.hpp"

void VarState::setValue(const std::string& name, int value) {
  if (scopes_.empty()) scopes_.push_back({});
  scopes_.back()[name] = value;
}

int VarState::getValue(const std::string& name) const {
  for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
    auto f = it->find(name);
    if (f != it->end()) return f->second;
  }
  throw BasicError("VARIABLE NOT DEFINED");
}

void VarState::clear() { scopes_.assign(1, {}); }

void VarState::pushScope() { scopes_.push_back({}); }

void VarState::popScope() {
  if (scopes_.size() <= 1) {
    throw BasicError("SCOPE UNDERFLOW");
  }
  scopes_.pop_back();
}
