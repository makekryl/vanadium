#include "TypeChecker.h"

#include <algorithm>
#include <stack>
#include <string_view>
#include <type_traits>
#include <utility>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "Program.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"

namespace vanadium::core {
namespace checker {

namespace utils {
const ast::nodes::FormalPars* GetCallableDeclParams(const ast::nodes::Decl* decl) {
  switch (decl->nkind) {
    case ast::NodeKind::FuncDecl:
      return decl->As<ast::nodes::FuncDecl>()->params;
    case ast::NodeKind::TemplateDecl:
      return decl->As<ast::nodes::TemplateDecl>()->params;
    default:
      return nullptr;
  }
}

const semantic::Symbol* GetCallableReturnType(const SourceFile& file, const ast::nodes::Decl* decl) {
  const auto resolve = [&file](const auto* v) -> const semantic::Symbol* {
    return file.module->scope->Resolve(file.Text(v));
  };
  switch (decl->nkind) {
    case ast::NodeKind::FuncDecl: {
      const auto* spec = decl->As<ast::nodes::FuncDecl>()->ret;
      return spec ? resolve(spec->type) : nullptr;
    }
    case ast::NodeKind::TemplateDecl:
      return resolve(decl->As<ast::nodes::TemplateDecl>()->type);
    default:
      return nullptr;
  }
}
}  // namespace utils

namespace {
template <typename ConcreteNode, auto PropertyPtr>
  requires ast::IsNode<ConcreteNode>
const semantic::Symbol* ResolveTypeVia(const SourceFile& file, const ast::Node* n) {
  const auto* m = n->As<ConcreteNode>();
  const auto* decl_file = ast::utils::SourceFileOf(m);
  const auto& prop = m->*PropertyPtr;
  return decl_file->module->scope->Resolve(file.Text(prop));
}
}  // namespace

const semantic::Symbol* DeduceExpressionType(const SourceFile& file, const ast::nodes::Expr* n) {
  switch (n->nkind) {
    case ast::NodeKind::Ident: {
      const auto* file = ast::utils::SourceFileOf(n);
      return file->module->scope->Resolve(file->Text(n));
    }
    case ast::NodeKind::CallExpr: {
      return ResolveTypeVia<ast::nodes::CallExpr, &ast::nodes::CallExpr::fun>(file, n);
    }
    default: {
      return nullptr;
    }
  }
}

class BasicTypeChecker {
 public:
  explicit BasicTypeChecker(const ModuleDescriptor& module, std::vector<TypeError>& errors)
      : module_(module),
        errors_(errors),
        inspector_(ast::NodeInspector::create<BasicTypeChecker, &BasicTypeChecker::Inspect>(this)) {}

  void Check() {
    InspectScope(module_.scope);
  }

 private:
  void InspectScope(const semantic::Scope* scope) {
    scope_ = scope;
    Visit(scope->GetContainer());
    scope_ = nullptr;

    for (const auto* child : scope->GetChildren()) {
      InspectScope(child);
    }
  }

  const semantic::Symbol* CheckType(const ast::Node* n, const semantic::Symbol* expected_type = nullptr);
  bool Inspect(const ast::Node*);

  void VisitChildren(const ast::Node* n) {
    n->Accept(inspector_);
  }
  void Visit(const ast::Node* n) {
    if (Inspect(n)) {
      VisitChildren(n);
    }
  }

  template <typename TParamDescriptorNode>
    requires ast::IsNode<TParamDescriptorNode>
  void PerformArgumentsTypeCheck(std::span<const ast::nodes::Expr* const> args, const ast::Range& args_range,
                                 const SourceFile* params_file, std::span<const TParamDescriptorNode* const> params,
                                 std::predicate<const TParamDescriptorNode*> auto is_param_required);

  const ModuleDescriptor& module_;
  std::vector<TypeError>& errors_;

