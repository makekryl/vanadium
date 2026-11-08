#include "vanadium/ls/actions/AddMissingArguments.h"

#include <algorithm>
#include <format>
#include <iterator>
#include <string_view>

#include <magic_enum/magic_enum.hpp>

#include <LSProtocol.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/core/utils/SemanticUtils.h>
#include <vanadium/core/utils/WithUtils.h>
#include <vanadium/lib/ArenaString.h>
#include <vanadium/lib/Bitset.h>
#include <vanadium/lib/FunctionRef.h>
#include <vanadium/lib/Metaprogramming.h>

#include "vanadium/ls/LanguageServerConv.h"
#include "vanadium/ls/LanguageServerLogger.h"

namespace vanadium::ls::actions {

namespace {
template <ast::IsNode TParamDescriptorNode>
lib::Bitset CollectPresenceMask(const core::SourceFile& params_file, ast::NodeSpan<TParamDescriptorNode> params,
                                const core::SourceFile& args_file, ast::NodeSpan<ast::nodes::Expr> arglist) {
  lib::Bitset mask(params.size());
  std::int64_t last_arg_idx{0};
  for (const auto* arg : arglist) {
    if (const auto* ae = arg->Maybe<ast::nodes::AssignmentExpr>()) {
      last_arg_idx = -1;
      const auto par_it = std::ranges::find_if(params, [&](const TParamDescriptorNode* par) {
        return params_file.Text(*par->name) == args_file.Text(ae->property);
      });
      if (par_it == params.end()) {
        continue;
      }
      mask.Set(par_it - params.begin());
    } else if (last_arg_idx != -1) {
      mask.Set(last_arg_idx);
      ++last_arg_idx;
    }
  }
  return mask;
}

ast::pos_t FindInsertionPos(const core::SourceFile& file, ast::pos_t start, ast::pos_t end) {
  ast::pos_t pos = start;
  while (pos < end) {
    auto c = file.src[pos];
    if (c == ',') {
      return pos + 1;
    }
    ++pos;
  }
  return pos;
}

template <ast::IsNode TParamDescriptorNode, ast::IsNode TArgContainerNode>
void GatherEdits(std::vector<lsp::TextEdit>& edits, const core::SourceFile& params_file,
                 ast::NodeSpan<TParamDescriptorNode> params, const core::SourceFile& args_file,
                 const TArgContainerNode* args, LsSessionRef& d,
                 const mp::Predicate<const TParamDescriptorNode*> auto& is_mandatory_param) {
  const auto present_args = CollectPresenceMask(params_file, params, args_file, args->list);

  ast::pos_t insertion_pos = args->nrange.begin + 1;  // `(` + 1
  auto it = args->list.begin();
  for (std::size_t i = 0; i < present_args.Size();) {
    if (present_args.Get(i)) {
      insertion_pos = FindInsertionPos(args_file, (*it)->nrange.end, args->nrange.end);
      ++it;
      ++i;
      continue;
    }

    auto& buf = *d.arena.Alloc<std::string>();
    for (; i < present_args.Size() && !present_args.Get(i); ++i) {
      const auto* par = params[i];
      if (is_mandatory_param(par)) {
        std::format_to(std::back_inserter(buf), "{} := -, ", params_file.Text(*par->name));
      }
    }

    if (!buf.empty()) {
      if (it == args->list.end()) {
        // drop trailing ', '
        buf.pop_back();
        buf.pop_back();
        if (it != args->list.begin()) {
          // ...and prepend it instead
          buf.insert(0, ", ");
        }
      } else if (it != args->list.begin()) {
        buf.pop_back();  // ' '
        buf.insert(0, " ");
      }
      const auto loc = conv::ToLSPPosition(args_file.ast.lines.Translate(insertion_pos));
      edits.push_back(lsp::TextEdit{
          .range = {.start = loc, .end = loc},
          .newText = buf,
      });
    }
  }
}
}  // namespace

void AddMissingArguments::ResolveDiagnostic(std::vector<lsp::CodeAction>& result, const lsp::CodeActionParams& params,
                                            const lsp::Diagnostic& diag, const core::SourceFile& file,
                                            LsSessionRef& d) {
  if (!diag.message.contains("too few arguments") && !diag.message.contains("too few mandatory arguments")) {
    return;
  }

  const auto* n = ast::utils::GetNodeAt(file.ast, file.ast.lines.GetPosition(ast::Location{
                                                      .line = diag.range.start.line,
                                                      .column = diag.range.start.character + 1,
                                                  }));
  const auto* scope = core::semantic::utils::FindScope(file.module->scope, n);

  std::vector<lsp::TextEdit> edits;

  switch (n->nkind) {
    case ast::NodeKind::ParenExpr: {
      const auto* pe = n->As<ast::nodes::ParenExpr>();
      const auto* pars = core::checker::utils::ResolveCallableParams(&file, scope, pe);
      if (!pars) {
        VLS_WARN("{} failed to get pars, parent nkind={}", __FILE__, magic_enum::enum_name(n->parent->nkind));
        break;
      }
      const auto& pars_file = *ast::utils::SourceFileOf(pars);
      GatherEdits(edits, pars_file, pars->list, file, pe, d, [](const ast::nodes::FormalPar* par) {
        return par->value == nullptr;
      });
      break;
    }
    case ast::NodeKind::CompositeLiteral: {
      const auto* cl = n->As<ast::nodes::CompositeLiteral>();
      auto itype = core::checker::ext::DeduceCompositeLiteralType(&file, scope, cl);
      if (!itype || !(itype->Flags() & core::semantic::SymbolFlags::kStructural)) {
        break;
      }
      const auto* typedecl = itype->Declaration();
      if (typedecl->nkind != ast::NodeKind::StructTypeDecl) {
        break;
      }
      const auto& fields = typedecl->As<ast::nodes::StructTypeDecl>()->fields;
      const auto& fields_file = *ast::utils::SourceFileOf(typedecl);

      bool implicit_omit = false;
      if (const auto* pn = ast::utils::GetAnyPredecessor<ast::nodes::TemplateDecl, ast::nodes::CallExpr>(n);
          pn && pn->nkind == ast::NodeKind::TemplateDecl) {
        implicit_omit = core::utils::ResolveTemplateOmitMode(file.ast, pn->As<ast::nodes::TemplateDecl>()) ==
                        core::utils::TemplateOmitMode::kImplicit;
      }

      GatherEdits(edits, fields_file, fields, file, cl, d, [&](const ast::nodes::Field* field) {
        return implicit_omit ? !field->optional : true;
      });
      break;
    }
    default:
      VLS_WARN("{} unsupported node {}", __FILE__, magic_enum::enum_name(n->nkind));
      break;
  }

  result.emplace_back(lsp::CodeAction{
      .title = "Add missing arguments",
      .kind = lsp::CodeActionKind::kQuickfix,
      .isPreferred = true,
      .edit =
          lsp::WorkspaceEdit{
              .changes = {{{params.textDocument.uri, std::move(edits)}}},
          },
  });
}

}  // namespace vanadium::ls::actions
