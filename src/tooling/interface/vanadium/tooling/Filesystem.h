#pragma once

#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <vanadium/lib/Error.h>
#include <vanadium/lib/FunctionRef.h>

namespace vanadium::tooling {
namespace fs {

using FileContentsAllocator = lib::FunctionRef<char*(std::size_t)>;

class Filesystem {
 public:
  virtual ~Filesystem() = default;

  [[nodiscard]] virtual std::string Join(std::string_view base_path, std::string_view path) const = 0;
  [[nodiscard]] virtual std::string Relative(std::string_view path, std::string_view base_path) const = 0;
  [[nodiscard]] virtual std::string Normalize(std::string_view path) const = 0;

  [[nodiscard]] virtual bool Exists(const std::string& path) const = 0;
  [[nodiscard]] virtual bool IsDirectory(const std::string& path) const = 0;

  [[nodiscard]] virtual std::expected<std::string_view, Error> ReadFile(const std::string& path,
                                                                        FileContentsAllocator) const = 0;
  [[nodiscard]] virtual std::optional<Error> WriteFile(const std::string& path, std::string_view contents) const = 0;

  virtual void VisitFiles(const std::string& path, lib::Consumer<std::string> accept) const = 0;
};

struct Path {
  std::shared_ptr<Filesystem> fs;
  std::string base_path;

  [[nodiscard]] std::string Join(std::string_view subpath) const {
    return fs->Join(base_path, subpath);
  }
  [[nodiscard]] Path Resolve(std::string_view subpath) const {
    return Path{
        .fs = fs,
        .base_path = Join(subpath),
    };
  }
  [[nodiscard]] Path Normalize() const {
    return Path{
        .fs = fs,
        .base_path = fs->Normalize(base_path),
    };
  }

  [[nodiscard]] bool Exists() const {
    return fs->Exists(base_path);
  }
  [[nodiscard]] bool IsDirectory() const {
    return fs->IsDirectory(base_path);
  }

  [[nodiscard]] std::expected<std::string_view, Error> Read(FileContentsAllocator alloc) const {
    return fs->ReadFile(base_path, alloc);
  }
  [[nodiscard]] std::optional<Error> Write(std::string_view contents) const {
    return fs->WriteFile(base_path, contents);
  }

  [[nodiscard]] std::expected<std::string_view, Error> ReadFile(const std::string& subpath,
                                                                FileContentsAllocator alloc) const {
    return fs->ReadFile(fs->Join(base_path, subpath), alloc);
  }
  [[nodiscard]] std::optional<Error> WriteFile(const std::string& subpath, std::string_view contents) const {
    return fs->WriteFile(fs->Join(base_path, subpath), contents);
  }

  void VisitFiles(lib::Consumer<std::string> accept) const {
    fs->VisitFiles(base_path, accept);
  }
};

template <std::derived_from<Filesystem> ConcreteFS>
[[nodiscard]] Path Root(const std::string& path = "") {
  return Path{
      .fs = std::make_shared<ConcreteFS>(),
      .base_path = path,
  };
}

}  // namespace fs
}  // namespace vanadium::tooling
