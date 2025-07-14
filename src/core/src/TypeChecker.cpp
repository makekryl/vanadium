#include "TypeChecker.h"

#include <algorithm>
#include <stack>
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
  requires std::is_base_of_v<ast::Node, ConcreteNode>
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

  void Visit(const ast::Node* n) {
    if (Inspect(n)) {
      n->Accept(inspector_);
    }
  }

  void MaybeVisit(const ast::Node* n) {
    if (n != nullptr) {
      Visit(n);
    }
  }

  const ModuleDescriptor& module_;
  std::vector<TypeError>& errors_;

  const semantic::Scope* scope_;
  const ast::NodeInspector inspector_;
};

const semantic::Symbol* BasicTypeChecker::CheckType(const ast::Node* n, const semantic::Symbol* expected_type) {
  const semantic::Symbol* resulting_type{nullptr};
  switch (n->nkind) {
    case ast::NodeKind::CallExpr: {
      const auto* m = n->As<ast::nodes::CallExpr>();
      const auto* callee_sym = DeduceExpressionType(*module_.sf, m);
      if (!callee_sym) {
        break;
      }

      const auto* callee_decl = callee_sym->Declaration()->As<ast::nodes::Decl>();
      const auto* callee_file = ast::utils::SourceFileOf(callee_decl);

      //
      resulting_type = utils::GetCallableReturnType(*callee_file, callee_decl);
      //

      const ast::nodes::FormalPars* params = utils::GetCallableDeclParams(callee_decl);

      const auto minimal_args_cnt =
          static_cast<std::size_t>(std::ranges::count_if(params->list, [](const ast::nodes::FormalPar* par) {
            return par->value == nullptr;
          }));
      const auto maximal_args_cnt = params->list.size();
      const auto provided_args_cnt = m->args->list.size();

      if (provided_args_cnt < minimal_args_cnt || provided_args_cnt > maximal_args_cnt) {
        errors_.emplace_back(TypeError{
              .range = (provided_args_cnt < minimal_args_cnt) ?
                  ast::Range{
                      .begin = m->args->nrange.end - 1,
                      .end = m->args->nrange.end,  // closing paren
                  } : m->args->nrange,
              .message = std::format("{} arguments expected, {} provided", minimal_args_cnt, provided_args_cnt),
          });
      }

      const auto check_arg = [&](const ast::nodes::FormalPar* par, const ast::Node* n) {
        const auto* exp_sym = callee_sym->OriginatedScope()->Resolve(callee_file->Text(par->type));
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

      const auto argcnt = m->args->list.size();
      bool seen_explicit_assignment{false};
      for (std::size_t i = 0; i < argcnt; ++i) {
        const auto* argnode = m->args->list[i];
        switch (argnode->nkind) {
          case ast::NodeKind::AssignmentExpr: {
            const auto* ae = argnode->As<ast::nodes::AssignmentExpr>();
            if (ae->property->nkind != ast::NodeKind::Ident) [[unlikely]] {
              errors_.emplace_back(TypeError{
                  .range = ae->nrange,
                  .message = "check if this even a valid syntax, it should not be a valid syntax [ f(g() := 1) ]",
              });
              continue;
            }

            seen_explicit_assignment = true;

            const auto propname = module_.sf->Text(ae->property);
            for (const auto* par : params->list) {
              if (!par->name || callee_file->Text(*par->name) != propname) {
                continue;
              }
              check_arg(par, ae->value);
            }

            break;
          }
          default: {
            if (seen_explicit_assignment) {
              errors_.emplace_back(TypeError{
                  .range = argnode->nrange,
                  .message = "positional arguments can't be placed after (how explicit args called?)",
              });
            }
            if (i >= minimal_args_cnt) {
              Visit(argnode);
              continue;
            }
            check_arg(params->list[i], argnode);
            break;
          }
        }
      }
      break;
    }

    case ast::NodeKind::CompositeLiteral: {
      const auto* m = n->As<ast::nodes::CompositeLiteral>();
      if (!expected_type) break;

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
      const auto* record_file = ast::utils::SourceFileOf(record_decl);

      const auto minimal_args_cnt =
          static_cast<std::size_t>(std::ranges::count_if(record_decl->fields, [](const ast::nodes::Field* f) {
            return !f->optional.has_value() || !(*f->optional);
          }));
      const auto maximal_args_cnt = record_decl->fields.size();
      const auto provided_args_cnt = m->list.size();

      if (provided_args_cnt < minimal_args_cnt || provided_args_cnt > maximal_args_cnt) {
        errors_.emplace_back(TypeError{
              .range = (provided_args_cnt < minimal_args_cnt) ?
                  ast::Range{
                      .begin = m->nrange.end - 1,
                      .end = m->nrange.end,  // closing paren
                  } : m->nrange,
              .message = std::format("{} arguments expected, {} provided", minimal_args_cnt, provided_args_cnt),
          });
      }

      const auto check_arg = [&](const ast::nodes::Field* field, const ast::Node* n) {
        const auto* exp_sym = record_sym->Members()->Lookup(record_file->Text(field->type));
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

      const auto argcnt = m->list.size();
      bool seen_explicit_assignment{false};
      for (std::size_t i = 0; i < argcnt; ++i) {
        const auto* argnode = m->list[i];
        switch (argnode->nkind) {
          case ast::NodeKind::AssignmentExpr: {
            const auto* ae = argnode->As<ast::nodes::AssignmentExpr>();
            if (ae->property->nkind != ast::NodeKind::Ident) [[unlikely]] {
              errors_.emplace_back(TypeError{
                  .range = ae->nrange,
                  .message = "check if this even a valid syntax, it should not be a valid syntax [ f(g() := 1) ]",
              });
              continue;
            }

            seen_explicit_assignment = true;

            const auto propname = module_.sf->Text(ae->property);
            for (const auto* par : record_decl->fields) {
              if (!par->name || record_file->Text(*par->name) != propname) {
                continue;
              }
              check_arg(par, ae->value);
            }

            break;
          }
          default: {
            if (seen_explicit_assignment) {
              errors_.emplace_back(TypeError{
                  .range = argnode->nrange,
                  .message = "positional arguments can't be placed after (how explicit args called?)",
              });
            }
            if (i >= minimal_args_cnt) {
              Visit(argnode);
              continue;
            }
            check_arg(record_decl->fields[i], argnode);
            break;
          }
        }
      }

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
