#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

#include "Arena.h"
#include "Filesystem.h"
#include "FunctionRef.h"

namespace c4::yml {
class Tree;
}

namespace vanadium {
namespace tooling {

class Project {
 public:
  [[nodiscard]] std::optional<std::string_view> GetProperty(std::string_view path) const;
  [[nodiscard]] bool VisitPropertyList(std::string_view path, const lib::Predicate<std::string_view>&) const;
  [[nodiscard]] bool VisitPropertyDict(std::string_view path,
                                       const lib::Predicate<std::string_view, std::string_view>&) const;

  [[nodiscard]] std::string_view GetInheritedProperty(std::string_view path) const;

  void VisitFiles(const lib::Consumer<std::string_view>&) const;

  [[nodiscard]] const std::filesystem::path& GetPath() {
    return path_;
  }
  [[nodiscard]] const core::VirtualFS& GetFS() const {
    return vfs_;
  }

  static std::optional<Project> Load(const std::filesystem::path& config_path);

 private:
  Project(std::filesystem::path);

  void Init();

  c4::yml::Tree* tree_;
  std::filesystem::path path_;

  core::VirtualFS vfs_;
  lib::Arena arena_;
};

}  // namespace tooling
}  // namespace vanadium
