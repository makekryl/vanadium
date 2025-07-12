#include "utils/FileReader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <system_error>

#include "FunctionRef.h"

// TODO: move file IO out of core

namespace vanadium::core {
namespace utils {

std::expected<std::string_view, std::system_error> ReadFile(const std::filesystem::path& path,
                                                            lib::FunctionRef<char*(std::size_t)> alloc) {
  auto f = std::ifstream(path, std::ios::in | std::ios::binary | std::ios::ate);
  if (!f) {
    return std::unexpected{std::system_error(errno, std::system_category())};
  }

  const std::size_t bytes = f.tellg();
  auto* contents = alloc(bytes);
  f.seekg(0, std::ios::beg);
  f.read(contents, bytes);
  return contents;
}

}  // namespace utils
}  // namespace vanadium::core
