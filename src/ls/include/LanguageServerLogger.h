#pragma once

#include <print>  // IWYU pragma: keep

// This way of logging does not work with LSP client-server communication channels other than stdio.
// Logging should be done via a server->client notification in such cases, but it require context.

#define VLS_LOG(tag, msg, ...)                         \
  do {                                                 \
    std::println(stderr, tag "/ " msg, ##__VA_ARGS__); \
  } while (0)

#define VLS_INFO(...) VLS_LOG("I", __VA_ARGS__)
#define VLS_WARN(...) VLS_LOG("W", __VA_ARGS__)
#define VLS_ERROR(...) VLS_LOG("E", __VA_ARGS__)

#ifndef NDEBUG
#define VLS_DEBUG(...) VLS_LOG("D", __VA_ARGS__)
#else
#define VLS_DEBUG(msg, ...)
#endif
