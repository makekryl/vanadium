#include "detail/InlayHint.h"

#include <algorithm>
#include <concepts>
#include <stack>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LanguageServerConv.h"
#include "Program.h"
#include "ScopedNodeVisitor.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "detail/Definition.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls::detail {

struct InlayHintPayload {
  std::string path;
  core::ast::pos_t apos;
  std::underlying_type_t<core::ast::NodeKind> nk;
};

namespace {
template <typename DeduceCompositeLiteralTypeFn = decltype(&core::checker::ext::DeduceCompositeLiteralType)>
  requires std::is_invocable_r_v<const core::semantic::Symbol*, DeduceCompositeLiteralTypeFn, const core::SourceFile*,
                                 const core::semantic::Scope*, const core::ast::nodes::CompositeLiteral*>
struct InlayHintTargetLocatorOptions {
  DeduceCompositeLiteralTypeFn deduceCompositeLiteralType;
};
constexpr auto kNonCachingInlayHintTargetLocatorOptions = InlayHintTargetLocatorOptions{
    .deduceCompositeLiteralType =
        [](const core::SourceFile* _file, const core::semantic::Scope* _scope,
           const core::ast::nodes::CompositeLiteral* _cl) {
          return core::checker::ext::DeduceCompositeLiteralType(_file, _scope, _cl);
        },
};

template <typename Options>
[[nodiscard]] const core::ast::Node* LocateInlayHintTarget(const core::SourceFile* file,
                                                           const core::semantic::Scope* scope, const core::ast::Node* n,
                                                           Options options) {
  switch (n->nkind) {
    case core::ast::NodeKind::CallExpr: {
      const auto* m = n->As<core::ast::nodes::CallExpr>();

      // ResolveCallableParams is not used because we need to filter out builtin functions
      // return core::checker::utils::ResolveCallableParams(file, scope, m->args);  // -> FormalPars

      const auto* fun_sym = core::checker::ResolveExprType(file, scope, m->fun);
      if (!fun_sym || (fun_sym->Flags() & core::semantic::SymbolFlags::kBuiltinDef) ||
          !(fun_sym->Flags() & (core::semantic::SymbolFlags::kFunction | core::semantic::SymbolFlags::kTemplate))) {
        return nullptr;
      }

      return core::ast::utils::GetCallableDeclParams(
          fun_sym->Declaration()->As<core::ast::nodes::Decl>());  // -> FormalPars
    }
    case core::ast::NodeKind::CompositeLiteral: {
      const auto* m = n->As<core::ast::nodes::CompositeLiteral>();
      const auto* sym = options.deduceCompositeLiteralType(file, scope, m);
      if (!sym || (sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
        return nullptr;
      }

      const auto* decl = sym->Declaration();
      if (decl->nkind != core::ast::NodeKind::StructTypeDecl) {
        return nullptr;
      }
      return decl;  // -> StructTypeDecl
    }
    default:
      return nullptr;
  }
}

template <typename InlayHintTargetLocatorOptions>
void ComputeInlayHint(const core::SourceFile* file, const core::semantic::Scope* scope, const core::ast::Node* n,
                      lib::Arena& arena, std::vector<lsp::InlayHint>& out,
                      InlayHintTargetLocatorOptions inlayHintTargetLocatorOptions) {
  const auto add_parameter_inlay_hint = [&](const core::ast::pos_t pos, std::string_view name) {
    out.emplace_back(lsp::InlayHint{
        .position = conv::ToLSPPosition(file->ast.lines.Translate(pos)),
        .label = *arena.Alloc<std::string>(std::format("{} := ", name)),
        .kind = lsp::InlayHintKind::kParameter,
        // TODO: find a way to have strongly-typed 'data' on both ends (maybe modify lspgen to produce templates,
        // exposing internal payload structure to the higher levels does not sounds good though - and it will be
        // required to do in such case)
        .data =
            glz::json_t{
                {"path", file->path},
                {"apos", n->nrange.begin},
                {"nk", std::to_underlying(n->nkind)},
            },
    });
  };

  const core::ast::Node* tgt = LocateInlayHintTarget(file, scope, n, inlayHintTargetLocatorOptions);
  if (!tgt) {
    return;
  }

  switch (n->nkind) {
    case core::ast::NodeKind::CallExpr: {
      const auto* m = n->As<core::ast::nodes::CallExpr>();

      const auto* params = tgt->As<core::ast::nodes::FormalPars>();
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

        add_parameter_inlay_hint(arg->nrange.begin, param_name);
      }

      break;
    }
    case core::ast::NodeKind::CompositeLiteral: {
      const auto* m = n->As<core::ast::nodes::CompositeLiteral>();

      const auto* stdecl = tgt->As<core::ast::nodes::StructTypeDecl>();
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

        add_parameter_inlay_hint(arg->nrange.begin, param_name);
      }

      break;
    }
    default:
      break;
  }
}
}  // namespace

