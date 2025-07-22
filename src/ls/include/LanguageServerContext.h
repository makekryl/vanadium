#pragma once

#include <oneapi/tbb/enumerable_thread_specific.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/task_arena.h>

#include <filesystem>

#include "Arena.h"
#include "LSConnection.h"
#include "Linter.h"
#include "Solution.h"

// TODO:
// Limitations:
//  - subprojects paths cannot be overlapping

namespace vanadium::ls {

struct LsState {
  tbb::task_arena task_arena;

  std::optional<tooling::Solution> solution;

  lint::Linter linter;

  lib::Arena& GetTemporaryArena() {
    return temporary_arena_.local();
  }

  template <typename T, typename... Args>
  T& Temp(Args&&... args) {
    return *GetTemporaryArena().Alloc<T>(std::forward<Args>(args)...);
  }

  // TODO: those three functions are a temporary solution
  [[nodiscard]] std::pair<tooling::ProjectEntry&, std::string> ResolveFile(std::string_view file_uri) {
    constexpr std::size_t kSchemaLength = std::string_view{"file://"}.size();
    const std::string_view path = file_uri.substr(kSchemaLength, file_uri.size() - kSchemaLength);

    return {*solution->ProjectOf(path), std::filesystem::relative(path, solution->RootDirectory()).string()};
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
