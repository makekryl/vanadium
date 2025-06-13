#include "JsonRpc.h"
#include "LSProtocol.h"
#include "LanguageServerProcedures.h"

namespace vanadium::ls::procedures {
void initialize(VanadiumLsContext& ctx, lserver::PooledMessageToken&& token) {
  jsonrpc::Request req(token->tree);
  auto res = ctx.AcquireToken();

  lsp::InitializeResult initialize_result(jsonrpc::Response::Success(res->tree, req.id()).result());
  initialize_result.init();

  // ir.capabilities().add_hoverProvider().jsonNode().set_type(ryml::VAL);
  // ir.capabilities().hoverProvider().set_boolean(true);

  auto diag = initialize_result.capabilities().add_diagnosticProvider();
  diag.jsonNode() |= ryml::MAP;
  diag.as_DiagnosticRegistrationOptions().set_interFileDependencies(true);
  diag.as_DiagnosticRegistrationOptions().set_workspaceDiagnostics(true);

  auto sync = initialize_result.capabilities().add_textDocumentSync();
  sync.jsonNode().set_type(ryml::KEYVAL);
  lsp::detail::SerializeEnum(lsp::TextDocumentSyncKind::kFull, sync.jsonNode());

  initialize_result.add_serverInfo().set_name("vanadiumd");
  initialize_result.serverInfo().set_version("0.0.0.1");

  ctx.Send(std::move(res));
}
}  // namespace vanadium::ls::procedures
