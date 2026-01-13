#pragma once

#include <tbb/enumerable_thread_specific.h>
#include <tbb/task_arena.h>
#include <vanadium/core/Program.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/Metaprogramming.h>
#include <vanadium/lib/lserver/Connection.h>

#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "LanguageServerSession.h"
#include "LanguageServerSolution.h"
#include "Linter.h"
#include "Solution.h"

// TODO: support overlapping projects paths

namespace vanadium::ls {

template <typename Params>
concept IsDocumentBoundParams = requires(Params params) {
  { params.textDocument.uri } -> std::convertible_to<std::string_view>;
};

struct LsContext {
  lserver::Connection* const connection;

  tbb::task_arena task_arena;

  std::optional<tooling::Solution> solution;
  std::unordered_map<std::string, std::int32_t> file_versions;

  lint::Linter linter;

  //

  LsContext(lserver::Connection& conn) : connection(&conn) {}

  //

  lib::Arena& TemporaryArena() {
    return temporary_arena_.local();
  }

  template <typename T, typename... Args>
  T& Temp(Args&&... args) {
    return *TemporaryArena().Alloc<T>(std::forward<Args>(args)...);
  }

  template <typename F>
    requires(std::is_invocable_v<F, LsSessionRef &&>)
  auto LockData(F f) {
    // TODO: lock data, RWmutex with writer preference
    return f({
        .solution = *solution,
        .linter = linter,
        .arena = temporary_arena_.local(),
    });
  }

  template <typename Result, IsDocumentBoundParams Params>
    requires(!std::is_same_v<Result, void>)
  std::optional<Result> WithFile(const Params& params,
                                 mp::Invocable<Result, const Params&, const core::SourceFile&, LsSessionRef> auto f) {
    return LockData([&](LsSessionRef&& d) -> std::optional<Result> {
      if (auto resolution = ResolveFileUri(params.textDocument.uri)) {
        auto& [project, path] = *resolution;
        const auto* file = project.program.GetFile(path);
        if (file) {
          // VLS_INFO("LOCK :: {}#'{}'", project.Name(), file->path);
          return f(params, *file, std::move(d));
        }
      }
      return std::nullopt;
    });
    return std::nullopt;
  }
  template <IsDocumentBoundParams Params>
  void WithFile(const Params& params, mp::Consumer<const Params&, const core::SourceFile&, LsSessionRef> auto f) {
    struct Stub {};
    WithFile<Stub>(params, [&](const Params& params, const core::SourceFile& file, LsSessionRef&& d) {
      f(params, file, std::forward<LsSessionRef>(d));
      return Stub{};
    });
  }

  [[nodiscard]] std::optional<std::pair<tooling::SolutionProject&, std::string>> ResolveFileUri(
      std::string_view file_uri) {
    return ls::ResolveFileUri(*solution, file_uri);
  }
  [[nodiscard]] std::string PathToFileUri(std::string_view path) const {
    return ls::PathToFileUri(*solution, path);
  }

 private:
  tbb::enumerable_thread_specific<lib::Arena> temporary_arena_;
};

}  // namespace vanadium::ls
