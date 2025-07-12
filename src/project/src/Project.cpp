#include "Project.h"

#include <cassert>
#include <expected>
#include <filesystem>

#include "FsDirectory.h"
#include "utils/FileReader.h"

namespace vanadium {
namespace tooling {

Project::Project(std::shared_ptr<core::IDirectory> dir, std::string &&contents, ProjectDescriptor &&descriptor)
    : dir_(std::move(dir)), contents_(std::move(contents)), descriptor_(std::move(descriptor)) {};

std::expected<Project, Error> Project::Load(const std::filesystem::path &config_path) {
  if (!std::filesystem::exists(config_path)) {
    return std::unexpected<Error>{"File not exists"};
  }

  std::string contents;
  const auto result = core::utils::ReadFile(config_path, [&](std::size_t size) {
    contents.resize(size);
    return contents.data();
  });
  if (!result.has_value()) {
    return std::unexpected<Error>{result.error().what()};
  }

  rfl::Result<ProjectDescriptor> parse_result = rfl::toml::read<ProjectDescriptor>(contents);
  if (!parse_result.has_value()) {
    return std::unexpected<Error>{parse_result.error().what()};
  }

  Project project(std::make_shared<core::FilesystemDirectory>(config_path.parent_path()), std::move(contents),
                  std::move(parse_result.value()));

  if (!result) {
    // TODO: improve error handling, maybe rethink the Error wrapper class
    const auto &err = result.error();
    return std::unexpected{Error{"Failed to read project descriptor", Error{err.what()}}};
  }

  return project;
}

}  // namespace tooling
}  // namespace vanadium
