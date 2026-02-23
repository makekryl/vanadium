#pragma once

#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <vanadium/lib/Arena.h>
#include <vanadium/lib/Metaprogramming.h>
#include <vanadium/lib/concurrency/ThreadSpecific.h>

#include "LanguageServerSession.h"
#include "LanguageServerSolution.h"

// A lot of stuff here is wrapped in unique_ptr to reduce build times
// by preventing the leakage of a lot of headers to a lot of unrelated TUs

namespace vanadium {

namespace core {
struct SourceFile;
}

namespace lint {
class Linter;
}

namespace format {
struct PrintOptions;
}

namespace tooling {
struct SolutionProject;
}

namespace lserver {
class Connection;
}

namespace ls {

template <typename Params>
concept IsDocumentBoundParams = requires(Params params) {
  { params.textDocument.uri } -> std::convertible_to<std::string_view>;
};

class LsContext {
 public:
  LsContext(lserver::Connection&);
  ~LsContext();

  //

  lserver::Connection* const connection;

  std::unique_ptr<tooling::Solution> solution{nullptr};
  std::unordered_map<std::string, std::int32_t> file_versions;

  const std::unique_ptr<lint::Linter> linter;
  std::unique_ptr<format::PrintOptions> fmt_opts{nullptr};

  //

  lib::Arena& TemporaryArena() {
    return temporary_arena_.Local();
  }

  template <typename T, typename... Args>
  T& Temp(Args&&... args) {
    return *TemporaryArena().Alloc<T>(std::forward<Args>(args)...);
  }

  //

  template <typename F>
    requires(std::is_invocable_v<F, LsSessionRef &&>)
  auto LockData(F f) {
    // TODO: lock data, RWmutex with writer preference
    return f({.solution = *solution,
              .arena = TemporaryArena(),
              .tools = {
                  .linter = *linter,
                  .fmt_opts = fmt_opts.get(),
              }});
  }

  template <typename Result, IsDocumentBoundParams Params>
    requires(!std::is_same_v<Result, void>)
  std::optional<Result> WithFile(const Params& params,
                                 mp::Invocable<Result, const Params&, const core::SourceFile&, LsSessionRef> auto f) {
    return LockData([&](LsSessionRef&& d) -> std::optional<Result> {
      if (const auto* file = ResolveSourceFile(params.textDocument.uri)) {
        // VLS_INFO("LOCK :: {}#'{}'", project.Name(), file->path);
        return f(params, *file, std::move(d));
      }
      return std::nullopt;
    });
  }
  template <IsDocumentBoundParams Params>
  void WithFile(const Params& params, mp::Consumer<const Params&, const core::SourceFile&, LsSessionRef> auto f) {
    struct Stub {};
    WithFile<Stub>(params, [&](const Params& params, const core::SourceFile& file, LsSessionRef&& d) {
      f(params, file, std::forward<LsSessionRef>(d));
      return Stub{};
    });
  }

  [[nodiscard]] const core::SourceFile* ResolveSourceFile(std::string_view file_uri) const;
  [[nodiscard]] std::optional<std::pair<tooling::SolutionProject&, std::string>> ResolveFileUri(
      std::string_view file_uri) const {
    return ls::ResolveFileUri(*solution, file_uri);
  }
  [[nodiscard]] std::string PathToFileUri(std::string_view path) const {
    return ls::PathToFileUri(*solution, path);
  }

 private:
  lib::concurrency::ThreadSpecific<lib::Arena> temporary_arena_;
};

}  // namespace ls

}  // namespace vanadium
