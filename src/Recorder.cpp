#include "Recorder.hpp"

#include <iostream>

#include "Statement.hpp"

Recorder::~Recorder() = default;

void Recorder::add(int line, Statement* stmt) {
  lines_[line].reset(stmt);
}

void Recorder::remove(int line) { lines_.erase(line); }

const Statement* Recorder::get(int line) const noexcept {
  auto it = lines_.find(line);
  if (it == lines_.end()) return nullptr;
  return it->second.get();
}

bool Recorder::hasLine(int line) const noexcept {
  return lines_.find(line) != lines_.end();
}

void Recorder::clear() noexcept { lines_.clear(); }

void Recorder::printLines() const {
  for (const auto& kv : lines_) {
    std::cout << kv.first << " " << kv.second->text() << "\n";
  }
}

int Recorder::nextLine(int line) const noexcept {
  auto it = lines_.upper_bound(line);
  if (it == lines_.end()) return -1;
  return it->first;
}
