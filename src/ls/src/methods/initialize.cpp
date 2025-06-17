#include <filesystem>

#include "LSProtocol.h"
#include "LanguageServerMethods.h"
#include "Project.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::InitializeResult> methods::initialize::operator()(LsContext& ctx,
                                                                           const lsp::InitializeParams& params) {
  if (params.workspaceFolders) {
    const auto& folders = *params.workspaceFolders;

    // TODO
    const auto prefix_size = std::string_view{"file://"}.size();

    std::string path(folders[0].uri.substr(prefix_size, folders[0].uri.size() - prefix_size));
    ctx->project = *tooling::Project::Load(std::filesystem::path(path) / ".vanadiumrc.yml");

    const auto& project = *ctx->project;

    const auto read_file = [&](std::string_view path, vanadium::lib::Arena& arena) -> std::string_view {
      const auto contents = project.GetFS().ReadFile(path, [&](std::size_t size) {
        return arena.AllocStringBuffer(size).data();
      });
      if (!contents) {
        // TODO
        std::abort();
      }
      return *contents;
    };
    ctx->program.Commit([&](const auto& modify) {
      project.VisitFiles([&](const std::string& path) {
        modify.update(path, read_file);
      });
    });
  }

  return lsp::InitializeResult{
      .capabilities =
          lsp::ServerCapabilities{
              .textDocumentSync = lsp::TextDocumentSyncKind::kFull,
              .definitionProvider = true,
              // .codeActionProvider = true,
              .diagnosticProvider =
                  lsp::DiagnosticOptions{
                      .identifier = "vanadium",
                      .interFileDependencies = true,
                  },
          },
      .serverInfo =
          lsp::ServerInfo{
              .name = "vanadiumd",
              .version = "0.0.0.3",
          },
  };
}
}  // namespace vanadium::ls