[[nodiscard]] std::vector<lsp::InlayHint> CollectInlayHints(const core::SourceFile* file,
                                                            const core::ast::Range& requested_range,
                                                            lib::Arena& arena) {
  std::vector<lsp::InlayHint> hints;

  const core::semantic::Scope* scope{nullptr};

  std::stack<const core::semantic::Symbol*> cl_type_cache;
  cl_type_cache.emplace(nullptr);
  //
  const auto memoizing_visitor = [&](this auto&& self, const core::ast::Node* n) {
    ComputeInlayHint(file, scope, n, arena, hints,
                     InlayHintTargetLocatorOptions{
                         .deduceCompositeLiteralType =
                             [&](const core::SourceFile* _file, const core::semantic::Scope* _scope,
                                 const core::ast::nodes::CompositeLiteral* _cl) {
                               const auto* res = core::checker::ext::DeduceCompositeLiteralType(_file, _scope, _cl,
                                                                                                cl_type_cache.top());
                               cl_type_cache.emplace(res);
                               return res;
                             },
                     });
    n->Accept(self);
    if (n->nkind == core::ast::NodeKind::CompositeLiteral) {
      cl_type_cache.pop();
    }
    return false;
  };

  const auto overlaps = [](const core::ast::Range& a, const core::ast::Range& b) {
    return std::max(a.begin, b.begin) <= std::min(a.end, b.end);
  };
  core::semantic::InspectScope(
      file->module->scope,
      [&](const core::semantic::Scope* scope_under_inspection) {
        scope = scope_under_inspection;
      },
      [&](const core::ast::Node* n) -> bool {
        if (overlaps(requested_range, n->nrange)) {
          if (n->nkind == core::ast::NodeKind::CompositeLiteral) {
            memoizing_visitor(n);
            return false;
          }
          ComputeInlayHint(file, scope, n, arena, hints, kNonCachingInlayHintTargetLocatorOptions);
          return true;
        }
        return n->nrange.Contains(requested_range);
      });

  return hints;
}

std::optional<lsp::InlayHint> ResolveInlayHint(const tooling::Solution& solution, lib::Arena& arena,
                                               const lsp::InlayHint& original_hint) {
  if (!original_hint.data) {
    return std::nullopt;
  }
  const auto payload = glz::read_json<InlayHintPayload>(*original_hint.data);

  const auto* project = solution.ProjectOf((solution.RootDirectory() / payload->path).string());
  const auto* file = project->program.GetFile(payload->path);

  const auto* n = detail::FindNode(file, original_hint.position);
  if (!n) [[unlikely]] {
    return std::nullopt;
  }

  const core::ast::Node* container_node = core::ast::utils::GetNodeAt(file->ast, payload->apos);
  while (container_node->nkind != static_cast<core::ast::NodeKind>(payload->nk)) {
    container_node = container_node->parent;
  }

  const auto* tgt = LocateInlayHintTarget(file, core::semantic::utils::FindScope(file->module->scope, container_node),
                                          container_node, kNonCachingInlayHintTargetLocatorOptions);
  if (!tgt) {
    return std::nullopt;
  }

  const auto render = [&](std::invocable<lsp::InlayHint&> auto accept) -> lsp::InlayHint {
    lsp::InlayHint rendition{original_hint};

    if (std::holds_alternative<std::string_view>(original_hint.label)) {
      rendition.textEdits = {{lsp::TextEdit{
          .range =
              lsp::Range{
                  .start = original_hint.position,
                  .end = original_hint.position,
              },
          .newText = std::get<std::string_view>(original_hint.label),
      }}};
    }

    accept(rendition);

    return rendition;
  };

  switch (container_node->nkind) {
    case core::ast::NodeKind::CallExpr: {
      const auto* m = container_node->As<core::ast::nodes::CallExpr>();

      const auto* params = tgt->As<core::ast::nodes::FormalPars>();

      while (n->parent != m->args) {
        n = n->parent;
      }

      const auto it = std::ranges::find(m->args->list, n);
      if (it == m->args->list.end()) {  // such cases should not be possible, but check just in case to avoid crash
        return std::nullopt;
      }

      const auto idx = it - m->args->list.begin();
      if (idx >= std::ssize(params->list)) {  //
        return std::nullopt;
      }

      const auto* param = params->list[idx];
      if (!param->name) {
        return std::nullopt;
      }
      const auto* param_file = core::ast::utils::SourceFileOf(param);

      return render([&](lsp::InlayHint& rendition) {
        rendition.label = std::vector<lsp::InlayHintLabelPart>{{{
            .value = std::get<std::string_view>(rendition.label),
            .location =
                lsp::Location{
                    // TODO: unify with context, but I don't like bringing LsContext here
                    .uri = *arena.Alloc<std::string>(
                        std::format("file://{}", (solution.RootDirectory() / param_file->path).string())),
                    .range = conv::ToLSPRange(param->name->nrange, param_file->ast),
                },
        }}};
      });
    }
    case core::ast::NodeKind::CompositeLiteral: {
      const auto* m = container_node->As<core::ast::nodes::CompositeLiteral>();

      const auto* stdecl = tgt->As<core::ast::nodes::StructTypeDecl>();

      while (n->parent != m) {
        n = n->parent;
      }

      const auto it = std::ranges::find(m->list, n);
      if (it == m->list.end()) {  //
        return std::nullopt;
      }

      const auto idx = it - m->list.begin();
      if (idx >= std::ssize(stdecl->fields)) {  //
        return std::nullopt;
      }

      const auto* field = stdecl->fields[idx];
      if (!field->name) {
        return std::nullopt;
      }
      const auto* field_file = core::ast::utils::SourceFileOf(stdecl);

      return render([&](lsp::InlayHint& rendition) {
        rendition.label = std::vector<lsp::InlayHintLabelPart>{{{
            .value = std::get<std::string_view>(rendition.label),
            .location =
                lsp::Location{
                    // TODO: unify with context, but I don't like bringing LsContext here
                    .uri = *arena.Alloc<std::string>(
                        std::format("file://{}", (solution.RootDirectory() / field_file->path).string())),
                    .range = conv::ToLSPRange(field->name->nrange, field_file->ast),
                },
        }}};
      });
    }
    default:
      break;
  }

  return std::nullopt;
}

}  // namespace vanadium::ls::detail
