#pragma once

#include <expected>
#include <optional>
#include <rfl/toml/read.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Error.h"
#include "Filesystem.h"

namespace vanadium {
namespace tooling {

// reflectcpp does not support reading into string_views
// despite we're not asking for any lifetime guarantees,
// and this is extremely disappointing

struct ExternalProjectDescriptor {
  std::string path;
  std::optional<std::vector<std::string>> references;
};

struct ProjectManifest {
  std::optional<bool> root;

  struct {
    std::string name;
    std::optional<std::vector<std::string>> references;
    std::optional<std::vector<std::string>> subprojects;
  } project;

  std::optional<std::unordered_map<std::string, ExternalProjectDescriptor>> external;
};

class Project {
 public:
  static constexpr std::string_view kManifestFilename = ".vanadiumrc.toml";

  [[nodiscard]] const fs::Path& Path() const noexcept {
    return path_;
  }

  [[nodiscard]] const std::string& Name() const noexcept {
    return manifest_.project.name;
  }

  [[nodiscard]] const ProjectManifest& Manifest() const noexcept {
    return manifest_;
  }

  template <typename T>
  [[nodiscard]] std::expected<T, Error> ReadSpec() const {
    rfl::Result<T> result = rfl::toml::read<T>(manifest_contents_);
    if (result.has_value()) {
      return result.value();
    }
    return Error{result.error().what()};
  }

  static std::expected<Project, Error> Load(const fs::Path& path);
  Project(fs::Path path, std::string&& contents, ProjectManifest&&);

 private:
  fs::Path path_;

  std::string manifest_contents_;
  ProjectManifest manifest_;
};

}  // namespace tooling
}  // namespace vanadium
