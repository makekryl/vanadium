// #include "JsonRpc.h"
// #include "LSProtocol.h"
// #include "LanguageServerDiagnostic.h"
// #include "LanguageServerMethods.h"
// #include "c4/yml/node_type.hpp"

// namespace vanadium::ls::procedures::textDocument {
// void codeAction(LsContext& ctx, lserver::PooledMessageToken&& token) {
//   jsonrpc::Request req(token->tree);
//   auto res = ctx.AcquireToken();

//   lsp::CodeActionParams params(req.params());
//   const auto* file = ctx->program.GetFile(std::string(params.textDocument().uri()));
//   if (!file) {
//     return;  // TODO
//   }

//   lsp::FullDocumentDiagnosticReport report(jsonrpc::Response::Success(res->tree, req.id()).result());
//   report.init();

//   ctx.Send(std::move(res));
// }
// }  // namespace vanadium::ls::procedures::textDocument
