#pragma once

#include <memory>
#include <new>
#include <utility>

template <class T>
struct ManualLifetime {
  alignas(T) std::byte storage[sizeof(T)];

  ManualLifetime() = default;
  ManualLifetime(const ManualLifetime&) = delete;
  ManualLifetime(ManualLifetime&&) noexcept = delete;
  ManualLifetime& operator=(const ManualLifetime&) = delete;
  ManualLifetime& operator=(ManualLifetime&&) noexcept = delete;

  constexpr T* get() noexcept {
    return std::launder(reinterpret_cast<T*>(storage));
  }

  constexpr const T* get() const noexcept {
    return std::launder(reinterpret_cast<const T*>(storage));
  }

  template <class... Args>
  constexpr T& construct(Args&&... args) {
    return *std::construct_at(get(), std::forward<Args>(args)...);
  }

  constexpr void destroy() noexcept {
    std::destroy_at(get());
  }

  constexpr T& operator*() noexcept {
    return *get();
  }
  constexpr const T& operator*() const noexcept {
    return *get();
  }

  constexpr T* operator->() noexcept {
    return get();
  }
  constexpr const T* operator->() const noexcept {
    return get();
  }
};
