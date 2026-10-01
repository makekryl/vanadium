#pragma once

#include <string_view>

#include "vanadium/lib/Arena.h"

namespace vanadium::lib {

template <typename... Args>
std::string_view FormatStringToArena(Arena& arena, std::format_string<Args...> fmt, Args&&... args) {
  const std::size_t length = std::formatted_size(fmt, std::forward<Args>(args)...);
  auto buf = arena.AllocStringBuffer(length);
  std::format_to_n(buf.data(), buf.size(), fmt, std::forward<Args>(args)...);
  return {buf.data(), length};
}

}  // namespace vanadium::lib
