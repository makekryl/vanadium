#pragma once

#include <expected>
#include <ranges>

#include "Error.h"
#include "Filesystem.h"
#include "Program.h"
#include "Project.h"

namespace vanadium {
namespace tooling {

struct SolutionProject {
  Project project;
  bool managed{true};
  core::Program program;

  SolutionProject(Project&& project_) : project(std::move(project_)) {}

  [[nodiscard]] const fs::Path& Path() const noexcept {
    return project.Path();
  }
  [[nodiscard]] const std::string& Name() const noexcept {
    return project.Name();
  }
};

class Solution {
 public:
  const Project& RootProject() const noexcept {
    return root_project_;
  }
  [[nodiscard]] const fs::Path& Path() const noexcept {
    return root_project_.Path();
  }

  [[nodiscard]] const SolutionProject* ProjectOf(std::string_view path) const;
  [[nodiscard]] SolutionProject* ProjectOf(std::string_view path) {
    return const_cast<SolutionProject*>(static_cast<const Solution*>(this)->ProjectOf(path));
  }

  auto Projects() const {
    return projects_ | std::views::values;
  }
  auto Projects() {
    return projects_ | std::views::values;
  }

  static std::expected<Solution, Error> Load(const fs::Path&);

 private:
  Solution(Project&& root_project);

  Project root_project_;
  std::unordered_map<std::string, SolutionProject> projects_;
};

}  // namespace tooling
}  // namespace vanadium
