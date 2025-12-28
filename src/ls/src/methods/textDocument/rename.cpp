#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>

#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "Program.h"
#include "Semantic.h"
#include "detail/Definition.h"

namespace vanadium::ls {

namespace {
lsp::RenameResult ProvideRename(const lsp::RenameParams& params, const core::SourceFile& file, LsSessionRef) {
  const auto symres = detail::FindSymbol(&file, params.position);
  if (!symres || !symres->scope || symres->node->nkind != ast::NodeKind::Ident) {
    return nullptr;
  }

  const auto* sym = symres->type.sym;
  const auto* scope = symres->scope;
  const auto sym_name = sym->GetName();

  if (!(sym->Flags() & (core::semantic::SymbolFlags::kVariable | core::semantic::SymbolFlags::kArgument))) {
    // TODO: support fields
    return nullptr;
  }

  lsp::WorkspaceEdit result;
  auto& changes = result.changes.emplace();
  auto& edits = changes[params.textDocument.uri];  // TODO: multifile support

  scope->Container()->Accept([&](const ast::Node* vn) {
    if (vn->nkind == ast::NodeKind::Ident && file.Text(vn) == sym_name) {
      edits.emplace_back(lsp::TextEdit{
          .range = conv::ToLSPRange(vn->nrange, file.ast),
          .newText = params.newName,
      });
    }
    return true;
  });

  return result;
}
}  // namespace

template <>
rpc::ExpectedResult<lsp::RenameResult> methods::textDocument::rename::invoke(LsContext& ctx,
                                                                             const lsp::RenameParams& params) {
  return ctx.WithFile<lsp::RenameResult>(params, ProvideRename).value_or(nullptr);
}
}  // namespace vanadium::ls
