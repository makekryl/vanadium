#include "Solution.h"

#include <expected>
#include <print>

#include "Filesystem.h"
#include "Program.h"
#include "Project.h"

namespace vanadium::tooling {

Solution::Solution(Project&& root_project) : root_project_(std::move(root_project)) {}

namespace {
void InitSubproject(const Solution& solution, SolutionProject& subproject) {
  const auto& dir = subproject.project.Directory();
  if (!dir.Exists()) {
    // TODO
    std::println(stderr, "Project directory does not exist: '{}'", dir.base_path);
    return;
  }

  const auto read_file = [&](const std::string& path, std::string& srcbuf) -> void {
    const auto res = solution.Directory().ReadFile(path, [&](std::size_t size) {
      srcbuf.resize(size);
      return srcbuf.data();
    });
    if (!res) {
      // TODO
      std::println(stderr, "Failed to read file '{}' during project '{}' initialization: {}", path, subproject.Name(),
                   res.error().String());
      std::fflush(stderr);
      std::abort();
    }
  };

  subproject.program.Update([&](const core::Program::ProgramModifier& modify) {
    dir.VisitFiles([&](const std::string& filepath) {
      if (!filepath.ends_with(".ttcn")) {
        return;
      }
      if (filepath.contains("stubs")) {
        // TODO: home hack
        return;
      }
      modify.update(dir.fs->Relative(dir.Join(filepath), solution.Directory().base_path), read_file);
    });
  });
}
}  // namespace

std::expected<Solution, Error> Solution::Load(const fs::Path& path, lib::Consumer<Solution&> precommit) {
  auto root_load_result = tooling::Project::Load(path);
  if (!root_load_result.has_value()) {
    return std::unexpected{Error{"Failed to load root project", std::move(root_load_result.error())}};
  }

  Solution solution(std::move(*root_load_result));

  const auto& root_desc = solution.root_project_.Manifest();

  if (root_desc.external) {
    for (const auto& [ext_name, ext_desc] : *root_desc.external) {
      auto [it, inserted] =
          solution.projects_.try_emplace(ext_name, Project(path.Resolve(ext_desc.path), "",
                                                           ProjectManifest{
                                                               .project =
                                                                   {
                                                                       .name = ext_name,
                                                                       .references = ext_desc.references,
                                                                   },
                                                           }));

      if (!inserted) {
        return std::unexpected{Error{std::format("Duplicate project: '{}'", ext_name)}};
      }

      auto& sol_project = it->second;

      sol_project.managed = false;
      InitSubproject(solution, sol_project);
    }
  }

  if (root_desc.project.subprojects) {
    for (const auto& subpath : *root_desc.project.subprojects) {
      auto result = tooling::Project::Load(path.Resolve(subpath));
      if (!result) {
        return std::unexpected{
            Error{std::format("Failed to load project at '{}'", subpath), std::move(result.error())}};
      }

      const auto name = result->Name();  // *result will be moved
      auto [it, inserted] = solution.projects_.try_emplace(name, std::move(*result));

      if (!inserted) {
        return std::unexpected{Error{std::format("Duplicate project: '{}'", name)}};
      }

      auto& sol_project = it->second;
      sol_project.managed = true;

      InitSubproject(solution, sol_project);
    }
  } else {
    auto [it, _] = solution.projects_.try_emplace("<root>", Project(solution.root_project_));
    auto& sol_project = it->second;
    sol_project.managed = true;
    InitSubproject(solution, sol_project);
  }

  for (auto& [name, subproj] : solution.projects_) {
    const auto& refs = subproj.project.Manifest().project.references;
    if (!refs) {
      continue;
    }
    for (const auto& ref : *refs) {
      auto it = solution.projects_.find(ref);
      if (it == solution.projects_.end()) {
        return std::unexpected{Error{std::format("Reference '{}' of project '{}' cannot be satisfied", ref, name)}};
      }
      subproj.program.AddReference(&it->second.program);
    }
  }

  precommit(solution);

  for (auto& proj : solution.projects_ | std::views::values) {
    proj.program.Commit([](auto&) {});
  }

  return solution;
}

const SolutionProject* Solution::ProjectOf(std::string_view path) const {
  for (const auto& candidate : projects_ | std::views::values) {
    if (path.starts_with(candidate.project.Directory().base_path)) {
      // TODO: support overlapping directories
      return &candidate;
    }
  }
  return nullptr;
}

}  // namespace vanadium::tooling
