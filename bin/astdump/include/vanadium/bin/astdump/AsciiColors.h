#pragma once

namespace asciicolors {

constexpr const char* const kReset = "\e[0m";

constexpr const char* const kBold = "\e[1m";
constexpr const char* const kDim = "\e[2m";

constexpr const char* const kWhite = "\e[38;5;255m";
constexpr const char* const kBrightGray = "\e[1;90m";
constexpr const char* const kBrightCyan = "\e[0;96m";
constexpr const char* const kRed = "\e[31m";

}  // namespace asciicolors
