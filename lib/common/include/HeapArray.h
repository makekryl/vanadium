#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>

template <typename T>
class HeapArray {
 public:
  explicit HeapArray(std::size_t size) : size_(size), ptr_(new T[size]()) {}
  HeapArray(const HeapArray& other) : size_(other.size_), ptr_(new T[other.size_]) {
    std::copy_n(other.ptr_, other.size_, ptr_);
  }
  HeapArray(HeapArray&& other) noexcept : size_(other.size_), ptr_(std::exchange(other.ptr_, nullptr)) {}

  ~HeapArray() {
    delete[] ptr_;
  }

  HeapArray& operator=(HeapArray other) {
    HeapArray tmp(std::move(other));
    swap(*this, tmp);
    return *this;
  }

  HeapArray& operator=(HeapArray&& other) noexcept {
    swap(*this, other);
    return *this;
  }

  const T& operator[](std::size_t i) const {
    return ptr_[i];
  }
  T& operator[](std::size_t i) {
    return const_cast<T&>(static_cast<const HeapArray*>(this)->operator[](i));
  }

  [[nodiscard]] std::size_t Size() const {
    return size_;
  }

  friend void swap(HeapArray& lhs, HeapArray& rhs) noexcept {
    std::swap(lhs.size_, rhs.size_);
    std::swap(lhs.ptr_, rhs.ptr_);
  }

 private:
  std::size_t size_;
  T* ptr_;
};
