#include <filesystem>
#include <print>

#include "Filesystem.h"
#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerLogger.h"
#include "LanguageServerMethods.h"
#include "Program.h"
#include "Solution.h"
#include "impl/SystemFS.h"

namespace vanadium::ls {

template <>
rpc::ExpectedResult<lsp::InitializeResult> methods::initialize::operator()(LsContext& ctx,
                                                                           const lsp::InitializeParams& params) {
  VLS_INFO("Initializing... (jobs={}, concurrency={})", ctx->task_arena.max_concurrency(),
           ctx.GetConnectionConcurrency());

  if (params.workspaceFolders) {
    const auto& folders = *params.workspaceFolders;

    // TODO
    const auto prefix_size = std::string_view{"file://"}.size();
    const std::filesystem::path root_directory{folders[0].uri.substr(prefix_size, folders[0].uri.size() - prefix_size)};

    const auto manifest_path = root_directory / tooling::Project::kManifestFilename;
    if (std::filesystem::exists(manifest_path)) {
      const auto precommit = [&](tooling::Solution& solution) {
        for (auto& proj : solution.Projects()) {
          for (const auto& file : proj.program.Files() | std::views::values) {
            const_cast<core::SourceFile&>(file).skip_analysis = true;
          }
        }
      };
      auto result =
          tooling::Solution::Load(tooling::fs::Root<tooling::fs::SystemFS>(root_directory.string()), precommit);
      if (result.has_value()) {
        ctx->solution = std::move(*result);
      } else {
        // TODO
        std::println(stderr, "Failed to load solution: {}", result.error().String());
        std::fflush(stderr);
        std::abort();
      }
    }
  }

  if (ctx->solution.has_value()) {
    const auto& solution = *ctx->solution;
    VLS_INFO("Loaded solution with {} subprojects", solution.Projects().size());
    for (const auto& project : solution.Projects()) {
      VLS_INFO(" * '{}' [{}]: {} units", project.Name(), project.Directory().base_path, project.program.Files().size());
    }
  }

  return lsp::InitializeResult{
      .capabilities =
          lsp::ServerCapabilities{
              .textDocumentSync = lsp::TextDocumentSyncKind::kFull,  // TODO: diagnose & fix incremental sync
              .completionProvider =
                  lsp::CompletionOptions{
                      .triggerCharacters = {{"."}},
                      .resolveProvider = true,
                  },
              .hoverProvider = true,
              .signatureHelpProvider =
                  lsp::SignatureHelpOptions{
                      .triggerCharacters = {{"(", ")", "{", "}", "<", ">", ","}},
                  },
              .definitionProvider = true,
              .typeDefinitionProvider = true,
              .referencesProvider = true,
              .documentHighlightProvider = true,
              .documentSymbolProvider = true,
              .codeActionProvider = true,
              .renameProvider = true,
              .semanticTokensProvider =
                  lsp::SemanticTokensOptions{
                      .legend =
                          {
                              .tokenTypes = lsp::kBuiltinSemanticTokenTypes,
                              .tokenModifiers = lsp::kBuiltinSemanticTokenModifiers,
                          },
                      .range = true,
                  },
              .inlayHintProvider =
                  lsp::InlayHintOptions{
                      .resolveProvider = true,
                  },
              // .diagnosticProvider =
              //     lsp::DiagnosticOptions{
              //         .identifier = "vanadium",
              //         .interFileDependencies = true,
              //     },
          },
      .serverInfo =
          lsp::ServerInfo{
              .name = "vanadiumd",
              .version = "0.0.0.5",
          },
  };
}

}  // namespace vanadium::ls
