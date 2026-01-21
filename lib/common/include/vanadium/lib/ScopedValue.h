#pragma once

#include <utility>

namespace vanadium::lib {
template <typename T>
class ScopedValue {
 public:
  ScopedValue(T& place, T new_value) : place_(place), previous_value_(std::exchange(place, std::move(new_value))) {}

  ~ScopedValue() {
    place_ = std::move(previous_value_);
  }

  ScopedValue(const ScopedValue&) = delete;
  ScopedValue(ScopedValue&&) = delete;
  ScopedValue& operator=(const ScopedValue&) = delete;
  ScopedValue& operator=(ScopedValue&&) = delete;

 private:
  T& place_;
  T previous_value_;
};
}  // namespace vanadium::lib
