#pragma once

#include <cstdint>
#include <mutex>
#include <span>
#include <vector>

namespace vanadium::lib::concurrency {

class GenericThreadSpecific {
 public:
  using Key = std::uint32_t;
  using AllocFunc = void* (*)();
  using DeallocFunc = void (*)(void*);

  GenericThreadSpecific(AllocFunc alloc, DeallocFunc dealloc);
  ~GenericThreadSpecific();

  GenericThreadSpecific(const GenericThreadSpecific&) = delete;
  GenericThreadSpecific(GenericThreadSpecific&&) = delete;
  GenericThreadSpecific& operator=(const GenericThreadSpecific&) = delete;
  GenericThreadSpecific& operator=(GenericThreadSpecific&&) = delete;

  [[nodiscard]] void* Local() const;
  [[nodiscard]] std::span<void* const> All();
  [[nodiscard]] std::span<const void* const> All() const;

 private:
  std::uint32_t key_;

  AllocFunc alloc_;
  DeallocFunc dealloc_;

  mutable std::vector<void*> allocated_;
  mutable std::mutex mtx_;
};

template <typename T>
class ThreadSpecific : private GenericThreadSpecific {
 public:
  ThreadSpecific()
      : GenericThreadSpecific(
            [] -> void* {
              return new T;
            },
            [](void* p) -> void {
              delete reinterpret_cast<T*>(p);
            }) {}

  [[nodiscard]] T& Local() const {
    return *reinterpret_cast<T*>(GenericThreadSpecific::Local());
  }

  [[nodiscard]] std::span<T* const> All() {
    auto type_erased_span = GenericThreadSpecific::All();
    return {reinterpret_cast<T* const*>(type_erased_span.data()), type_erased_span.size()};
  }
  [[nodiscard]] std::span<const T* const> All() const {
    auto type_erased_span = GenericThreadSpecific::All();
    return {reinterpret_cast<const T* const*>(type_erased_span.data()), type_erased_span.size()};
  }
};

}  // namespace vanadium::lib::concurrency
