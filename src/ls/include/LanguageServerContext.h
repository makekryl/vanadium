#pragma once

#include <oneapi/tbb/enumerable_thread_specific.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/task_arena.h>

#include "Arena.h"
#include "LSConnection.h"
#include "Linter.h"
#include "Program.h"
#include "Project.h"

namespace vanadium::ls {

struct LsState {
  tbb::task_arena task_arena;

  std::optional<tooling::Project> project;
  core::Program program;

  lint::Linter linter;

  lib::Arena& GetTemporaryArena() {
    return temporary_arena_.local();
  }

  // TODO: those two functions are a temporary solution
  [[nodiscard]] std::string FileUriToPath(std::string_view file_uri) const {
    constexpr std::size_t kSchemaLength = std::string_view{"file://"}.size();

    const std::string_view path = file_uri.substr(kSchemaLength, file_uri.size() - kSchemaLength);
    return std::filesystem::relative(path, project->GetPath()).string();
  }
  [[nodiscard]] std::string PathToFileUri(std::string_view path) const {
    return std::format("file://{}", (project->GetPath() / path).string());
  }

 private:
  tbb::enumerable_thread_specific<lib::Arena> temporary_arena_;
};

using LsContext = lserver::ConnectionContext<LsState>;
using VanadiumLsConnection = lserver::Connection<LsState>;

}  // namespace vanadium::ls
