#pragma once

#include <expected>
#include <filesystem>
#include <ranges>
#include <vector>

#include "Error.h"
#include "Program.h"
#include "Project.h"
#include "VirtualFS.h"

namespace vanadium {
namespace tooling {

struct ProjectEntry {
  std::string name;

  std::string path;
  std::unique_ptr<IDirectory> dir;

  core::Program program;

  std::vector<std::string> references;
};

class Solution {
 public:
  const std::filesystem::path& RootDirectory() const {
    return root_directory_;
  }

  [[nodiscard]] const ProjectEntry* ProjectOf(std::string_view path) const;
  [[nodiscard]] ProjectEntry* ProjectOf(std::string_view path) {
    return const_cast<ProjectEntry*>(static_cast<const Solution*>(this)->ProjectOf(path));
  }

  auto Entries() const {
    return projects_ | std::views::values;
  }
  auto Entries() {
    return projects_ | std::views::values;
  }

  static std::expected<Solution, Error> Load(const std::filesystem::path& directory);

 private:
  Solution(std::filesystem::path root_directory, Project&& project);

  std::filesystem::path root_directory_;
  Project root_project_;
  std::unordered_map<std::string, ProjectEntry> projects_;
};

}  // namespace tooling
}  // namespace vanadium
