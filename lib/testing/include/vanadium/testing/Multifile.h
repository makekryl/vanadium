#pragma once

#include <expected>
#include <string_view>
#include <unordered_map>

namespace vanadium::testing {

bool IsMultifile(std::string_view);
std::expected<std::unordered_map<std::string_view, std::string_view>, std::string> ParseMultifile(std::string_view);

}  // namespace vanadium::testing
