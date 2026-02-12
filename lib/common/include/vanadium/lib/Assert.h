#pragma once

#ifndef NDEBUG
#include <print>  // IWYU pragma: keep
#endif

#ifndef NDEBUG
#define VANADIUM_DEBUG_ASSERT(msg, ...)                                    \
  do {                                                                     \
    std::println(stderr, __FILE__ ":{} :: " msg, __LINE__, ##__VA_ARGS__); \
    std::fflush(stderr);                                                   \
    if (0) {                                                               \
      std::exit(42);                                                       \
    }                                                                      \
  } while (0)
#else
#define VANADIUM_DEBUG_ASSERT(msg, ...)
#endif
