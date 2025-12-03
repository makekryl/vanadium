#include <magic_enum/magic_enum.hpp>
#include <utility>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerLogger.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "Program.h"

namespace vanadium::ls {

namespace {
[[nodiscard]] core::Program* HandleChange(LsContext& ctx, const lsp::FileEvent& event) {
  VLS_INFO("{}: '{}'", magic_enum::enum_name(event.type), event.uri);

  auto resolution = ctx.ResolveFileUri(event.uri);
  if (!resolution) {
    return nullptr;
  }

  auto& [project, path] = *resolution;
  auto& program = project.program;

  switch (event.type) {
    case lsp::FileChangeType::kCreated:
    case lsp::FileChangeType::kChanged: {
      const auto read_file = [&](const std::string& path, std::string& srcbuf) -> void {
        const auto res = ctx.solution->Directory().ReadFile(path, [&](std::size_t size) {
          srcbuf.resize(size);
          return srcbuf.data();
        });
        if (!res) [[unlikely]] {
          VLS_ERROR("Failed to read file '{}': '{}'", path, res.error().String());
        }
      };
      // TODO: improvement requied - program.Update is parallelized internally,
      //       but we call HandleChange sequentially, limiting the performance
      program.Update([&](auto& modify) {
        modify.update(path, read_file);
      });
      return &program;
    }
    case lsp::FileChangeType::kDeleted: {
      const auto* sf = program.GetFile(path);
      if (!sf) {
        VLS_WARN("Received lsp::FileEvent with type kDeleted for unknown file: '{}'", path);
        return nullptr;
      }
      // TODO: same as the above
      program.Update([&](auto& modify) {
        modify.drop(path);
      });
      return &program;
    }
    default:
      VLS_ERROR("Bad lsp::FileEvent type: {}", std::to_underlying(event.type));
      return nullptr;
  }
}
}  // namespace

template <>
void methods::workspace::didChangeWatchedFiles::invoke(LsContext& ctx, const lsp::DidChangeWatchedFilesParams& params) {
  ctx.LockData([&](LsSessionRef) {
    if (params.changes.size() == 1) {
      if (auto* program = HandleChange(ctx, params.changes.front()); program) {
        program->Commit([](auto&) {});
      }
      return;
    }
    std::unordered_set<core::Program*> affected_programs;
    for (const auto& change : params.changes) {
      if (auto* program = HandleChange(ctx, change); program) {
        affected_programs.emplace(program);
      }
    }
    // TODO: topological sort (+ in solution, should speed up init)
    for (auto* program : affected_programs) {
      program->Commit([](auto&) {});
    }
  });
}
}  // namespace vanadium::ls
