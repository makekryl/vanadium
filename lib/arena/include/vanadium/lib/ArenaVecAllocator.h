#pragma once

#include <cassert>
#include <cstring>
#include <type_traits>

#include "vanadium/lib/Arena.h"

namespace vanadium::lib {

template <typename T>
  requires(std::is_trivially_copyable_v<T>)
class ArenaVec {
  using value_type = std::remove_const_t<T>;

 public:
  explicit ArenaVec(Arena& scratch_arena) : scratch_arena_(scratch_arena) {}

  ArenaVec(const ArenaVec&) = delete;
  ArenaVec& operator=(const ArenaVec&) = delete;

  void push_back(const value_type& value) {
    emplace_back(value);
  }

  void push_back(value_type&& value) {
    emplace_back(std::move(value));
  }

  template <typename... Args>
  value_type& emplace_back(Args&&... args) {
    if (size_ == capacity_) {
      reserve_impl(capacity_ == 0 ? 2 : capacity_ * 2);
    }

    auto* const element = std::construct_at(data_ + size_, std::forward<Args>(args)...);

    ++size_;
    return *element;
  }

  void reserve(std::size_t new_capacity) {
    if (new_capacity == capacity_) {
      return;
    }
    reserve_impl(new_capacity);
  }

  [[nodiscard]] std::size_t size() const noexcept {
    return size_;
  }

  [[nodiscard]] bool empty() const noexcept {
    return size_ == 0;
  }

  [[nodiscard]] std::span<T> view() const {
    return {data_, size_};
  }

  [[nodiscard]] std::span<T> build(Arena& out_arena) && {
    if (size_ == 0) {
      return {};
    }

    auto* result = AllocUninitializedArray(out_arena, size_);
    std::memcpy(static_cast<void*>(result), data_, size_ * sizeof(T));

    return {result, size_};
  }

 private:
  void reserve_impl(std::size_t new_capacity) {
    assert(new_capacity > capacity_);

    if (!data_ || !scratch_arena_.TryExtend(data_ + capacity_, (new_capacity - capacity_) * sizeof(T))) {
      auto* new_data = AllocUninitializedArray(scratch_arena_, new_capacity);
      std::memcpy(new_data, data_, size_ * sizeof(T));
      data_ = new_data;
    }

    capacity_ = new_capacity;
  }

  static value_type* AllocUninitializedArray(Arena& arena, std::size_t n) {
    return reinterpret_cast<value_type*>(arena.AllocBuffer(n * sizeof(value_type), alignof(value_type)));
  }

  Arena& scratch_arena_;
  //
  value_type* data_{nullptr};
  std::size_t size_{0};
  std::size_t capacity_{0};
};

class ArenaVecAllocator {
 public:
  ArenaVecAllocator(Arena& out_arena) : out_arena_{out_arena} {}

  template <typename T>
  class BoundVec : public ArenaVec<T> {
   public:
    BoundVec(Arena& scratch_arena, std::span<T>& result_, Arena& out_arena)
        : ArenaVec<T>{scratch_arena}, result_{result_}, out_arena_{out_arena} {}

    ~BoundVec() {
      assert(result_.empty());
      result_ = std::move(*this).ArenaVec<T>::build(out_arena_);
    }

   private:
    std::span<T>& result_;
    Arena& out_arena_;
  };

  template <typename T>
  [[nodiscard]] BoundVec<T> Bind(std::span<T>& result) {
    return BoundVec<T>{scratch_arena_, result, out_arena_};
  }

  template <typename T>
  [[nodiscard]] std::span<T> Make(const auto& f) {
    ArenaVec<T> v{scratch_arena_};
    f(v);
    return std::move(v).build(out_arena_);
  }

  template <typename T>
  void Fill(std::span<T>& result, const auto& f) {
    assert(result.empty());
    ArenaVec<T> v{scratch_arena_};
    f(v);
    result = std::move(v).build(out_arena_);
  }

 private:
  Arena scratch_arena_;
  Arena& out_arena_;
};

}  // namespace vanadium::lib
