#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>

#include "Filesystem.h"
#include "utils/FileReader.h"

namespace vanadium::core {

std::optional<std::string_view> VirtualFS::ReadFile(std::string_view path, FileContentsAllocator alloc) const {
  const std::optional<std::string> fspath = Resolve(path);
  if (!fspath) {
    return std::nullopt;  // TODO
  }
  return utils::ReadFile(*fspath, alloc);
}

bool VirtualFS::WriteFile(std::string_view path, std::string_view contents) const {
  const std::optional<std::string> fspath = Resolve(path);
  if (!fspath) {
    return false;
  }

  if (auto out = std::ofstream(*fspath, std::ios::out)) {
    out << contents;
    return true;
  }
  return false;
}

void VirtualFS::AddSearchPath(std::string_view path) {
  search_paths_.emplace_back(path);
}

std::optional<std::string> VirtualFS::Resolve(std::string_view path) const {
  std::string full_path;
  for (const auto& parent : search_paths_) {
    full_path = parent;
    full_path += path;
    if (std::filesystem::exists(full_path)) {
      return full_path;
    }
  }
  return std::nullopt;
}

}  // namespace vanadium::core
