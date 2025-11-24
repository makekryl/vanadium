#pragma once

#include <filesystem>

namespace vanadium::e2e::core {
namespace opts {
extern std::filesystem::path suites_dir;
extern bool overwrite_snapshots;
}  // namespace opts
}  // namespace vanadium::e2e::core
