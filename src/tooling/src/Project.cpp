#include "vanadium/tooling/Project.h"

#include <cassert>
#include <expected>

#include <glaze/core/reflect.hpp>
#include <glaze/toml.hpp>

#include "vanadium/tooling/Filesystem.h"

namespace vanadium {
namespace tooling {

std::expected<Project, Error> Project::Load(const fs::Path& path) {
  std::string contents;
  {
    const auto& manifest_path = path.Resolve(kManifestFilename);
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

  ProjectManifest manifest;
  if (auto ec = glz::read<glz::opts{.format = glz::TOML, .error_on_unknown_keys = false}>(manifest, contents); ec) {
    return std::unexpected{Error{"Failed to parse manifest", Error{glz::format_error(ec, contents)}}};
  }

  return Project{path, std::move(contents), std::move(manifest)};
}

Project::Project(fs::Path path, std::string&& contents, ProjectManifest&& descriptor)
    : path_(std::move(path)), manifest_contents_(std::move(contents)), manifest_(std::move(descriptor)) {};

}  // namespace tooling
}  // namespace vanadium
