#pragma once

#include <utility>  // IWYU pragma: keep

// https://stackoverflow.com/a/17771358

#define ENUM_FLAGS_TRAITS(T)                                              \
  inline constexpr T operator&(T x, T y) {                                \
    return static_cast<T>(std::to_underlying(x) & std::to_underlying(y)); \
  };                                                                      \
  inline constexpr T operator|(T x, T y) {                                \
    return static_cast<T>(std::to_underlying(x) | std::to_underlying(y)); \
  };                                                                      \
  inline constexpr T operator^(T x, T y) {                                \
    return static_cast<T>(std::to_underlying(x) ^ std::to_underlying(y)); \
  };                                                                      \
  inline constexpr T operator~(T x) {                                     \
    return static_cast<T>(~std::to_underlying(x));                        \
  };                                                                      \
  inline constexpr T& operator&=(T& x, T y) {                             \
    x = x & y;                                                            \
    return x;                                                             \
  };                                                                      \
  inline constexpr T& operator|=(T& x, T y) {                             \
    x = x | y;                                                            \
    return x;                                                             \
  };                                                                      \
  inline constexpr T& operator^=(T& x, T y) {                             \
    x = x ^ y;                                                            \
    return x;                                                             \
  };                                                                      \
  inline constexpr bool to_bool(T x) {                                    \
    return std::to_underlying(x) != 0;                                    \
  }
