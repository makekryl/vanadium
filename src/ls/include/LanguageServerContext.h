#pragma once

#include <oneapi/tbb/enumerable_thread_specific.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/task_arena.h>

#include <filesystem>
#include <type_traits>
#include <unordered_map>

#include "Arena.h"
#include "LSConnection.h"
#include "Linter.h"
#include "Program.h"
#include "Solution.h"

// TODO:
// Limitations:
//  - subprojects paths cannot be overlapping

namespace vanadium::ls {

struct LsState {
  tbb::task_arena task_arena;

  std::optional<tooling::Solution> solution;
  std::unordered_map<std::string, std::int32_t> file_versions;

  lint::Linter linter;

  lib::Arena& TemporaryArena() {
    return temporary_arena_.local();
  }

  template <typename T, typename... Args>
  T& Temp(Args&&... args) {
    return *TemporaryArena().Alloc<T>(std::forward<Args>(args)...);
  }

  template <typename Result, typename F>
    requires(std::is_invocable_r_v<Result, F, core::Program&, core::SourceFile&>)
  std::optional<Result> WithFile(std::string_view file_uri, F f) {
    if (auto resolution = ResolveFile(file_uri)) {
      auto& [project, path] = *resolution;
      return f(project.program, *project.program.GetFile(path));
    }
    return std::nullopt;
  }

  // TODO: these three functions are a temporary solution
  [[nodiscard]] std::optional<std::pair<tooling::ProjectEntry&, std::string>> ResolveFile(std::string_view file_uri) {
    constexpr std::size_t kSchemaLength = std::string_view{"file://"}.size();
    const std::string_view path = file_uri.substr(kSchemaLength, file_uri.size() - kSchemaLength);

    auto* project = solution->ProjectOf(path);
    if (!project) {
      return std::nullopt;
    }
    return {{*project, std::filesystem::relative(path, solution->RootDirectory()).string()}};
  }
  [[nodiscard]] std::string FileUriToPath(std::string_view file_uri) const {
    constexpr std::size_t kSchemaLength = std::string_view{"file://"}.size();

    const std::string_view path = file_uri.substr(kSchemaLength, file_uri.size() - kSchemaLength);
    return std::filesystem::relative(path, solution->RootDirectory()).string();
  }
  [[nodiscard]] std::string PathToFileUri(std::string_view path) const {
    return std::format("file://{}", (solution->RootDirectory() / path).string());
  }

 private:
  tbb::enumerable_thread_specific<lib::Arena> temporary_arena_;
};

using LsContext = lserver::ConnectionContext<LsState>;
using VanadiumLsConnection = lserver::Connection<LsState>;

}  // namespace vanadium::ls
