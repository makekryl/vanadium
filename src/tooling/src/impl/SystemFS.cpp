#include "vanadium/tooling/impl/SystemFS.h"

#include <cerrno>
#include <cstring>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include <vanadium/lib/Error.h>

namespace {
Error GetSystemError() {
  return Error{std::strerror(errno)};
}
Error GetSystemError(std::string_view text) {
  return Error{std::format("{}: {}", text, std::strerror(errno))};
}
}  // namespace

namespace vanadium::tooling::fs {

std::string SystemFS::Join(std::string_view base_path, std::string_view path) const {
  return std::format("{}{}{}", base_path, std::filesystem::path::preferred_separator, path);
}

[[nodiscard]] std::string SystemFS::Relative(std::string_view path, std::string_view base_path) const {
  return std::filesystem::path(path).lexically_relative(base_path).string();
}

bool SystemFS::Exists(const std::string &path) const {
  return std::filesystem::exists(path);
}

bool SystemFS::IsDirectory(const std::string &path) const {
  return std::filesystem::is_directory(path);
}

std::expected<std::string_view, Error> SystemFS::ReadFile(const std::string &path, FileContentsAllocator alloc) const {
  auto f = std::ifstream(path, std::ios::in | std::ios::binary | std::ios::ate);
  if (!f) [[unlikely]] {
    return std::unexpected{GetSystemError("failed to open file")};
  }

  const std::streampos bytes = f.tellg();
  if (bytes == -1) [[unlikely]] {
    return std::unexpected{GetSystemError("failed to read file size")};
  }

  auto *contents = alloc(bytes);
  f.seekg(0, std::ios::beg);
  f.read(contents, bytes);

  if (!f) [[unlikely]] {
    return std::unexpected{GetSystemError()};
  }

  return contents;
}

std::optional<Error> SystemFS::WriteFile(const std::string &path, std::string_view contents) const {
  auto out = std::ofstream(path, std::ios::out);
  if (!out) [[unlikely]] {
    return Error{GetSystemError("failed to open file")};
  }

  out << contents;
  if (!out) [[unlikely]] {
    return Error{GetSystemError()};
  }

  return std::nullopt;
}

namespace {
void VisitDirectory(const std::filesystem::path &directory, lib::Consumer<const std::filesystem::path &> accept) {
  for (const auto &entry : std::filesystem::recursive_directory_iterator(directory)) {
    accept(entry.path());
  }
}
}  // namespace

void SystemFS::VisitFiles(const std::string &path, lib::Consumer<std::string> accept) const {
  const std::filesystem::path base_path(path);
  const auto accept_fwd = [&](const std::filesystem::path &fspath) {
    accept(std::filesystem::path(fspath).lexically_relative(base_path));
  };
  VisitDirectory(base_path, accept_fwd);
}

}  // namespace vanadium::tooling::fs