  const semantic::Scope* scope_;
  const ast::NodeInspector inspector_;
};

template <typename TParamDescriptorNode>
  requires ast::IsNode<TParamDescriptorNode>
void BasicTypeChecker::PerformArgumentsTypeCheck(std::span<const ast::nodes::Expr* const> args,
                                                 const ast::Range& args_range, const SourceFile* params_file,
                                                 std::span<const TParamDescriptorNode* const> params,
                                                 std::predicate<const TParamDescriptorNode*> auto is_param_required) {
  const auto args_count = args.size();

  const auto minimal_args_cnt = static_cast<std::size_t>(std::ranges::count_if(params, is_param_required));
  const auto maximal_args_cnt = params.size();

  if (args_count < minimal_args_cnt || args_count > maximal_args_cnt) {
    errors_.emplace_back(TypeError{
              .range = (args_count < minimal_args_cnt) ?
                  ast::Range{
                      .begin = args_range.end - 1,
                      .end = args_range.end,  // closing paren
                  } : args_range,
              .message = std::format("{} arguments expected, {} provided", minimal_args_cnt, args_count),
          });
  }

  const auto check_argument = [&](const TParamDescriptorNode* param, const ast::Node* n) {
    const auto* exp_sym = params_file->module->scope->Resolve(params_file->Text(param->type));
    const auto* actual_sym = CheckType(n, exp_sym);
    if (exp_sym != actual_sym) {
      errors_.emplace_back(TypeError{
          .range = n->nrange,
          .message = std::format(
              "type mismatch, todo: show type names in='{}' vs ref='{}'",
              actual_sym == nullptr
                  ? "nullptr"
                  : ast::utils::SourceFileOf(actual_sym->Declaration())->Text(actual_sym->Declaration()),
              exp_sym == nullptr ? "nullptr"
                                 : ast::utils::SourceFileOf(exp_sym->Declaration())->Text(exp_sym->Declaration())),
      });
    }
  };

  bool seen_named_argument{false};
  for (std::size_t i = 0; i < args_count; ++i) {
    const auto* argnode = args[i];
    switch (argnode->nkind) {
      case ast::NodeKind::AssignmentExpr: {
        const auto* ae = argnode->As<ast::nodes::AssignmentExpr>();
        seen_named_argument = true;

        if (ae->property->nkind != ast::NodeKind::Ident) [[unlikely]] {
          errors_.emplace_back(TypeError{
              .range = ae->nrange,
              .message = "check if this even a valid syntax, it should not be a valid syntax [ f(g() := 1) ]",
          });
          continue;
        }

        const auto propname = module_.sf->Text(ae->property);
        for (const auto* param : params) {
          if (!param->name || params_file->Text(*param->name) != propname) {
            continue;
          }
          check_argument(param, ae->value);
        }

        break;
      }
      default: {
        if (seen_named_argument) {
          errors_.emplace_back(TypeError{
              .range = argnode->nrange,
              .message = "positional arguments cannot follow named arguments",
          });
        }
        if (i >= minimal_args_cnt) {
          Visit(argnode);
          continue;
        }
        check_argument(params[i], argnode);
        break;
      }
    }
  }
}

const semantic::Symbol* BasicTypeChecker::CheckType(const ast::Node* n, const semantic::Symbol* expected_type) {
  const semantic::Symbol* resulting_type{nullptr};
  switch (n->nkind) {
    case ast::NodeKind::CallExpr: {
      const auto* m = n->As<ast::nodes::CallExpr>();
      const auto* callee_sym = DeduceExpressionType(*module_.sf, m);
      if (!callee_sym) {
        VisitChildren(m);
        break;
      }

      const auto* callee_decl = callee_sym->Declaration()->As<ast::nodes::Decl>();
      const auto* callee_file = ast::utils::SourceFileOf(callee_decl);

      //
      resulting_type = utils::GetCallableReturnType(*callee_file, callee_decl);
      //

      const ast::nodes::FormalPars* params = utils::GetCallableDeclParams(callee_decl);
      if (!params) {
        VisitChildren(m);
        break;
      }

      PerformArgumentsTypeCheck<ast::nodes::FormalPar>(m->args->list, m->args->nrange, callee_file, params->list,
                                                       [](const ast::nodes::FormalPar* param) {
                                                         return param->value == nullptr;
                                                       });

      break;
    }

    case ast::NodeKind::CompositeLiteral: {
      const auto* m = n->As<ast::nodes::CompositeLiteral>();
      if (!expected_type) {
        VisitChildren(m);
        break;
      }

      if (!(expected_type->Flags() & semantic::SymbolFlags::kStructure)) {
        errors_.emplace_back(TypeError{
            .range = m->nrange,
            .message = "expected non structural type, todo",
        });
        break;
      }
      const auto* record_sym = expected_type;

      //
      resulting_type = record_sym;
      //

      const auto* record_decl = record_sym->Declaration()->As<ast::nodes::StructTypeDecl>();
      if (!record_decl) {
        VisitChildren(m);
        break;
      }

      const auto* record_file = ast::utils::SourceFileOf(record_decl);
      PerformArgumentsTypeCheck<ast::nodes::Field>(m->list, m->nrange, record_file, record_decl->fields,
                                                   [](const ast::nodes::Field* field) {
                                                     return !field->optional.has_value() || !(*field->optional);
                                                   });

      break;
    }

    case ast::NodeKind::Ident: {
      const auto* m = n->As<ast::nodes::Ident>();
      const auto* sym = scope_->Resolve(module_.sf->Text(m));
      if (!sym) {
        break;
      }

      const auto* decl = sym->Declaration();
      const auto* decl_file = ast::utils::SourceFileOf(decl);

      switch (decl->nkind) {
        case ast::NodeKind::Ident: {
          switch (decl->parent->nkind) {
            case ast::NodeKind::Declarator:
              resulting_type =
                  ResolveTypeVia<ast::nodes::ValueDecl, &ast::nodes::ValueDecl::type>(*decl_file, decl->parent->parent);
              break;
            case ast::NodeKind::FormalPar:
              resulting_type =
                  ResolveTypeVia<ast::nodes::FormalPar, &ast::nodes::FormalPar::type>(*decl_file, decl->parent);
              break;
            default:
              break;
          }
          break;
        }
        default:
          break;
      }
    }

    default:
      break;
  }

  return resulting_type;
}

bool BasicTypeChecker::Inspect(const ast::Node* n) {
  for (const auto& child : scope_->GetChildren()) {
    if (child->GetContainer()->nrange.Contains(n->nrange)) {
      return false;
    }
  }

  switch (n->nkind) {
    // case ast::NodeKind::BlockStmt: {
    //   // we will fall into another scope, so
    //   return false;
    // }
    case ast::NodeKind::CallExpr: {
      CheckType(n);
      return false;
    }
    default:
      break;
  }

  return true;
}

void PerformTypeCheck(const ModuleDescriptor& module, std::vector<TypeError>& errors) {
  BasicTypeChecker(module, errors).Check();
}

}  // namespace checker
}  // namespace vanadium::core
