#pragma once

namespace asciicolors {

inline constexpr const char* const kReset = "\e[0m";

inline constexpr const char* const kBold = "\e[1m";
inline constexpr const char* const kDim = "\e[2m";

inline constexpr const char* const kWhite = "\e[38;5;255m";
inline constexpr const char* const kBrightGray = "\e[1;90m";
inline constexpr const char* const kBrightCyan = "\e[0;96m";
inline constexpr const char* const kRed = "\e[31m";

}  // namespace asciicolors
