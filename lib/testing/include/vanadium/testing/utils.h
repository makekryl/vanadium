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

inline void WriteFile(const std::filesystem::path& path, std::string_view buf) {
  std::ofstream f(path);
  f << buf;
}

}  // namespace vanadium::testing::utils
