#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

#include "vanadium/compiler/RuntimeBindings.h"

namespace vanadium::compiler::literals {

std::variant<RuntimeBindings::NativeIntType, std::string_view> ParseInt(std::string_view);
double ParseFloat(std::string_view);

std::string_view ParseCharstring(std::string_view);
std::string ParseOctetstring(std::string_view);
std::pair<std::string, std::uint32_t> ParseBitstring(std::string_view);
std::pair<std::string, std::uint32_t> ParseHexstring(std::string_view);

}  // namespace vanadium::compiler::literals
