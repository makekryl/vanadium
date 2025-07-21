#include "Solution.h"

#include <expected>
#include <filesystem>

#include "Project.h"

namespace vanadium::tooling {

Solution::Solution(std::filesystem::path root_directory, Project&& root_project)
    : root_directory_(std::move(root_directory)), root_project_(std::move(root_project)) {}

namespace {
void InitSubproject(ProjectEntry& subproject, std::string path, std::unique_ptr<tooling::IDirectory>&& dir,
                    std::string name, std::vector<std::string> references) {
  // TODO: think about merging path&dir by relativizing dir->Path()
  subproject.name = std::move(name);
  subproject.path = std::move(path);
  subproject.dir = std::move(dir);
  subproject.references = std::move(references);

  const auto read_file = [&](std::string_view path, std::string& srcbuf) -> void {
    const auto contents = subproject.dir->ReadFile(path, [&](std::size_t size) {
      srcbuf.resize(size);
      return srcbuf.data();
    });
    if (!contents) {
      // TODO, +requires IDirectory::ReadFile signature modification
      std::fprintf(stderr, "Failed to read file '%s' on init, I'm dying :( :)\n", path.data());
      std::fflush(stderr);
      std::abort();
    }
  };

  subproject.program.Update([&](const auto& modify) {
    subproject.dir->VisitFiles([&](const std::string& path) {
      if (!path.ends_with(".ttcn")) {
        return;
      }
      modify.update(std::format("{}/{}", subproject.path, path), read_file);
    });
  });
}
}  // namespace

std::expected<Solution, Error> Solution::Load(const std::filesystem::path& directory) {
  // TODO: handle errors (missing directory, manifest, etc)
  const auto manifest_path = directory / tooling::Project::kManifestFilename;
  if (!std::filesystem::exists(manifest_path)) {
    return std::unexpected<Error>{"Directory does not contain Vanadium manifest"};
  }

  auto root_load_result = tooling::Project::Load(manifest_path);
  if (!root_load_result.has_value()) {
    return std::unexpected{Error{"Failed to load root project", std::move(root_load_result.error())}};
  }

  Solution solution(directory, std::move(*root_load_result));

  const auto& root_desc = solution.root_project_.Read();

  if (root_desc.external) {
    for (const auto& [ext_name, ext_desc] : *root_desc.external) {
      auto& subproject = solution.projects_[ext_name];
      InitSubproject(subproject, ext_desc.path, solution.root_project_.Directory().Subdirectory(ext_desc.path),
                     ext_name, ext_desc.references.value_or(std::vector<std::string>{}));
    }
  }

  const auto add_subproject = [&](std::string_view path, const tooling::ProjectManifest& desc) {
    auto& subproject = solution.projects_[desc.project.name];
    InitSubproject(subproject, std::string{path}, solution.root_project_.Directory().Subdirectory(path),
                   desc.project.name, desc.project.references.value_or(std::vector<std::string>{}));
  };
  if (root_desc.project.subprojects) {
    for (const auto& path : *root_desc.project.subprojects) {
      auto result = tooling::Project::Load(std::filesystem::path(solution.root_project_.Directory().Path()) / path /
                                           tooling::Project::kManifestFilename);
      assert(result.has_value());
      add_subproject(path, result->Read());
    }
  } else {
    add_subproject("", root_desc);
  }

  for (auto& [name, subproj] : solution.projects_) {
    for (const auto& ref : subproj.references) {
      core::Program* program{nullptr};
      if (auto it = solution.projects_.find(ref); it != solution.projects_.end()) {
        program = &it->second.program;
      } else {
        // TODO
        std::fprintf(stderr, "reference '%s' of '%s' could not be satisfied\n", ref.c_str(), name.c_str());
        std::fflush(stderr);
        std::abort();
      }
      subproj.program.AddReference(program);
    }
  }

  for (auto& proj : solution.projects_ | std::views::values) {
    proj.program.Commit([](auto&) {});
  }

  return solution;
}

const ProjectEntry* Solution::ProjectOf(std::string_view path) const {
  const ProjectEntry* project{nullptr};
  for (const auto& candidate : projects_ | std::views::values) {
    if (path.starts_with(candidate.dir->Path())) {
      project = &candidate;

      // TODO: maybe support overlapping directories
      break;
    }
  }
  return project;
}

}  // namespace vanadium::tooling
