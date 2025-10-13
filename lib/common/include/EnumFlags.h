#pragma once

#include <utility>  // IWYU pragma: keep

template <typename E>
struct EnumFlagsWrapper {
  E value;
  constexpr operator E() const {
    return value;
  }
  constexpr operator bool() const {
    return std::to_underlying(value) != 0;
  }
};

#define ENUM_FLAGS_TRAITS(E)                                                \
  inline constexpr EnumFlagsWrapper<E> operator&(E x, E y) {                \
    return {static_cast<E>(std::to_underlying(x) & std::to_underlying(y))}; \
  };                                                                        \
  inline constexpr E operator|(E x, E y) {                                  \
    return static_cast<E>(std::to_underlying(x) | std::to_underlying(y));   \
  };                                                                        \
  inline constexpr E operator^(E x, E y) {                                  \
    return static_cast<E>(std::to_underlying(x) ^ std::to_underlying(y));   \
  };                                                                        \
  inline constexpr E operator~(E x) {                                       \
    return static_cast<E>(~std::to_underlying(x));                          \
  };                                                                        \
  inline constexpr E& operator&=(E& x, E y) {                               \
    x = x & y;                                                              \
    return x;                                                               \
  };                                                                        \
  inline constexpr E& operator|=(E& x, E y) {                               \
    x = x | y;                                                              \
    return x;                                                               \
  };                                                                        \
  inline constexpr E& operator^=(E& x, E y) {                               \
    x = x ^ y;                                                              \
    return x;                                                               \
  };
