#include <algorithm>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <vector>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "Program.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {
namespace {
class SignatureInformationBuilder {
 public:
  SignatureInformationBuilder(std::string& buf) : buf_(buf) {}

  void Estimate(std::size_t count) {
    buf_.reserve(count * 32);
  }

  void Push(std::invocable<std::string&> auto f) {
    std::uint32_t begin = buf_.size();
    f(buf_);
    buf_ += kSeparator;
    std::uint32_t end = buf_.size();

    params_.emplace_back(lsp::ParameterInformation{
        .label = std::make_tuple(begin, end),
    });
  }

  [[nodiscard]] std::vector<lsp::ParameterInformation> Build() {
    if (!params_.empty()) {
      buf_.erase(buf_.size() - 2);
    }
    return std::move(params_);
  }

 private:
  static constexpr std::string_view kSeparator = ", ";

  std::string& buf_;
  std::vector<lsp::ParameterInformation> params_;
};

template <typename ContainerNode>
  requires(std::is_same_v<ContainerNode, core::ast::nodes::ParenExpr> ||
           std::is_same_v<ContainerNode, core::ast::nodes::CompositeLiteral>)
std::optional<std::size_t> GetCurrentArgumentIndex(const ContainerNode* container, const core::ast::Node* n,
                                                   core::ast::pos_t exact_pos) {
  if (n->nkind == ContainerNode::kKind) {
    for (const auto [idx, arg] : container->list | std::views::enumerate | std::views::reverse) {
      if (arg->nrange.end < exact_pos) {
        return idx;
      }
    }
  } else if (auto it = std::ranges::find(container->list, n); it != container->list.end()) {
    return it - container->list.begin();
  }
  return 0;
}

lsp::SignatureHelpResult ProvideSignatureHelp(const lsp::SignatureHelpParams& params, const core::SourceFile& file,
                                              LsSessionRef d) {
  if (!file.module) {
    return nullptr;
  }

  const auto pos = file.ast.lines.GetPosition(conv::FromLSPPosition(params.position));
  const auto* n = core::ast::utils::GetNodeAt(file.ast, pos);

  const auto* container = n;
  if (n->nkind == core::ast::NodeKind::ErrorNode) {
    container = n->parent;
  }

  switch (container->nkind) {
    case core::ast::NodeKind::ParenExpr: {
      const auto* pe = container->As<core::ast::nodes::ParenExpr>();

      const core::semantic::Scope* scope = core::semantic::utils::FindScope(file.module->scope, pe);

      const auto* callable_params = core::checker::utils::ResolveCallableParams(&file, scope, pe);
      if (!callable_params) {
        return nullptr;
      }
      const core::ast::Node* callable_decl = callable_params->parent;
      const auto* callable_file = core::ast::utils::SourceFileOf(callable_decl);

      auto& label = *d.arena.Alloc<std::string>();
      label.reserve(32 * (callable_params->list.size() + 2));

      std::string_view return_type_name;
      switch (callable_decl->nkind) {
        case core::ast::NodeKind::FuncDecl: {
          const auto* fdecl = callable_decl->As<core::ast::nodes::FuncDecl>();
          label += callable_file->Text(fdecl->kind.range);
          label += " ";
          label += callable_file->Text(*fdecl->name);

          if (fdecl->ret) {
            return_type_name = callable_file->Text(fdecl->ret->type);
          }
          break;
        }
        case core::ast::NodeKind::TemplateDecl: {
          const auto* tdecl = callable_decl->As<core::ast::nodes::TemplateDecl>();
          label += "template ";
          label += callable_file->Text(*tdecl->name);

          return_type_name = callable_file->Text(tdecl->type);
        }
        default: {
          break;
        }
      }

      label += "(";
      //
      SignatureInformationBuilder builder{label};
      builder.Estimate(callable_params->list.size());
      for (const auto* param : callable_params->list) {
        builder.Push([&](std::string& buf) {
          buf += callable_file->Text(param);
        });
      }
      const auto parameters = builder.Build();
      //
      label += ")";

      if (return_type_name != "") {
        label += " -> ";
        label += return_type_name;
      }

      const std::optional<std::uint32_t> active_idx =
          std::ranges::any_of(pe->list,
                              [](const auto* arg) {
                                return arg->nkind == core::ast::NodeKind::AssignmentExpr;
                              })
              ? std::nullopt
              : GetCurrentArgumentIndex(pe, n, pos);

      return lsp::SignatureHelp{
          .signatures = {{
              lsp::SignatureInformation{
                  .label = label,
                  .parameters = std::move(parameters),
              },
          }},
          .activeSignature = 0,
          .activeParameter = active_idx,
      };
    }
    case core::ast::NodeKind::CompositeLiteral: {
      const auto* cl = container->As<core::ast::nodes::CompositeLiteral>();

      const core::semantic::Scope* scope = core::semantic::utils::FindScope(file.module->scope, cl);

      const auto* type = core::checker::ext::DeduceCompositeLiteralType(&file, scope, cl);
      if (!type) {
        break;
      }

      const auto* stdecl = type->Declaration();
      const auto& stfields = *core::ast::utils::GetStructFields(stdecl);
      const auto* stdecl_file = core::ast::utils::SourceFileOf(stdecl);

      auto& label = *d.arena.Alloc<std::string>();
      label.reserve(32 * (stfields.size() + 2));

      if (stdecl->nkind == core::ast::NodeKind::StructTypeDecl) {
        label += stdecl_file->Text(*stdecl->As<core::ast::nodes::StructTypeDecl>()->name);
      }
      label += "{";
      //
      SignatureInformationBuilder builder{label};
      builder.Estimate(stfields.size());
      for (const auto* field : stfields) {
        builder.Push([&](std::string& buf) {
          buf += stdecl_file->Text(field);
        });
      }
      const auto parameters = builder.Build();
      //
      label += "}";

      const std::optional<std::uint32_t> active_idx =
          std::ranges::any_of(cl->list,
                              [](const auto* arg) {
                                return arg->nkind == core::ast::NodeKind::AssignmentExpr;
                              })
              ? std::nullopt
              : GetCurrentArgumentIndex(cl, n, pos);

      return lsp::SignatureHelp{
          .signatures = {{
              lsp::SignatureInformation{
                  .label = label,
                  .parameters = std::move(parameters),
              },
          }},
          .activeSignature = 0,
          .activeParameter = active_idx,
      };
    }
    default: {
      break;
    }
  }

  return nullptr;
}
}  // namespace

template <>
rpc::ExpectedResult<lsp::SignatureHelpResult> methods::textDocument::signatureHelp::operator()(
    LsContext& ctx, const lsp::SignatureHelpParams& params) {
  return ctx->WithFile<lsp::SignatureHelpResult>(params, ProvideSignatureHelp).value_or(nullptr);
}
}  // namespace vanadium::ls
