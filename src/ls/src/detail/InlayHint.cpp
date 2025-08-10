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
#include "LanguageServerHelpers.h"
#include "Program.h"
#include "ScopedNodeVisitor.h"
#include "Semantic.h"
#include "Solution.h"
#include "TypeChecker.h"
#include "detail/Definition.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

// TODO: maybe track file version
struct InlayHintPayload {
  std::string path;
  vanadium::core::ast::pos_t anchor_pos;
  vanadium::core::ast::NodeKind node_kind;

  static glz::json_t AsJson(InlayHintPayload&& payload) {
    // TODO: find a way to have strongly-typed 'data' on both ends (maybe modify lspgen to produce templates,
    // exposing internal payload structure to the higher levels does not sounds good though - and it will be
    // required to do in such case)
    return {
        {"path", std::move(payload.path)},
        {"apos", payload.anchor_pos},
        {"nk", std::to_underlying(payload.node_kind)},
    };
  }
};
template <>
struct glz::meta<InlayHintPayload> {
  static constexpr std::string_view rename_key(std::string_view key) {
    if (key == "anchor_pos") return "apos";
    if (key == "node_kind") return "nk";
    return key;
  }
};

namespace vanadium::ls::detail {

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
[[nodiscard]] const core::ast::Node* LocateInlayHintTarget(const core::SourceFile& file,
                                                           const core::semantic::Scope* scope, const core::ast::Node* n,
                                                           Options options) {
  switch (n->nkind) {
    case core::ast::NodeKind::CallExpr: {
      const auto* m = n->As<core::ast::nodes::CallExpr>();

      // ResolveCallableParams is not used because we need to filter out builtin functions
      // return core::checker::utils::ResolveCallableParams(file, scope, m->args);  // -> FormalPars

      const auto* fun_sym = core::checker::ResolveExprType(&file, scope, m->fun);
      if (!fun_sym || (fun_sym->Flags() & core::semantic::SymbolFlags::kBuiltinDef) ||
          !(fun_sym->Flags() & (core::semantic::SymbolFlags::kFunction | core::semantic::SymbolFlags::kTemplate))) {
        return nullptr;
      }

      return core::ast::utils::GetCallableDeclParams(
          fun_sym->Declaration()->As<core::ast::nodes::Decl>());  // -> FormalPars
    }
    case core::ast::NodeKind::CompositeLiteral: {
      const auto* m = n->As<core::ast::nodes::CompositeLiteral>();
      const auto* sym = options.deduceCompositeLiteralType(&file, scope, m);
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
void ComputeInlayHint(const core::SourceFile& file, const core::semantic::Scope* scope, const core::ast::Node* n,
                      lib::Arena& arena, std::vector<lsp::InlayHint>& out,
                      InlayHintTargetLocatorOptions inlayHintTargetLocatorOptions) {
  const auto add_parameter_inlay_hint = [&](const core::ast::pos_t pos, std::string_view name) {
    out.emplace_back(lsp::InlayHint{
        .position = conv::ToLSPPosition(file.ast.lines.Translate(pos)),
        .label = *arena.Alloc<std::string>(std::format("{} := ", name)),
        .kind = lsp::InlayHintKind::kParameter,
        .data = InlayHintPayload::AsJson({
            .path = file.path,
            .anchor_pos = n->nrange.begin,
            .node_kind = n->nkind,
        }),
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
        if (param_name == file.Text(arg)) {
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
        if (param_name == file.Text(arg)) {
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

std::vector<lsp::InlayHint> CollectInlayHints(const lsp::InlayHintParams& params, const core::SourceFile& file,
                                              LsSessionRef d) {
  if (!file.module) {
    return {};
  }

  const auto requested_range = conv::FromLSPRange(params.range, file.ast);
  std::vector<lsp::InlayHint> hints;

  const core::semantic::Scope* scope{nullptr};

  std::stack<const core::semantic::Symbol*> cl_type_cache;
  cl_type_cache.emplace(nullptr);
  //
  const auto memoizing_visitor = [&](this auto&& self, const core::ast::Node* n) {
    ComputeInlayHint(file, scope, n, d.arena, hints,
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
      file.module->scope,
      [&](const core::semantic::Scope* scope_under_inspection) {
        scope = scope_under_inspection;
      },
      [&](const core::ast::Node* n) -> bool {
        if (overlaps(requested_range, n->nrange)) {
          if (n->nkind == core::ast::NodeKind::CompositeLiteral) {
            memoizing_visitor(n);
            return false;
          }
          ComputeInlayHint(file, scope, n, d.arena, hints, kNonCachingInlayHintTargetLocatorOptions);
          return true;
        }
        return n->nrange.Contains(requested_range);
      });

  return hints;
}

std::optional<lsp::InlayHint> ResolveInlayHint(const lsp::InlayHint& original_hint, LsSessionRef d) {
  if (!original_hint.data) {
    return std::nullopt;
  }
  const auto payload = glz::read_json<InlayHintPayload>(*original_hint.data);

  const auto* project = d.solution.ProjectOf(d.solution.Directory().Join(payload->path));
  if (!project) [[unlikely]] {
    return std::nullopt;
  }
  const auto* file = project->program.GetFile(payload->path);
  if (!file) [[unlikely]] {
    return std::nullopt;
  }

  const auto* n = detail::FindNode(file, original_hint.position);
  if (!n) [[unlikely]] {
    return std::nullopt;
  }

  const core::ast::Node* container_node = core::ast::utils::GetNodeAt(file->ast, payload->anchor_pos);
  while (container_node->nkind != static_cast<core::ast::NodeKind>(payload->node_kind)) {
    container_node = container_node->parent;
    if (!container_node) [[unlikely]] {
      return std::nullopt;
    }
  }

  const auto* tgt = LocateInlayHintTarget(*file, core::semantic::utils::FindScope(file->module->scope, container_node),
                                          container_node, kNonCachingInlayHintTargetLocatorOptions);
  if (!tgt) {
    return std::nullopt;
  }

  const auto render = [&](const core::SourceFile* tgt_file, const core::ast::Range& range) -> lsp::InlayHint {
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

    rendition.label = std::vector<lsp::InlayHintLabelPart>{{{
        .value = std::get<std::string_view>(rendition.label),
        .location =
            lsp::Location{
                .uri = *d.arena.Alloc<std::string>(helpers::PathToFileUri(d.solution, tgt_file->path)),
                .range = conv::ToLSPRange(range, tgt_file->ast),
            },
    }}};

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

      return render(param_file, param->name->nrange);
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

      return render(field_file, field->name->nrange);
    }
    default:
      break;
  }

  return std::nullopt;
}

}  // namespace vanadium::ls::detail
