#include <concepts>
#include <utility>

#include "ASTNodes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerLogger.h"
#include "LanguageServerMethods.h"
#include "Program.h"
#include "ScopedNodeVisitor.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "detail/InlayHint.h"
#include "utils/ASTUtils.h"

namespace lsp {
inline SemanticTokenModifiers operator|(SemanticTokenModifiers lhs, SemanticTokenModifiers rhs) {
  using U = std::underlying_type_t<SemanticTokenModifiers>;
  return static_cast<SemanticTokenModifiers>(static_cast<U>(lhs) | static_cast<U>(rhs));
}
}  // namespace lsp

namespace vanadium::ls {
// TODO: refactor
struct SemanticTokenW {
  const core::ast::nodes::Ident* ident;
  lsp::SemanticTokenTypes type;
  lsp::SemanticTokenModifiers modifiers;
};

void HighlightIdent(const core::SourceFile* file, const core::semantic::Scope* scope,
                    const core::ast::nodes::Ident* ident, std::invocable<SemanticTokenW&&> auto write_token) {
  if (ident->parent->nkind == core::ast::NodeKind::AssignmentExpr) {
    const auto* ae = ident->parent->As<core::ast::nodes::AssignmentExpr>();
    if (ident == ae->property && ae->parent->nkind == core::ast::NodeKind::CompositeLiteral) {
      write_token(SemanticTokenW{
          .ident = ident,
          .type = lsp::SemanticTokenTypes::kProperty,
          .modifiers = lsp::SemanticTokenModifiers::kUnset,
      });
      return;
    }
  }
  if (ident->parent->nkind == core::ast::NodeKind::EnumTypeDecl ||
      (ident->parent->nkind == core::ast::NodeKind::CallExpr &&
       ident->parent->parent->nkind == core::ast::NodeKind::EnumTypeDecl)) {
    write_token(SemanticTokenW{
        .ident = ident,
        .type = lsp::SemanticTokenTypes::kEnummember,
        .modifiers = lsp::SemanticTokenModifiers::kUnset,
    });
    return;
  }
  if (const auto sym = core::checker::ResolveExprSymbol(file, scope, ident); sym) {
    SemanticTokenW w{.ident = ident};
    using Fu = std::pair<lsp::SemanticTokenTypes, lsp::SemanticTokenModifiers>;
    std::tie(w.type, w.modifiers) = [&] -> Fu {
      if (sym->Flags() & core::semantic::SymbolFlags::kTemplate) {
        return {lsp::SemanticTokenTypes::kVariable,
                (ident->parent->nkind == core::ast::NodeKind::FuncDecl ? lsp::SemanticTokenModifiers::kDeclaration
                                                                       : lsp::SemanticTokenModifiers::kUnset) |
                    lsp::SemanticTokenModifiers::kReadonly};
      }
      if (sym->Flags() & core::semantic::SymbolFlags::kFunction) {
        return {lsp::SemanticTokenTypes::kFunction,
                (ident->parent->nkind == core::ast::NodeKind::FuncDecl ? lsp::SemanticTokenModifiers::kDeclaration
                                                                       : lsp::SemanticTokenModifiers::kUnset)};
      }
      if (sym->Flags() & core::semantic::SymbolFlags::kVariable) {
        const auto* decl = sym->Declaration();
        if (decl->nkind == core::ast::NodeKind::Declarator) {
          const auto* dd = decl->As<core::ast::nodes::Declarator>();
          const auto* vd = dd->parent->As<core::ast::nodes::ValueDecl>();
          if (vd->kind && vd->kind->kind != core::ast::TokenKind::VAR) {  // modulepar+const, todo: refactor
            return {lsp::SemanticTokenTypes::kVariable,
                    lsp::SemanticTokenModifiers::kDefinition | lsp::SemanticTokenModifiers::kReadonly};
          }
          return {lsp::SemanticTokenTypes::kVariable, lsp::SemanticTokenModifiers::kDeclaration};
        }
        return {lsp::SemanticTokenTypes::kVariable, lsp::SemanticTokenModifiers::kUnset};
      }
      if (sym->Flags() & core::semantic::SymbolFlags::kArgument) {
        return {lsp::SemanticTokenTypes::kParameter,
                (ident->parent->nkind == core::ast::NodeKind::FormalPar ? lsp::SemanticTokenModifiers::kDeclaration
                                                                        : lsp::SemanticTokenModifiers::kUnset)};
      }
      if (sym->Flags() & (core::semantic::SymbolFlags::kComponent | core::semantic::SymbolFlags::kClass)) {
        return {lsp::SemanticTokenTypes::kClass, lsp::SemanticTokenModifiers::kUnset};
      }
      if (sym->Flags() & (core::semantic::SymbolFlags::kEnum)) {
        return {lsp::SemanticTokenTypes::kEnum, lsp::SemanticTokenModifiers::kUnset};
      }
      if (sym->Flags() & (core::semantic::SymbolFlags::kEnumMember)) {
        return {lsp::SemanticTokenTypes::kEnummember, lsp::SemanticTokenModifiers::kUnset};
      }
      if (sym->Flags() & (core::semantic::SymbolFlags::kStructural)) {
        return {lsp::SemanticTokenTypes::kStruct, lsp::SemanticTokenModifiers::kUnset};
      }
      if (sym->Flags() & core::semantic::SymbolFlags::kField) {
        return {lsp::SemanticTokenTypes::kType, lsp::SemanticTokenModifiers::kDeclaration};
      }
      if (sym->Flags() & core::semantic::SymbolFlags::kSubtype) {
        return {lsp::SemanticTokenTypes::kType, lsp::SemanticTokenModifiers::kUnset};
      }
      if (sym->Flags() & core::semantic::SymbolFlags::kPort) {
        return {lsp::SemanticTokenTypes::kClass, lsp::SemanticTokenModifiers::kUnset};
      }
      if (sym->Flags() & core::semantic::SymbolFlags::kType) {
        return {lsp::SemanticTokenTypes::kType, lsp::SemanticTokenModifiers::kUnset};
      }
      if (sym->Flags() & core::semantic::SymbolFlags::kImportedModule) {
        return {lsp::SemanticTokenTypes::kNamespace, lsp::SemanticTokenModifiers::kUnset};
      }

      // regexp lol todo
      return {lsp::SemanticTokenTypes::kRegexp, lsp::SemanticTokenModifiers::kUnset};
    }();
    // regexp lol todo
    if (w.type != lsp::SemanticTokenTypes::kRegexp) {
      write_token(std::move(w));
    }
  }
}

lsp::SemanticTokens CollectSemanticTokens(const lsp::SemanticTokensRangeParams& params, const core::SourceFile& file,
                                          LsSessionRef) {
  if (!file.module) {
    return {};
  }

  lsp::SemanticTokens result;
  lsp::Position lastpos = {.line = 0, .character = 0};
  const auto write_token = [&](SemanticTokenW&& tok) {
    const auto pos = conv::ToLSPPosition(file.ast.lines.Translate(tok.ident->nrange.begin));

    result.data.reserve(result.data.size() + 5);

    const auto relline = pos.line - lastpos.line;
    if (relline != 0) {
      lastpos.character = 0;
    }
    result.data.emplace_back(relline);
    result.data.emplace_back(pos.character - lastpos.character);
    result.data.emplace_back(tok.ident->nrange.Length());
    result.data.emplace_back(static_cast<std::uint32_t>(tok.type));
    result.data.emplace_back(static_cast<std::uint32_t>(tok.modifiers));

    lastpos = pos;
  };

  const auto requested_range = conv::FromLSPRange(params.range, file.ast);
  const auto overlaps = [](const core::ast::Range& a, const core::ast::Range& b) {
    return std::max(a.begin, b.begin) <= std::min(a.end, b.end);
  };

  const core::semantic::Scope* scope{nullptr};
  const auto inspector = [&](this auto&& self, const core::ast::Node* n) -> bool {
    if (overlaps(requested_range, n->nrange)) {
      switch (n->nkind) {
        case core::ast::NodeKind::Ident: {
          HighlightIdent(&file, scope, n->As<core::ast::nodes::Ident>(), write_token);
          return false;
        }
        default: {
          break;
        }
      }
      return true;
    }
    return n->nrange.Contains(requested_range);
  };
  core::semantic::InspectScope(
      file.module->scope,
      [&](const core::semantic::Scope* scope_under_inspection) {
        scope = scope_under_inspection;
      },
      inspector);

  return result;
}

template <>
rpc::ExpectedResult<lsp::SemanticTokensRangeResult> methods::textDocument::semanticTokens::range::operator()(
    LsContext& ctx, const lsp::SemanticTokensRangeParams& params) {
  return ctx->WithFile<lsp::SemanticTokensRangeResult>(params, CollectSemanticTokens).value_or(nullptr);
}
}  // namespace vanadium::ls
