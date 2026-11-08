#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>

namespace vanadium::lib {

template <typename T>
class HeapArray {
 public:
  explicit HeapArray(std::size_t size) : size_(size), ptr_(std::make_unique<T[]>(size)) {}

  HeapArray(const HeapArray& other) : size_(other.size_), ptr_(std::make_unique_for_overwrite<T[]>(other.size_)) {
    std::copy_n(other.ptr_.get(), other.size_, ptr_.get());
  }

  HeapArray(HeapArray&&) noexcept = default;
  HeapArray& operator=(HeapArray&&) noexcept = default;

  HeapArray& operator=(const HeapArray& other) {
    if (this != &other) {
      auto tmp = std::make_unique<T[]>(other.size_);
      std::copy_n(other.ptr_.get(), other.size_, tmp.get());
      ptr_ = std::move(tmp);
      size_ = other.size_;
    }
    return *this;
  }

  // Destructor is implicitly defined correctly by unique_ptr
  ~HeapArray() = default;

  const T& operator[](std::size_t i) const {
    return ptr_[i];
  }
  T& operator[](std::size_t i) {
    return ptr_[i];
  }

  [[nodiscard]] std::size_t size() const {
    return size_;
  }

  void fill(const T& value) {
    std::fill_n(ptr_.get(), size_, value);
  }

 private:
  std::size_t size_;
  std::unique_ptr<T[]> ptr_;
};

}  // namespace vanadium::lib
