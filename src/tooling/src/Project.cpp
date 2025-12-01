#include "Project.h"

#include <cassert>
#include <expected>
#include <rfl/toml.hpp>

#include "Filesystem.h"

namespace vanadium {
namespace tooling {

std::expected<Project, Error> Project::Load(const fs::Path &path) {
  std::string contents;
  {
    const auto &manifest_path = path.Resolve(kManifestFilename);
    if (!manifest_path.Exists() || manifest_path.IsDirectory()) {
      return std::unexpected<Error>{"Manifest file does not exist"};
    }

    const auto alloc_contents = [&contents](std::size_t size) {
      contents.resize(size);
      return contents.data();
    };
    if (auto res = manifest_path.Read(alloc_contents); !res) {
      return std::unexpected{Error{"Failed to read file: {}", std::move(res.error())}};
    }
  }

  rfl::Result<ProjectManifest> parse_result = rfl::toml::read<ProjectManifest>(contents);
  if (!parse_result.has_value()) {
    return std::unexpected{Error{"Failed to parse manifest", Error{parse_result.error().what()}}};
  }

  return Project{path, std::move(contents), std::move(*parse_result)};
}

Project::Project(fs::Path path, std::string &&contents, ProjectManifest &&descriptor)
    : path_(std::move(path)), manifest_contents_(std::move(contents)), manifest_(std::move(descriptor)) {};

}  // namespace tooling
}  // namespace vanadium
