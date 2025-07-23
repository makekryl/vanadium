#include "detail/InlayHint.h"

#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <vector>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "ScopedNodeVisitor.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "detail/LanguageServerConv.h"

namespace vanadium::ls {

template <>
rpc::ExpectedResult<lsp::InlayHintResult> methods::textDocument::inlayHint::operator()(
    LsContext& ctx, const lsp::InlayHintParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  if (!file->module) {
    return nullptr;
  }

  const auto requested_range = conv::FromLSPRange(params.range, file->ast);

  std::vector<lsp::InlayHint> hints;

  const core::semantic::Scope* scope{nullptr};
  core::semantic::InspectScope(
      file->module->scope,
      [&](const core::semantic::Scope* scope_under_inspection) {
        scope = scope_under_inspection;
      },
      [&](const core::ast::Node* n) -> bool {
        if (requested_range.Contains(n->nrange)) {
          detail::ComputeInlayHint(file, scope, n, ctx->TemporaryArena(), hints);
          return true;
        }
        return n->nrange.Contains(requested_range);
      });

  return hints;
}
}  // namespace vanadium::ls
