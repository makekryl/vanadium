#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include "FunctionRef.h"

namespace vanadium::tooling {

class IDirectory {
 public:
  virtual ~IDirectory() {}

  using FileContentsAllocator = lib::FunctionRef<char*(std::size_t)>;

  [[nodiscard]] virtual std::string_view Path() const = 0;

  [[nodiscard]] virtual std::optional<std::string_view> ReadFile(std::string_view path,
                                                                 FileContentsAllocator alloc) const = 0;
  [[nodiscard]] virtual bool WriteFile(std::string_view path, std::string_view contents) const = 0;

  virtual void VisitFiles(const lib::Consumer<std::string>&) const = 0;

  [[nodiscard]] virtual std::unique_ptr<IDirectory> Subdirectory(std::string_view name) const = 0;
};

class FilesystemDirectory : public IDirectory {
 public:
  explicit FilesystemDirectory(std::filesystem::path);

  [[nodiscard]] std::string_view Path() const final;

  [[nodiscard]] std::optional<std::string_view> ReadFile(std::string_view path,
                                                         FileContentsAllocator alloc) const final;
  [[nodiscard]] bool WriteFile(std::string_view path, std::string_view contents) const final;

  void VisitFiles(const lib::Consumer<std::string>&) const final;

  [[nodiscard]] std::unique_ptr<IDirectory> Subdirectory(std::string_view name) const final;

 private:
  std::filesystem::path base_path_;
  std::string base_path_str_;
};

}  // namespace vanadium::tooling
