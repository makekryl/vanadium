#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <vector>

namespace vanadium::runtime {

template <std::size_t Size>
class StaticArena {
 public:
  StaticArena() : cursor_(buf_.begin()) {
    stack_.reserve(32);
  }

  StaticArena(const StaticArena&) = delete;
  StaticArena(StaticArena&&) = delete;
  StaticArena& operator=(const StaticArena&) = delete;
  StaticArena& operator=(StaticArena&&) = delete;

  void* Alloc(std::size_t bytes, std::size_t alignment) {
    void* ptr = cursor_;
    std::size_t space = buf_.end() - cursor_;

    if (std::align(alignment, bytes, ptr, space)) {
      cursor_ = static_cast<std::byte*>(ptr) + bytes;
      return ptr;
    }

    return nullptr;
  }

  [[nodiscard]] std::size_t RemainingSpace() {
    return buf_.end() - cursor_;
  }

  void Mark() {
    stack_.push_back(cursor_);
  }
  void Sweep() {
    cursor_ = stack_.back();
    stack_.pop_back();
  }

 private:
  std::byte* cursor_;
  std::vector<std::byte*> stack_;
  std::array<std::byte, Size> buf_;
};

}  // namespace vanadium::runtime
