#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "Error.h"
#include "Filesystem.h"
#include "FunctionRef.h"

namespace vanadium::tooling::fs {

class SystemFS final : public Filesystem {
 public:
  SystemFS() = default;

  [[nodiscard]] std::string Join(std::string_view base_path, std::string_view path) const final;
  [[nodiscard]] std::string Relative(std::string_view path, std::string_view base_path) const final;

  [[nodiscard]] bool Exists(const std::string& path) const final;
  [[nodiscard]] bool IsDirectory(const std::string& path) const final;

  [[nodiscard]] std::expected<std::string_view, Error> ReadFile(const std::string& path,
                                                                FileContentsAllocator) const final;
  [[nodiscard]] std::optional<Error> WriteFile(const std::string& path, std::string_view contents) const final;

  void VisitFiles(const std::string& path, lib::Consumer<std::string> accept) const final;
};

}  // namespace vanadium::tooling::fs
