#pragma once

#ifndef NDEBUG
#include <print>  // IWYU pragma: keep

#define VANADIUM_DEBUG_ERROR(msg, ...)                                     \
  do {                                                                     \
    std::println(stderr, __FILE__ ":{} :: " msg, __LINE__, ##__VA_ARGS__); \
    std::fflush(stderr);                                                   \
    std::exit(42);                                                         \
  } while (0);

#define VANADIUM_DEBUG_ASSERT(cond, msg, ...)   \
  do {                                          \
    if (!(cond)) {                              \
      VANADIUM_DEBUG_ERROR(msg, ##__VA_ARGS__); \
    }                                           \
  } while (0);

#else
#define VANADIUM_DEBUG_ERROR()
#define VANADIUM_DEBUG_ASSERT()
#endif
