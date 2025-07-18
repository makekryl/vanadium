#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <optional>
#include <rfl/toml/read.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Error.h"
#include "VirtualFS.h"

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

  [[nodiscard]] std::optional<std::filesystem::path> WalkRoot() const;

  [[nodiscard]] const IDirectory& Directory() const noexcept;

  [[nodiscard]] const ProjectManifest& Read() const noexcept;

  template <typename T>
  [[nodiscard]] std::expected<T, Error> ReadSpec() const;

  static std::expected<Project, Error> Load(const std::filesystem::path& config_path);
  Project(std::shared_ptr<IDirectory>, std::string&& contents, ProjectManifest&&);

 private:
  std::shared_ptr<IDirectory> dir_;
  std::string contents_;
  ProjectManifest descriptor_;
};

inline const IDirectory& Project::Directory() const noexcept {
  return *dir_;
}

inline const ProjectManifest& Project::Read() const noexcept {
  return descriptor_;
}

template <typename T>
inline std::expected<T, Error> Project::ReadSpec() const {
  rfl::Result<T> result = rfl::toml::read<T>(contents_);
  if (result.has_value()) {
    return result.value();
  }
  return Error{result.error().what()};
}

}  // namespace tooling
}  // namespace vanadium
