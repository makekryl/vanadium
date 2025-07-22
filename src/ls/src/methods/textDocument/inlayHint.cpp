#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <ranges>
#include <vector>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "detail/LanguageServerConv.h"
#include "detail/LanguageServerSymbolDef.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {

template <>
rpc::ExpectedResult<lsp::InlayHintResult> methods::textDocument::inlayHint::operator()(
    LsContext& ctx, const lsp::InlayHintParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  const auto requested_range = conv::FromLSPRange(params.range, file->ast);

  std::vector<lsp::InlayHint> hints;

  file->ast.root->Accept([&](const core::ast::Node* n) {
    // if (!n->nrange.Contains(requested_range)) {
    //   return false;
    // }
    // if (!requested_range.Contains(n->nrange)) {
    //   return true;
    // }

    switch (n->nkind) {
      case core::ast::NodeKind::CallExpr: {
        // PoC. TODO: iterate scopes like in typechecker, then visit their containers
        const auto* scope = core::semantic::utils::FindScope(file->module->scope, n);

        const auto* m = n->As<core::ast::nodes::CallExpr>();
        const auto* params = core::checker::utils::ResolveCallableParams(file, scope, m->args);
        if (!params) {
          break;
        }
        const auto* params_file = core::ast::utils::SourceFileOf(params);

        for (const auto& [idx, arg] : m->args->list | std::views::enumerate) {
          if (idx >= std::ssize(params->list)) {
            break;
          }
          if (arg->nkind == core::ast::NodeKind::AssignmentExpr) {
            break;
          }

          const auto& param_name_opt = params->list[idx]->name;
          if (!param_name_opt) [[unlikely]] {
            break;
          }

          const auto param_name = params_file->Text(*param_name_opt);
          if (param_name == file->Text(arg)) {
            continue;
          }

          hints.emplace_back(lsp::InlayHint{
              .position = conv::ToLSPPosition(file->ast.lines.Translate(arg->nrange.begin)),
              .label = ctx->Temp<std::string>(std::format("{} := ", param_name)),
              .kind = lsp::InlayHintKind::kParameter,
          });
        }

        break;
      }
      case core::ast::NodeKind::CompositeLiteral: {
        // PoC. TODO: iterate scopes like in typechecker, then visit their containers
        const auto* scope = core::semantic::utils::FindScope(file->module->scope, n);

        const auto* m = n->As<core::ast::nodes::CompositeLiteral>();
        const auto* sym = core::checker::DeduceCompositeLiteralType(file, scope, m);
        if (!sym || (sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
          break;
        }

        const auto* decl = sym->Declaration();
        if (decl->nkind != core::ast::NodeKind::StructTypeDecl) {
          break;
        }
        const auto* stdecl = decl->As<core::ast::nodes::StructTypeDecl>();
        const auto* stdecl_file = core::ast::utils::SourceFileOf(stdecl);

        for (const auto& [idx, arg] : m->list | std::views::enumerate) {
          if (idx >= std::ssize(stdecl->fields)) {
            break;
          }
          if (arg->nkind == core::ast::NodeKind::AssignmentExpr) {
            break;
          }

          const auto& param_name_opt = stdecl->fields[idx]->name;
          if (!param_name_opt) [[unlikely]] {
            break;
          }

          const auto param_name = stdecl_file->Text(*param_name_opt);
          if (param_name == file->Text(arg)) {
            continue;
          }

          hints.emplace_back(lsp::InlayHint{
              .position = conv::ToLSPPosition(file->ast.lines.Translate(arg->nrange.begin)),
              .label = ctx->Temp<std::string>(std::format("{} := ", param_name)),
              .kind = lsp::InlayHintKind::kParameter,
          });
        }

        break;
      }
      default:
        break;
    }
    return true;
  });

  return hints;
}
}  // namespace vanadium::ls
