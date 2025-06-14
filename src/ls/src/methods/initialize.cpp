#include "LSProtocol.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::InitializeResult> methods::initialize::operator()(VanadiumLsContext&,
                                                                           const lsp::InitializeParams& params) {
  return lsp::InitializeResult{
      .capabilities =
          lsp::ServerCapabilities{
              .textDocumentSync = lsp::TextDocumentSyncKind::kFull,
              // .codeActionProvider = true,
              .diagnosticProvider =
                  lsp::DiagnosticRegistrationOptions{
                      // .identifier = "vanadium",
                      .interFileDependencies = true,
                      .workspaceDiagnostics = true,
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
