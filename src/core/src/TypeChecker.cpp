#include "TypeChecker.h"

#include <algorithm>
#include <cstdlib>
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

namespace {
template <ast::IsNode ConcreteNode, auto PropertyPtr>
const semantic::Symbol* ResolveTypeVia(const SourceFile& file, const ast::Node* n) {
  const auto* m = n->As<ConcreteNode>();
  const auto* decl_file = ast::utils::SourceFileOf(m);
  const auto& prop = m->*PropertyPtr;
  return decl_file->module->scope->Resolve(file.Text(prop));
}
}  // namespace

namespace utils {

[[nodiscard]] std::string_view GetReadableTypeName(const SourceFile& sf, const semantic::Symbol* sym) {
  const auto* decl = sym->Declaration();
  switch (decl->nkind) {
    case ast::NodeKind::StructTypeDecl: {
      const auto* m = decl->As<ast::nodes::StructTypeDecl>();
      return sf.Text(ast::Range{
          .begin = m->kind.range.begin,
          .end = m->name->nrange.end,
      });
    }
    case ast::NodeKind::SubTypeDecl: {
      const auto* m = decl->As<ast::nodes::SubTypeDecl>();
      return sf.Text(ast::Range{
          .begin = m->field->nrange.begin,
          .end = m->field->name->nrange.end,
      });
    }
    default:
      return sf.Text(sym->Declaration());
  }
}

[[nodiscard]] std::string_view GetReadableTypeName(const semantic::Symbol* sym) {
  if (sym->Flags() & semantic::SymbolFlags::kBuiltin) {
    return sym->GetName();
  }
  return GetReadableTypeName(*ast::utils::SourceFileOf(sym->Declaration()), sym);
}

const semantic::Symbol* GetIdentType(const SourceFile& sf, const semantic::Scope* scope, const ast::nodes::Ident* m) {
  const auto* sym = scope->Resolve(sf.Text(m));
  if (!sym) {
    return nullptr;
  }

  const auto* decl = sym->Declaration();
  const auto* decl_file = ast::utils::SourceFileOf(decl);

  switch (decl->nkind) {
    case ast::NodeKind::Ident: {
      switch (decl->parent->nkind) {
        case ast::NodeKind::Declarator:
          return ResolveTypeVia<ast::nodes::ValueDecl, &ast::nodes::ValueDecl::type>(*decl_file, decl->parent->parent);
        case ast::NodeKind::FormalPar:
          return ResolveTypeVia<ast::nodes::FormalPar, &ast::nodes::FormalPar::type>(*decl_file, decl->parent);
        default:
          break;
      }
      break;
    }
    default:
      break;
  }

  return nullptr;
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

template <typename OnNonStructuralType, typename OnUnknownProperty>
  requires(std::is_invocable_v<OnNonStructuralType, const ast::Node*, const semantic::Symbol*> &&
           std::is_invocable_v<OnUnknownProperty, const ast::nodes::SelectorExpr*,
                               std::pair<const SourceFile*, const semantic::Symbol*>>)
struct SelectorExprTypeExtractor {
  const SourceFile& sf;
  const semantic::Scope* scope;

  OnNonStructuralType on_non_structural_type;
  OnUnknownProperty on_unknown_property;

  const semantic::Symbol* ExtractSelectorExprType(const ast::nodes::SelectorExpr* se) const {
    const semantic::Symbol* x_sym{nullptr};
    if (se->x->nkind == ast::NodeKind::SelectorExpr) {
      x_sym = ExtractSelectorExprType(se->x->As<ast::nodes::SelectorExpr>());
    } else {
      x_sym = GetEffectiveType(sf, scope, se->x);
    }

    if (!x_sym) {
      return nullptr;
    }

    if (!(x_sym->Flags() & semantic::SymbolFlags::kStructural)) {
      on_non_structural_type(se->x, x_sym);
      return nullptr;
    }

    const auto* record_decl = x_sym->Declaration()->As<ast::nodes::StructTypeDecl>();
    const auto* record_file = ast::utils::SourceFileOf(record_decl);

    const auto property_name = sf.Text(se->sel);

    auto it = std::ranges::find_if(record_decl->fields, [&](const auto* f) {
      return f->name && record_file->Text(*f->name) == property_name;
    });
    if (it == record_decl->fields.end()) {
      on_unknown_property(se, std::make_pair(record_file, x_sym));
      return nullptr;
    }

    const auto* field = *it;

    return record_file->module->scope->Resolve(record_file->Text(field->type));
  }
};

const semantic::Symbol* GetSelectorExprType(const SourceFile& sf, const semantic::Scope* scope,
                                            const ast::nodes::SelectorExpr* se) {
  const SelectorExprTypeExtractor extractor{
      .sf = sf,
      .scope = scope,
      .on_non_structural_type = [](const auto*, auto) {},
      .on_unknown_property = [](const auto*, auto) {},
  };
  return extractor.ExtractSelectorExprType(se);
}

std::optional<std::pair<std::size_t, std::size_t>> GetLengthExprBounds(const SourceFile* sf,
                                                                       const ast::nodes::LengthExpr* lc) {
  if (!lc->size || lc->size->nkind != ast::NodeKind::ParenExpr) {
    return std::nullopt;
  }

  const auto* pe = lc->size->As<ast::nodes::ParenExpr>();
  if (pe->list.size() != 1 || pe->list.front()->nkind != ast::NodeKind::BinaryExpr) {
    return std::nullopt;
  }

  const auto* be = pe->list.front()->As<ast::nodes::BinaryExpr>();
  const auto lo_str = sf->Text(be->x);
  std::size_t min_args{};
  if (std::from_chars(lo_str.data(), lo_str.data() + lo_str.size(), min_args).ec != std::errc{}) {
    return std::nullopt;
  }

  const auto hi_str = sf->Text(be->y);
  std::size_t max_args{};
  if (hi_str == "infinity") {
    max_args = static_cast<std::size_t>(-1);
  } else if (std::from_chars(hi_str.data(), hi_str.data() + hi_str.size(), max_args).ec != std::errc{}) {
    return std::nullopt;
  }

  return std::make_pair(min_args, max_args);
}

}  // namespace utils

const semantic::Symbol* GetEffectiveType(const SourceFile& sf, const semantic::Scope* scope, const ast::Node* n) {
  switch (n->nkind) {
    case ast::NodeKind::FuncDecl:
    case ast::NodeKind::TemplateDecl: {
      return utils::GetCallableReturnType(sf, n->As<ast::nodes::Decl>());
    }

    case ast::NodeKind::Ident: {
      return utils::GetIdentType(sf, scope, n->As<ast::nodes::Ident>());
    }

    default:
      return nullptr;
  }
}

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
    const auto* prev_scope = scope_;
    scope_ = scope;
    Visit(scope->GetContainer());
    scope_ = prev_scope;
  }

  void MatchTypes(const ast::Range& range, const semantic::Symbol* actual, const semantic::Symbol* expected);

  const semantic::Symbol* CheckType(const ast::Node* n, const semantic::Symbol* desired_type = nullptr);
  bool Inspect(const ast::Node*);

  void Introspect(const ast::Node* n) {
    n->Accept(inspector_);
  }
  void Visit(const ast::Node* n) {
    if (Inspect(n)) {
      Introspect(n);
    }
  }

  struct ArgumentsTypeCheckOptions {
    bool is_union{false};
  };

  template <ast::IsNode TParamDescriptorNode, ArgumentsTypeCheckOptions Options>
  void PerformArgumentsTypeCheck(std::span<const ast::nodes::Expr* const> args, const ast::Range& args_range,
                                 const SourceFile* params_file, std::span<const TParamDescriptorNode* const> params,
                                 std::predicate<const TParamDescriptorNode*> auto is_param_required);

  const ModuleDescriptor& module_;
  std::vector<TypeError>& errors_;

  const semantic::Scope* scope_;
  const ast::NodeInspector inspector_;
};

