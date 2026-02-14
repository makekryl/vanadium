#pragma once

#ifndef NDEBUG
#include <print>  // IWYU pragma: keep
#endif

#ifndef NDEBUG
#define VANADIUM_DEBUG_ASSERT(cond, msg, ...)                              \
  if (!(cond)) {                                                           \
    std::println(stderr, __FILE__ ":{} :: " msg, __LINE__, ##__VA_ARGS__); \
    std::fflush(stderr);                                                   \
    std::exit(42);                                                         \
  }
#else
#define VANADIUM_DEBUG_ASSERT(msg, ...)
#endif
