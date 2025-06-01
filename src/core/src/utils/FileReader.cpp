#include "utils/FileReader.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "FunctionRef.h"

namespace vanadium::core {
namespace utils {

std::optional<std::string_view> ReadFile(const std::filesystem::path& path,
                                         lib::FunctionRef<char*(std::size_t)> alloc) {
  // TODO: HANDLE ERROR
  if (auto f = std::ifstream(path, std::ios::in | std::ios::binary | std::ios::ate)) {
    const std::size_t bytes = f.tellg();
    auto* contents = alloc(bytes);
    f.seekg(0, std::ios::beg);
    f.read(contents, bytes);
    return contents;
  }
  std::cerr << " [ERROR] Failed to read file '" << path << "'\n";  // TODO: remove this & handle the error
  return std::nullopt;
}

}  // namespace utils
}  // namespace vanadium::core
