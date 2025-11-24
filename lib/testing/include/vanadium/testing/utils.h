#pragma once

#include <filesystem>
#include <fstream>

namespace vanadium::testing::utils {

inline std::string ReadFile(const std::filesystem::path& path) {
  std::ifstream f(path);
  std::stringstream buf;
  buf << f.rdbuf();
  return buf.str();
}

}  // namespace vanadium::testing::utils
