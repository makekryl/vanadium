#pragma once

#include <oneapi/tbb/enumerable_thread_specific.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/task_arena.h>

#include <filesystem>
#include <ranges>

#include "Arena.h"
#include "FsDirectory.h"
#include "LSConnection.h"
#include "Linter.h"
#include "Program.h"
#include "Project.h"

// TODO:
// Limitations:
//  - subprojects paths cannot be overlapping

namespace vanadium::ls {

struct SubprojectEntry {
  std::string name;

  std::string path;
  std::unique_ptr<core::IDirectory> dir;

  core::Program program;

  std::vector<std::string> references;
};

struct LsState {
  tbb::task_arena task_arena;

  std::filesystem::path root_directory;
  std::optional<tooling::Project> root_project;
  std::unordered_map<std::string, SubprojectEntry> subprojects;

  lint::Linter linter;

  lib::Arena& GetTemporaryArena() {
    return temporary_arena_.local();
  }

  // TODO: those three functions are a temporary solution
  [[nodiscard]] std::pair<SubprojectEntry&, std::string> ResolveFile(std::string_view file_uri) {
    constexpr std::size_t kSchemaLength = std::string_view{"file://"}.size();
    const std::string_view path = file_uri.substr(kSchemaLength, file_uri.size() - kSchemaLength);

    SubprojectEntry* subproject{nullptr};
    for (auto& candidate : subprojects | std::views::values) {
      if (path.starts_with(candidate.dir->Path())) {
        subproject = &candidate;
        break;
      }
    }

    return {*subproject, std::filesystem::relative(path, root_directory).string()};
  }
  [[nodiscard]] std::string FileUriToPath(std::string_view file_uri) const {
    constexpr std::size_t kSchemaLength = std::string_view{"file://"}.size();

    const std::string_view path = file_uri.substr(kSchemaLength, file_uri.size() - kSchemaLength);
    return std::filesystem::relative(path, root_directory).string();
  }
  [[nodiscard]] std::string PathToFileUri(std::string_view path) const {
    return std::format("file://{}", (root_directory / path).string());
  }

 private:
  tbb::enumerable_thread_specific<lib::Arena> temporary_arena_;
};

using LsContext = lserver::ConnectionContext<LsState>;
using VanadiumLsConnection = lserver::Connection<LsState>;

}  // namespace vanadium::ls
