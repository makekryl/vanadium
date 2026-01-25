#include <LSProtocol.h>
#include <vanadium/core/Program.h>

#include <filesystem>

#include "Filesystem.h"
#include "Solution.h"
#include "impl/SystemFS.h"
#include "vanadium/ls/LanguageServerClientMessaging.h"
#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerLogger.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/LanguageServerTestFlags.h"

namespace vanadium::ls {

namespace testflags {
bool do_not_skip_full_analysis{false};
}

rpc::ExpectedResult<lsp::InitializeResult> methods::initialize::invoke(LsContext& ctx,
                                                                       const lsp::InitializeParams& params) {
  VLS_INFO("Initializing... (jobs={}, concurrency={})", ctx.task_arena.max_concurrency(),
           ctx.connection->GetConcurrency());

  if (params.workspaceFolders) {
    const auto& folders = *params.workspaceFolders;

    // TODO
    const auto prefix_size = std::string_view{"file://"}.size();
    const std::filesystem::path root_directory{folders[0].uri.substr(prefix_size, folders[0].uri.size() - prefix_size)};

    const auto manifest_path = root_directory / tooling::Project::kManifestFilename;
    if (std::filesystem::exists(manifest_path)) {
      const auto precommit = [&](tooling::Solution& solution) {
        if (testflags::do_not_skip_full_analysis) {
          return;
        }
        for (auto& proj : solution.Projects()) {
          for (const auto& file : proj.program.Files() | std::views::values) {
            const_cast<core::SourceFile&>(file).skip_analysis = true;
          }
        }
      };
      auto result =
          tooling::Solution::Load(tooling::fs::Root<tooling::fs::SystemFS>(root_directory.string()), precommit);
      if (result.has_value()) {
        ctx.solution = std::move(*result);
      } else {
        const auto& err_message = result.error().String();
        VLS_ERROR("Failed to load solution: {}", err_message);
        clientMessaging::ShowMessage(
            ctx, lsp::ShowMessageRequestParams{
                     .type = lsp::MessageType::kError,
                     .message = ctx.Temp<std::string>(std::format("Solution initialization failed:\n{}", err_message)),
                 });
      }
    }
  }

  if (ctx.solution.has_value()) {
    const auto& solution = *ctx.solution;
    VLS_INFO("Loaded solution with {} projects", solution.Projects().size());
    std::size_t total_units{0};
    for (const auto& project : solution.Projects()) {
      const std::size_t units = project.program.Files().size();
      total_units += units;
      VLS_INFO(" * '{}' [{}]: {} units", project.Name(), project.Directory().base_path, units);
    }
    VLS_INFO(" # Total units: {}", total_units);
  }

  return lsp::InitializeResult{
      .capabilities =
          lsp::ServerCapabilities{
              .textDocumentSync = lsp::TextDocumentSyncKind::kFull,
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
                          lsp::SemanticTokensLegend{
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
              .version = "0.0.1.0",
          },
  };
}

}  // namespace vanadium::ls
