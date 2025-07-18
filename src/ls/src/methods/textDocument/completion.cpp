#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <print>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::CompletionResult> methods::textDocument::completion::operator()(
    LsContext& ctx, const lsp::CompletionParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  // params.

  std::vector<lsp::CompletionItem> items;

  return items;
}
}  // namespace vanadium::ls
