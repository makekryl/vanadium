#pragma once

#include <expected>
#include <filesystem>
#include <string_view>

#include "FunctionRef.h"

namespace vanadium::core {
namespace utils {

std::expected<std::string_view, std::system_error> ReadFile(const std::filesystem::path& path,
                                                            lib::FunctionRef<char*(std::size_t)> alloc);

}  // namespace utils
}  // namespace vanadium::core
