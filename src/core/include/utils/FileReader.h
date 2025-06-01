#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

#include "FunctionRef.h"

namespace vanadium::core {
namespace utils {

std::optional<std::string_view> ReadFile(const std::filesystem::path& path, lib::FunctionRef<char*(std::size_t)> alloc);

}  // namespace utils
}  // namespace vanadium::core