void BasicTypeChecker::MatchTypes(const ast::Range& range, const semantic::Symbol* actual,
                                  const semantic::Symbol* expected) {
  if (expected == actual || !expected) {
    return;
  }

  if (!actual) {
    // Seems like that is is too much as right side types that cannot be resolved
    // will be reported by the semantic analyzeer

    // errors_.emplace_back(TypeError{
    //     .range = range,
    //     .message = std::format("expected argument of type '{}', got argument of unknown type",
    //                            utils::GetReadableTypeName(expected)),
    // });
    return;
  }

  errors_.emplace_back(TypeError{
      .range = range,
      .message = std::format("expected argument of type '{}', got argument of type '{}'",
                             utils::GetReadableTypeName(expected), utils::GetReadableTypeName(actual)),
  });
}

template <ast::IsNode TParamDescriptorNode, BasicTypeChecker::ArgumentsTypeCheckOptions Options = {}>
void BasicTypeChecker::PerformArgumentsTypeCheck(std::span<const ast::nodes::Expr* const> args,
                                                 const ast::Range& args_range, const SourceFile* params_file,
                                                 std::span<const TParamDescriptorNode* const> params,
                                                 std::predicate<const TParamDescriptorNode*> auto is_param_required) {
  const auto args_count = args.size();

  const auto minimal_args_cnt = static_cast<std::size_t>(std::ranges::count_if(params, is_param_required));
  const auto maximal_args_cnt = params.size();

  if constexpr (!Options.is_union) {
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
  }

  const auto check_argument = [&](const TParamDescriptorNode* param, const ast::Node* n) {
    const auto* exp_sym = params_file->module->scope->Resolve(params_file->Text(param->type));
    const auto* actual_sym = CheckType(n, exp_sym);
    MatchTypes(n->nrange, actual_sym, exp_sym);
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

        const auto property_name = module_.sf->Text(ae->property);
        auto param_it = std::ranges::find_if(params, [&](const auto* p) {
          return p->name && params_file->Text(*p->name) == property_name;
        });

        if (param_it != params.end()) {
          check_argument(*param_it, ae->value);
        } else {
          errors_.emplace_back(TypeError{
              .range = ae->property->nrange,
              .message = std::format("unknown property '{}'", property_name),
          });
          Visit(argnode);
        }

        break;
      }
      default: {
        if (seen_named_argument && !Options.is_union) {
          errors_.emplace_back(TypeError{
              .range = argnode->nrange,
              .message = "positional arguments cannot follow named ones",
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

  if constexpr (Options.is_union) {
    if (!seen_named_argument || args_count != 1) {
      errors_.emplace_back(TypeError{
          .range = args_range,
          .message = std::format("exactly one named argument is expected in union"),
      });
    }
  }
}

const semantic::Symbol* BasicTypeChecker::CheckType(const ast::Node* n, const semantic::Symbol* desired_type) {
  const semantic::Symbol* resulting_type{nullptr};
  switch (n->nkind) {
    case ast::NodeKind::CallExpr: {
      const auto* m = n->As<ast::nodes::CallExpr>();
      const auto* callee_sym = DeduceExpressionType(*module_.sf, m);
      if (!callee_sym) {
        Introspect(m);
        break;
      }

      const auto* callee_decl = callee_sym->Declaration()->As<ast::nodes::Decl>();
      const auto* callee_file = ast::utils::SourceFileOf(callee_decl);

      //
      resulting_type = utils::GetCallableReturnType(*callee_file, callee_decl);
      //

      const ast::nodes::FormalPars* params = ast::utils::GetCallableDeclParams(callee_decl);
      if (!params) {
        Introspect(m);
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
      if (!desired_type) {
        Introspect(m);
        break;
      }

      if (desired_type->Flags() & (semantic::SymbolFlags::kStructural | semantic::SymbolFlags::kSubType)) {
        //
        resulting_type = desired_type;
        //
      }

      if (desired_type->Flags() & semantic::SymbolFlags::kSubType) {
        const auto* subtype_sym = desired_type;
        const auto* subtype_decl = subtype_sym->Declaration()->As<ast::nodes::SubTypeDecl>();
        if (!subtype_decl) {
          Introspect(m);
          break;
        }
        const auto* subtype_file = ast::utils::SourceFileOf(subtype_decl);

        const auto* f = subtype_decl->field;
        if (f->type->nkind == ast::NodeKind::ListSpec) {
          const auto* ls = f->type->As<ast::nodes::ListSpec>();
          if (ls->length) {
            if (const auto& bounds_opt = utils::GetLengthExprBounds(subtype_file, ls->length); bounds_opt) {
              const auto& [min_args, max_args] = *bounds_opt;
              const auto args_count = m->list.size();
              if (args_count < min_args || args_count > max_args) {
                errors_.emplace_back(TypeError{
                    .range =
                        ast::Range{
                            .begin = m->nrange.end - 1,
                            .end = m->nrange.end,  // closing paren
                        },
                    .message = std::format("elements count is required to be in between {} and {}", min_args, max_args),
                });
              }
            }
          }

          if (ls->elemtype->nkind == ast::NodeKind::RefSpec) {
            const auto* rs = ls->elemtype->As<ast::nodes::RefSpec>();
            const auto* inner_type_sym = subtype_file->module->scope->Resolve(subtype_file->Text(rs->x));
            for (const auto* arg : m->list) {
              const auto* actual_sym = CheckType(arg, inner_type_sym);
              MatchTypes(arg->nrange, actual_sym, inner_type_sym);
            }
          }
        }

        break;
      }

      const auto* record_sym = desired_type;
      const auto* record_decl = record_sym->Declaration()->As<ast::nodes::StructTypeDecl>();
      if (!record_decl) {
        Introspect(m);
        break;
      }

      const auto* record_file = ast::utils::SourceFileOf(record_decl);

      if (record_file->Text(record_decl->kind.range) == "union") {
        PerformArgumentsTypeCheck<ast::nodes::Field, {.is_union = true}>(m->list, m->nrange, record_file,
                                                                         record_decl->fields, [](const auto*) {
                                                                           return false;
                                                                         });
      } else {
        PerformArgumentsTypeCheck<ast::nodes::Field>(m->list, m->nrange, record_file, record_decl->fields,
                                                     [](const ast::nodes::Field* field) {
                                                       return !field->optional;
                                                     });
      }

      break;
    }

    case ast::NodeKind::SelectorExpr: {
      const utils::SelectorExprTypeExtractor extractor{
          .sf = *module_.sf,
          .scope = scope_,
          .on_non_structural_type =
              [&](const ast::Node* x, const semantic::Symbol* sym) {
                errors_.emplace_back(TypeError{
                    .range = x->nrange,
                    .message = std::format("type '{}' is not subscriptable", utils::GetReadableTypeName(sym)),
                });
              },
          .on_unknown_property =
              [&](const ast::nodes::SelectorExpr* se, std::pair<const SourceFile*, const semantic::Symbol*> fsy) {
                const auto [sym_file, sym] = fsy;
                errors_.emplace_back(TypeError{
                    .range = se->sel->nrange,
                    .message = std::format("property '{}' does not exist on type '{}'", module_.sf->Text(se->sel),
                                           utils::GetReadableTypeName(*sym_file, sym)),
                });
              },
      };
      resulting_type = extractor.ExtractSelectorExprType(n->As<ast::nodes::SelectorExpr>());
      break;
    }

    case ast::NodeKind::Ident: {
      resulting_type = utils::GetIdentType(*module_.sf, scope_, n->As<ast::nodes::Ident>());
      break;
    }

    default:
      break;
  }

  return resulting_type;
}

bool BasicTypeChecker::Inspect(const ast::Node* n) {
  for (const auto& child : scope_->GetChildren()) {
    if (child->GetContainer()->nrange.Contains(n->nrange)) {
      InspectScope(child);
      return false;
    }
  }

  switch (n->nkind) {
    case ast::NodeKind::CallExpr: {
      CheckType(n);
      return false;
    }

    case ast::NodeKind::ValueDecl: {
      const auto* m = n->As<ast::nodes::ValueDecl>();
      const auto* expected_type = module_.scope->Resolve(module_.sf->Text(m->type));
      for (const auto* decl : m->decls) {
        if (!decl->value) {
          continue;
        }

        const auto* actual_type = CheckType(decl->value, expected_type);
        MatchTypes(decl->nrange, actual_type, expected_type);
      }
      break;
    };

    case ast::NodeKind::SubTypeDecl: {
      const auto* m = n->As<ast::nodes::SubTypeDecl>();
      const auto* f = m->field;

      if (f->type->nkind == ast::NodeKind::ListSpec) {
        const auto* ls = f->type->As<ast::nodes::ListSpec>();
        if (ls->length) {
          const auto& bounds_opt = utils::GetLengthExprBounds(module_.sf, ls->length);
          if (bounds_opt) {
            const auto& [min_args, max_args] = *bounds_opt;
            if (min_args > max_args) {
              errors_.emplace_back(TypeError{
                  .range = ls->length->nrange,
                  .message = "lower bound cannot exceed the highest",
              });
              break;
            }
          }
        }
      }

      if (f->value_constraint && f->type->nkind == ast::NodeKind::RefSpec) {
        const auto* sym = scope_->Resolve(module_.sf->Text(*f->type->As<ast::nodes::RefSpec>()->x));
        for (const auto* item : f->value_constraint->list) {
          CheckType(item, sym);
        }
      }

      break;
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
