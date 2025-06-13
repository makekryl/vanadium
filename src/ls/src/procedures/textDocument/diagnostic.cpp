#include "JsonRpc.h"
#include "LSProtocol.h"
#include "LanguageServerProcedures.h"

namespace vanadium::ls::procedures::textDocument {
void diagnostic(VanadiumLsContext& ctx, lserver::PooledMessageToken&& token) {
  jsonrpc::Request req(token->tree);
  auto res = ctx.AcquireToken();

  lsp::DocumentDiagnosticParams params(req.params());
  const auto* file = ctx->program.GetFile(std::string(params.textDocument().uri()));
  if (!file) {
    return;  // TODO
  }

  lsp::FullDocumentDiagnosticReport report(jsonrpc::Response::Success(res->tree, req.id()).result());
  report.init();
  report.jsonNode().append_child() << ryml::key("kind") << "full";

  auto items = report.items();
  for (const auto& err : file->ast.errors) {
    auto item = items.emplace();
    item.set_source("vanadium");
    item.set_severity(lsp::DiagnosticSeverity::kError);
    item.set_message(err.description);

    auto loc_begin = file->ast.lines.Translate(err.range.begin);
    item.range().start().set_line(loc_begin.line);
    item.range().start().set_character(loc_begin.column);
    auto loc_end = file->ast.lines.Translate(err.range.end);
    item.range().end().set_line(loc_end.line);
    item.range().end().set_character(loc_end.column);
  }
  if (file->module.has_value()) {
    for (const auto& err : file->module->unresolved) {
      auto item = items.emplace();
      item.set_source("vanadium");
      item.set_severity(lsp::DiagnosticSeverity::kError);
      //  item.set_message(err->On(file->ast.src));
      item.set_message(err->On(file->ast.src));

      auto loc_begin = file->ast.lines.Translate(err->nrange.begin);
      item.range().start().set_line(loc_begin.line);
      item.range().start().set_character(loc_begin.column);
      auto loc_end = file->ast.lines.Translate(err->nrange.end);
      item.range().end().set_line(loc_end.line);
      item.range().end().set_character(loc_end.column);
    }
  }
  report.jsonNode().append_child() << ryml::key("sz") << file->ast.errors.size();

  ctx.Send(std::move(res));
}
}  // namespace vanadium::ls::procedures::textDocument
