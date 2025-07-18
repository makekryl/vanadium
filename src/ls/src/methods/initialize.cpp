#include <filesystem>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Solution.h"

namespace vanadium::ls {

template <>
rpc::ExpectedResult<lsp::InitializeResult> methods::initialize::operator()(LsContext& ctx,
                                                                           const lsp::InitializeParams& params) {
  if (params.workspaceFolders) {
    const auto& folders = *params.workspaceFolders;

    // TODO
    const auto prefix_size = std::string_view{"file://"}.size();
    const std::filesystem::path root_directory{folders[0].uri.substr(prefix_size, folders[0].uri.size() - prefix_size)};

    auto manifest_path = root_directory / tooling::Project::kManifestFilename;
    if (std::filesystem::exists(manifest_path)) {
      auto result = tooling::Solution::Load(root_directory);
      if (result.has_value()) {
        ctx->solution = std::move(*result);
      } else {
        std::fprintf(stderr, "Failed to load solution: %s", result.error().What().c_str());
        std::fflush(stderr);
        std::abort();
      }
    }
  }

  return lsp::InitializeResult{
      .capabilities =
          lsp::ServerCapabilities{
              .textDocumentSync = lsp::TextDocumentSyncKind::kIncremental,
              .completionProvider =
                  lsp::CompletionOptions{
                      .triggerCharacters = {{"."}},
                  },
              .hoverProvider = true,
              .definitionProvider = true,
              .typeDefinitionProvider = true,
              .referencesProvider = true,
              .documentHighlightProvider = true,
              .codeActionProvider = true,
              .renameProvider = true,
              // .diagnosticProvider =
              //     lsp::DiagnosticOptions{
              //         .identifier = "vanadium",
              //         .interFileDependencies = true,
              //     },
          },
      .serverInfo =
          lsp::ServerInfo{
              .name = "vanadiumd",
              .version = "0.0.0.3",
          },
  };
}

}  // namespace vanadium::ls
