#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "FsDirectory.h"
#include "utils/FileReader.h"

// TODO: move this out of core

namespace vanadium::core {

FilesystemDirectory::FilesystemDirectory(std::filesystem::path path)
    : base_path_(std::move(path)), base_path_str_(base_path_.string()) {}

std::string_view FilesystemDirectory::Path() const {
  return base_path_str_;
}

std::optional<std::string_view> FilesystemDirectory::ReadFile(std::string_view path,
                                                              FileContentsAllocator alloc) const {
  auto result = utils::ReadFile(std::filesystem::path(path), alloc);
  if (result.has_value()) {
    return result.value();
  }
  // TODO: change API to retrieve actual error
  return std::nullopt;
}

bool FilesystemDirectory::WriteFile(std::string_view path, std::string_view contents) const {
  if (auto out = std::ofstream(std::filesystem::path(path), std::ios::out)) {
    out << contents;
    return true;
  }
  return false;
}

namespace {
void VisitDirectory(const std::filesystem::path &directory,
                    const lib::Consumer<const std::filesystem::path &> &accept) {
  for (const auto &entry : std::filesystem::recursive_directory_iterator(directory)) {
    accept(entry.path());
  }
}
}  // namespace

void FilesystemDirectory::VisitFiles(const lib::Consumer<std::string> &accept) const {
  const auto accept_fwd = [&](const std::filesystem::path &path) {
    accept(std::filesystem::relative(path, base_path_).string());
  };
  VisitDirectory(base_path_, accept_fwd);
}

std::unique_ptr<IDirectory> FilesystemDirectory::Subdirectory(std::string_view name) const {
  return std::make_unique<FilesystemDirectory>(base_path_ / name);
}

}  // namespace vanadium::core
