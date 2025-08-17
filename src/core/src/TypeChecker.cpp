#include "TypeChecker.h"

#include <algorithm>
#include <cstdlib>
#include <format>
#include <string_view>
#include <type_traits>
#include <utility>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "Builtins.h"
#include "Program.h"
#include "ScopedNodeVisitor.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"

namespace vanadium::core {
namespace checker {

namespace symbols {
const semantic::Symbol kTypeError{"<error-type>", nullptr, semantic::SymbolFlags::kBuiltinType};

const semantic::Symbol kVoidType{"<void>", nullptr, semantic::SymbolFlags::kBuiltinType};

const semantic::Symbol kInferType{"<infer>", nullptr, semantic::SymbolFlags::kBuiltinType};

const semantic::Symbol kVoidTypekWildcardType{
    "<*>", nullptr,
    semantic::SymbolFlags::Value(semantic::SymbolFlags::kBuiltinType | semantic::SymbolFlags::kTemplateSpec)};
const semantic::Symbol kQuestionType{
    "<?>", nullptr,
    semantic::SymbolFlags::Value(semantic::SymbolFlags::kBuiltinType | semantic::SymbolFlags::kTemplateSpec)};
}  // namespace symbols

struct InstantiatedType {
  const semantic::Symbol* sym{nullptr};
  const ast::Node* instance{nullptr};

  operator bool() const {
    return sym != nullptr;
  }
  const semantic::Symbol* operator->() const {
    return sym;
  }

  InstantiatedType& operator=(const semantic::Symbol* nsym) {
    sym = nsym;
    return *this;
  }
};
namespace {
InstantiatedType ResolveExprInstantiatedType(const SourceFile*, const semantic::Scope*, const ast::nodes::Expr*);
}  // namespace

namespace utils {
const ast::nodes::FormalPars* ResolveCallableParams(const SourceFile* file, const semantic::Scope* scope,
                                                    const ast::nodes::ParenExpr* pe) {
  if (pe->parent->nkind != ast::NodeKind::CallExpr) {
    // such cases should be checked, I don't have code samples though
    return nullptr;
  }

  const auto* ce = pe->parent->As<ast::nodes::CallExpr>();
  const auto* fun_sym = ResolveExprType(file, scope, ce->fun);
  if (!fun_sym || !(fun_sym->Flags() & (semantic::SymbolFlags::kFunction | semantic::SymbolFlags::kTemplate))) {
    return nullptr;
  }

  return ast::utils::GetCallableDeclParams(fun_sym->Declaration()->As<ast::nodes::Decl>());
}

std::string_view GetReadableTypeName(const SourceFile* sf, const semantic::Symbol* sym) {
  const auto* decl = sym->Declaration();
  switch (decl->nkind) {
    case ast::NodeKind::StructTypeDecl: {
      const auto* m = decl->As<ast::nodes::StructTypeDecl>();
      return sf->Text(ast::Range{
          .begin = m->kind.range.begin,
          .end = m->name->nrange.end,
      });
    }
    case ast::NodeKind::SubTypeDecl: {
      const auto* m = decl->As<ast::nodes::SubTypeDecl>();
      return sf->Text(ast::Range{
          .begin = m->field->nrange.begin,
          .end = m->field->name->nrange.end,
      });
    }
    case ast::NodeKind::ClassTypeDecl: {
      const auto* m = decl->As<ast::nodes::ClassTypeDecl>();
      return sf->Text(ast::Range{
          .begin = m->kind.range.begin,
          .end = m->name->nrange.end,
      });
    }
    case ast::NodeKind::ConstructorDecl: {
      return "class constructor";
    }
    default:
      return sf->Text(sym->Declaration());
  }
}
std::string_view GetReadableTypeName(const semantic::Symbol* sym) {
  if (sym->Flags() & semantic::SymbolFlags::kBuiltin) {
    return sym->GetName();
  }
  return GetReadableTypeName(ast::utils::SourceFileOf(sym->Declaration()), sym);
}

}  // namespace utils

namespace {
namespace detail {

std::optional<std::pair<std::size_t, std::size_t>> GetLengthExprBounds(const SourceFile* sf,
                                                                       const ast::nodes::LengthExpr* lc) {
  if (!lc->size || lc->size->nkind != ast::NodeKind::ParenExpr) {
    return std::nullopt;
  }

  const auto* pe = lc->size->As<ast::nodes::ParenExpr>();
  if (pe->list.size() != 1 || pe->list.front()->nkind != ast::NodeKind::BinaryExpr) {
    return std::nullopt;
  }

  // TODO: check against constants
  const auto* be = pe->list.front()->As<ast::nodes::BinaryExpr>();
  const auto lo_str = sf->Text(be->x);
  std::size_t min_args{};
  if (be->x->nkind != ast::NodeKind::ValueLiteral ||
      std::from_chars(lo_str.data(), lo_str.data() + lo_str.size(), min_args).ec != std::errc{}) {
    return std::nullopt;
  }

  const auto hi_str = sf->Text(be->y);
  std::size_t max_args{};
  if (be->y->nkind == ast::NodeKind::ValueLiteral && hi_str == "infinity") {
    max_args = static_cast<std::size_t>(-1);
  } else if (std::from_chars(hi_str.data(), hi_str.data() + hi_str.size(), max_args).ec != std::errc{}) {
    return std::nullopt;
  }

  return std::make_pair(min_args, max_args);
}

template <typename F>
concept IsInstantiatedTypeResolver =
    std::is_invocable_r_v<InstantiatedType, F, const SourceFile*, const semantic::Scope*, const ast::nodes::Expr*>;

template <IsInstantiatedTypeResolver InstantiatedTypeResolver, typename OnNonStructuralType, typename OnUnknownProperty,
          typename OnInvalidNonStaticPropertyAccess, typename OnInvalidStaticPropertyAccess>
  requires(
      std::is_invocable_v<OnNonStructuralType, const ast::Node*, const semantic::Symbol*> &&
      std::is_invocable_v<OnUnknownProperty, const ast::nodes::SelectorExpr*, const semantic::Symbol*> &&
      std::is_invocable_v<OnInvalidNonStaticPropertyAccess, const ast::nodes::SelectorExpr*, const semantic::Symbol*> &&
      std::is_invocable_v<OnInvalidStaticPropertyAccess, const ast::nodes::SelectorExpr*, const semantic::Symbol*>)
struct SelectorExprResolverOptions {
  InstantiatedTypeResolver resolve_type;
  OnNonStructuralType on_non_structural_type;
  OnUnknownProperty on_unknown_property;

  OnInvalidNonStaticPropertyAccess on_non_static_property_invalid_access;
  OnInvalidStaticPropertyAccess on_static_property_invalid_access;
};

template <typename Options>
class SelectorExprResolver {
 public:
  SelectorExprResolver(Options options) : options_(std::move(options)) {}

  const semantic::Symbol* Resolve(const SourceFile* sf, const semantic::Scope* scope,
                                  const ast::nodes::SelectorExpr* se) {
    sf_ = sf;
    scope_ = scope;
    se_tgt_ = se;
    return ResolveSelectorExpr(se);
  }

 private:
  bool mode_static_;
  const semantic::Symbol* ResolveSelectorExpr(const ast::nodes::SelectorExpr* se) {
    if (se->sel == nullptr) [[unlikely]] {
      return nullptr;
    }

    const semantic::Symbol* x_sym{nullptr};
    if (se->x->nkind == ast::NodeKind::SelectorExpr) {
      x_sym = ResolveSelectorExpr(se->x->As<ast::nodes::SelectorExpr>());
    } else {
      const auto type = options_.resolve_type(sf_, scope_, se->x);
      x_sym = type.sym;
      mode_static_ = !type.instance && x_sym && !bool(x_sym->Flags() & semantic::SymbolFlags::kImportedModule);
    }

    if (!x_sym) {
      return nullptr;
    }

    if (x_sym->Flags() & semantic::SymbolFlags::kTemplate) {
      mode_static_ = false;
      const auto* tdecl = x_sym->Declaration();
      const auto* tfile = ast::utils::SourceFileOf(tdecl);
      x_sym = ResolveCallableReturnType(tfile, tfile->module->scope, tdecl->As<ast::nodes::Decl>());
      if (!x_sym) {
        return nullptr;
      }
    }

    if (!(x_sym->Flags() & (semantic::SymbolFlags::kStructural | semantic::SymbolFlags::kClass |
                            semantic::SymbolFlags::kImportedModule))) {
      options_.on_non_structural_type(se->sel, x_sym);
      return nullptr;
    }

    const auto property_name = sf_->Text(se->sel);
    const semantic::Symbol* property_sym = [&] -> const semantic::Symbol* {
      if (x_sym->Flags() & semantic::SymbolFlags::kStructural) [[likely]] {
        return x_sym->Members()->Lookup(property_name);
      }
      if (x_sym->Flags() & semantic::SymbolFlags::kClass) [[likely]] {
        return x_sym->OriginatedScope()->ResolveHorizontally(property_name);
      }
      if (x_sym->Flags() & semantic::SymbolFlags::kImportedModule) {
        const auto* tgt_module = sf_->program->GetModule(x_sym->GetName());
        if (!tgt_module) {
          return nullptr;
        }
        return tgt_module->scope->ResolveDirect(property_name);
      }
      assert(false);
      return nullptr;
    }();
    if (!property_sym) {
      // TODO: remove file lookup from contract
      options_.on_unknown_property(se, x_sym);
      return nullptr;
    }

    if (mode_static_ && !(property_sym->Flags() & semantic::SymbolFlags::kVisibilityStatic)) [[unlikely]] {
      if (x_sym->Flags() & semantic::SymbolFlags::kStructural) [[likely]] {
        return x_sym->Members()->LookupShadow(property_name);
      }
      options_.on_non_static_property_invalid_access(se, x_sym);
      return nullptr;
    } else if (!mode_static_ && (property_sym->Flags() & semantic::SymbolFlags::kVisibilityStatic)) [[unlikely]] {
      options_.on_static_property_invalid_access(se, x_sym);
      return nullptr;
    }

    if (se == se_tgt_) {
      // we don't need to resolve type then
      return property_sym;
    }

    const auto* property_decl = property_sym->Declaration();
    if (property_decl->nkind == ast::NodeKind::Field &&
        property_decl->As<ast::nodes::Field>()->type->nkind != ast::NodeKind::RefSpec) {
      return x_sym->Members()->LookupShadow(property_name);
    }

    const auto* property_file = ast::utils::SourceFileOf(property_sym->Declaration());
    return ResolveDeclarationType(property_file, property_file->module->scope, property_decl->As<ast::nodes::Decl>());
  }

  const SourceFile* sf_;
  const semantic::Scope* scope_;
  const ast::nodes::SelectorExpr* se_tgt_;
  Options options_;
};
const semantic::Symbol* ResolveSelectorExprSymbol(const SourceFile* sf, const semantic::Scope* scope,
                                                  const ast::nodes::SelectorExpr* se) {
  SelectorExprResolver resolver{SelectorExprResolverOptions{
      .resolve_type = ResolveExprInstantiatedType,
      .on_non_structural_type = [](const auto*, auto) {},
      .on_unknown_property = [](const auto*, auto) {},
      .on_non_static_property_invalid_access = [](const auto*, auto) {},
      .on_static_property_invalid_access = [](const auto*, auto) {},
  }};
  return resolver.Resolve(sf, scope, se);
}

template <IsInstantiatedTypeResolver InstantiatedTypeResolver, typename OnNonSubscriptableType, typename IndexChecker>
  requires(std::is_invocable_v<OnNonSubscriptableType, const ast::Node*, const semantic::Symbol*> &&
           std::is_invocable_v<IndexChecker, const ast::nodes::Expr*>)
struct IndexExprResolverOptions {
  InstantiatedTypeResolver resolve_type;
  OnNonSubscriptableType on_non_subscriptable_type;
  IndexChecker check_index;
};

template <typename Options>
class IndexExprResolver {
 public:
  IndexExprResolver(Options options) : options_(std::move(options)) {}

  const semantic::Symbol* Resolve(const SourceFile* sf, const semantic::Scope* scope, const ast::nodes::IndexExpr* ie) {
    sf_ = sf;
    scope_ = scope;

    const auto* res = ResolveIndexExpr(ie);
    if (depth_ != 0) [[unlikely]] {
      return nullptr;
    }
    return res;
  }

 private:
  std::size_t depth_{0};
  const semantic::Symbol* ResolveIndexExpr(const ast::nodes::IndexExpr* ie) {
    options_.check_index(ie->index);

    const semantic::Symbol* x_sym{nullptr};
    if (ie->x->nkind == ast::NodeKind::IndexExpr) {
      x_sym = ResolveIndexExpr(ie->x->As<ast::nodes::IndexExpr>());
      if (depth_ > 0) [[unlikely]] {
        --depth_;
        return x_sym;
      }
    } else {
      const auto type = options_.resolve_type(sf_, scope_, ie->x);
      x_sym = type.sym;
      if (!type.instance) [[unlikely]] {
        if (type.sym) {
          options_.on_non_subscriptable_type(ie->x, x_sym);
        }
        return nullptr;
      }
      if (const auto* arraydef_vec = ast::utils::GetArrayDef(type.instance); arraydef_vec && !arraydef_vec->empty())
          [[unlikely]] {
        depth_ = arraydef_vec->size() - 1;
        return x_sym;
      }
    }

    if (!x_sym) {
      return nullptr;
    }

    if (x_sym->Flags() & semantic::SymbolFlags::kBuiltinString) {
      return x_sym;
    }

    if (x_sym->Flags() & semantic::SymbolFlags::kTemplate) {
      // yes, it does not use options_.resolve_type - avoids double error emit
      const auto* td = x_sym->Declaration()->As<ast::nodes::TemplateDecl>();
      const auto* tf = ast::utils::SourceFileOf(td);
      x_sym = ResolveExprType(tf, tf->module->scope, td->type);
      if (!x_sym) {
        return nullptr;
      }
    }

    if (!(x_sym->Flags() & semantic::SymbolFlags::kSubtype)) {
      options_.on_non_subscriptable_type(ie->x, x_sym);
      return nullptr;
    }

    const auto* subtype_decl = x_sym->Declaration()->As<ast::nodes::SubTypeDecl>();
    const auto* subtype_file = ast::utils::SourceFileOf(subtype_decl);

    const auto* f = subtype_decl->field;
    if (f->type->nkind != ast::NodeKind::ListSpec) {
      options_.on_non_subscriptable_type(ie->x, x_sym);
      return nullptr;
    }
    const auto* ls = f->type->As<ast::nodes::ListSpec>();
    const auto* rs = ls->elemtype->As<ast::nodes::RefSpec>();

    return subtype_file->module->scope->Resolve(subtype_file->Text(rs->x));
  }

  const SourceFile* sf_;
  const semantic::Scope* scope_;
  Options options_;
};
const semantic::Symbol* ResolveIndexExprType(const SourceFile* sf, const semantic::Scope* scope,
                                             const ast::nodes::IndexExpr* se) {
  IndexExprResolver resolver{IndexExprResolverOptions{
      .resolve_type = ResolveExprInstantiatedType,
      .on_non_subscriptable_type = [](const auto*, auto) {},
      .check_index = [](const auto*) {},
  }};
  return resolver.Resolve(sf, scope, se);
}

}  // namespace detail

const semantic::Symbol* ResolveAliasedType(const semantic::Symbol* sym) {
  const ast::Node* decl = sym->Declaration()->As<ast::nodes::SubTypeDecl>()->field->type;
  while (decl && decl->nkind == ast::NodeKind::RefSpec) {
    const auto* file = ast::utils::SourceFileOf(decl);
    sym = ResolveExprType(file, file->module->scope, decl->As<ast::nodes::RefSpec>()->x);
    if (!sym) [[unlikely]] {
      break;
    }
    decl = sym->Declaration();
  }

  return sym;
}
}  // namespace

const semantic::Symbol* ResolvePotentiallyAliasedType(const semantic::Symbol* sym) {
  if (!(sym->Flags() & semantic::SymbolFlags::kSubtype)) [[likely]] {
    return sym;
  }
  return ResolveAliasedType(sym);
}

namespace ext {
const semantic::Symbol* ResolveAssignmentTarget(const SourceFile* file, const semantic::Scope* scope,
                                                const ast::nodes::AssignmentExpr* n) {
  const auto* property = n->property;
  const auto property_name = file->Text(property);

  switch (n->parent->nkind) {
    case core::ast::NodeKind::ParenExpr: {
      const auto* m = n->parent->As<core::ast::nodes::ParenExpr>();

      const auto* ce = m->parent->As<core::ast::nodes::CallExpr>();
      const auto* callee_sym = core::checker::ResolveExprSymbol(file, scope, ce->fun);
      if (!callee_sym ||
          !(callee_sym->Flags() & (core::semantic::SymbolFlags::kFunction | semantic::SymbolFlags::kTemplate))) {
        return nullptr;
      }

      return callee_sym->OriginatedScope()->ResolveDirect(property_name);
    }
    case core::ast::NodeKind::CompositeLiteral: {
      const auto* m = n->parent->As<core::ast::nodes::CompositeLiteral>();
      const auto* sym = core::checker::ext::DeduceCompositeLiteralType(file, scope, m);
      if (!sym) {
        return nullptr;
      }
      return sym->Members()->Lookup(property_name);
    }
    default: {
      return core::checker::ResolveExprSymbol(file, scope, property);
    }
  }
}
const semantic::Symbol* DeduceCompositeLiteralType(const SourceFile* file, const semantic::Scope* scope,
                                                   const ast::nodes::CompositeLiteral* n,
                                                   const semantic::Symbol* parent_hint) {
  switch (n->parent->nkind) {
    case ast::NodeKind::CompositeLiteral: {
      const auto* cl = n->parent->As<ast::nodes::CompositeLiteral>();

      const auto* sym = parent_hint ? parent_hint : DeduceCompositeLiteralType(file, scope, cl);
      if (!sym) {
        return nullptr;
      }

      if (sym->Flags() & semantic::SymbolFlags::kSubtype) {
        sym = ResolveAliasedType(sym);
        const ast::Node* decl = sym->Declaration()->As<ast::nodes::SubTypeDecl>()->field->type;
        if (decl && decl->nkind == ast::NodeKind::ListSpec) {
          const auto* decl_file = ast::utils::SourceFileOf(decl);
          sym = ResolveExprType(
              decl_file, decl_file->module->scope,
              decl->As<ast::nodes::ListSpec>()->elemtype->As<ast::nodes::Expr>());  // TODO: typespec expr
          return sym;
        }
        return nullptr;
      }

      const auto* decl = sym->Declaration();
      if (decl->nkind != core::ast::NodeKind::StructTypeDecl && decl->nkind != ast::NodeKind::StructSpec) {
        return nullptr;
      }

      const auto& fields = *ast::utils::GetStructFields(decl);

      const auto param_index = std::ranges::find(cl->list, n) - cl->list.begin();
      if (param_index >= std::ssize(fields)) {
        return nullptr;
      }

      const auto* field = fields[param_index];
      const auto* decl_file = core::ast::utils::SourceFileOf(decl);

      if (field->type->nkind != ast::NodeKind::RefSpec && field->name) {
        return sym->Members()->LookupShadow(decl_file->Text(*field->name));
      }

      return ResolveExprType(decl_file, decl_file->module->scope, field->type->As<ast::nodes::Expr>());
    }
    case ast::NodeKind::AssignmentExpr: {
      const auto* ae = n->parent->As<ast::nodes::AssignmentExpr>();
      const auto* property = ae->property;
      const auto property_name = file->Text(property);
      switch (ae->parent->nkind) {
        case ast::NodeKind::CompositeLiteral: {
          const auto* cl = ae->parent->As<ast::nodes::CompositeLiteral>();
          const auto* cl_sym = parent_hint ? parent_hint : DeduceCompositeLiteralType(file, scope, cl);
          if (!cl_sym) {
            return nullptr;
          }

          if (cl_sym->Flags() & semantic::SymbolFlags::kSubtype) {
            cl_sym = ResolveAliasedType(cl_sym);
            const ast::Node* decl = cl_sym->Declaration()->As<ast::nodes::SubTypeDecl>()->field->type;
            if (decl && decl->nkind == ast::NodeKind::ListSpec && ae->property->nkind == ast::NodeKind::IndexExpr) {
              const auto* decl_file = ast::utils::SourceFileOf(decl);
              cl_sym = ResolveExprType(
                  decl_file, decl_file->module->scope,
                  decl->As<ast::nodes::ListSpec>()->elemtype->As<ast::nodes::Expr>());  // TODO: typespec expr
            }
            return cl_sym;
          }

          const auto* property_sym = cl_sym->Members()->Lookup(property_name);
          if (!property_sym) {
            return nullptr;
          }
          const auto* property_file = ast::utils::SourceFileOf(property_sym->Declaration());

          return ResolveDeclarationType(property_file, property_file->module->scope,
                                        property_sym->Declaration()->As<ast::nodes::Decl>());
        }
        case ast::NodeKind::ParenExpr: {
          const auto* pe = ae->parent->As<ast::nodes::ParenExpr>();
          const auto* params = utils::ResolveCallableParams(file, scope, pe);
          if (!params) {
            return nullptr;
          }

          const auto* params_file = ast::utils::SourceFileOf(params);

          const auto it = std::ranges::find_if(params->list, [&](const ast::nodes::FormalPar* par) {
            return par->name && params_file->Text(*par->name) == property_name;
          });
          if (it == params->list.end()) {
            return nullptr;
          }

          const auto* param = *it;
          return ResolveExprType(params_file, params_file->module->scope, param->type);
        }
        case ast::NodeKind::ExprStmt: {
          const auto* es = ae->parent->As<ast::nodes::ExprStmt>();
          return ResolveExprType(file, scope, es->expr);
        }
        default:
          break;
      }
    }
    case ast::NodeKind::ParenExpr: {
      const auto* pe = n->parent->As<ast::nodes::ParenExpr>();
      const auto* params = utils::ResolveCallableParams(file, scope, pe);
      if (!params) {
        return nullptr;
      }

      const auto param_index = std::ranges::find(pe->list, n) - pe->list.begin();
      if (param_index >= std::ssize(params->list)) {
        return nullptr;
      }

      const auto* param = params->list[param_index];
      const auto* params_file = ast::utils::SourceFileOf(params);

      return ResolveExprType(params_file, params_file->module->scope, param->type);
    }
    case ast::NodeKind::Declarator: {
      const auto* decl = n->parent->As<ast::nodes::Declarator>();
      const auto* vd = decl->parent->As<ast::nodes::ValueDecl>();
      return ResolveExprType(file, scope, vd->type);
    }
    case ast::NodeKind::TemplateDecl: {
      const auto* decl = n->parent->As<ast::nodes::TemplateDecl>();
      return ResolveExprType(file, scope, decl->type);
    }
    case ast::NodeKind::ReturnStmt: {
      const auto* rs = n->parent->As<ast::nodes::ReturnSpec>();
      const auto* decl = ast::utils::GetPredecessor<ast::nodes::FuncDecl>(rs);
      if (!decl || !decl->ret) {
        return nullptr;
      }
      return ResolveExprType(file, scope, decl->ret->type);
    }
    default:
      return nullptr;
  }
}

const semantic::Symbol* DeduceExpectedType(const SourceFile* file, const semantic::Scope* scope, const ast::Node* n) {
  const auto* parent = n->parent;
  switch (parent->nkind) {
    case ast::NodeKind::Declarator: {
      const auto* d = parent->As<ast::nodes::Declarator>();
      if (n != d->value) {
        break;
      }

      const auto* vd = d->parent->As<ast::nodes::ValueDecl>();
      return ResolveExprType(file, scope, vd->type);
    }
    case ast::NodeKind::FormalPar: {
      const auto* fp = parent->As<ast::nodes::FormalPar>();
      if (n != fp->value) {
        break;
      }
      return ResolveExprType(file, scope, fp->type);
    }
    case ast::NodeKind::AssignmentExpr: {
      const auto* ae = parent->As<ast::nodes::AssignmentExpr>();
      if (n != ae->value) {
        break;
      }

      const auto* decl_sym = ResolveAssignmentTarget(file, scope, ae);
      if (!decl_sym) {
        break;
      }

      const auto* decl = decl_sym->Declaration()->As<ast::nodes::Decl>();
      const auto* decl_file = ast::utils::SourceFileOf(decl);
      const auto* decl_scope = decl_file->module->scope;

      return ResolveDeclarationType(decl_file, decl_scope, decl);
    }
    case ast::NodeKind::ParenExpr: {
      const auto* pe = parent->As<ast::nodes::ParenExpr>();

      const auto* params = utils::ResolveCallableParams(file, file->module->scope, pe);
      if (!params) {
        break;
      }

      const auto idx = std::ranges::find(pe->list, n) - pe->list.begin();
      if (idx >= std::ssize(params->list)) {
        return nullptr;
      }

      const auto* params_file = ast::utils::SourceFileOf(params);
      const auto* params_scope = params_file->module->scope;

      return ResolveExprType(params_file, params_scope, params->list[idx]->type);
    }
    case ast::NodeKind::CompositeLiteral: {
      const auto* cl = parent->As<ast::nodes::CompositeLiteral>();

      const auto* sym = DeduceCompositeLiteralType(file, scope, cl);
      if (!(sym->Flags() & semantic::SymbolFlags::kStructural)) {
        // e.g. ListSpec
        return sym;
      }
      const auto* decl = sym->Declaration();

      const auto* fields_ptr = ast::utils::GetStructFields(decl);
      if (!fields_ptr) {
        break;
      }
      const auto& fields = *fields_ptr;

      const auto idx = std::ranges::find(cl->list, n) - cl->list.begin();
      if (idx >= std::ssize(fields)) {
        return nullptr;
      }

      const auto* fields_file = ast::utils::SourceFileOf(decl);
      const auto* fields_scope = fields_file->module->scope;

      return ResolveExprType(fields_file, fields_scope, fields[idx]->type->As<ast::nodes::Expr>());
    }
    default:
      break;
  }
  return nullptr;
}

}  // namespace ext

// Expression -> Declaration
const semantic::Symbol* ResolveExprSymbol(const SourceFile* file, const semantic::Scope* scope,
                                          const ast::nodes::Expr* expr) {
  switch (expr->nkind) {
    case ast::NodeKind::Ident: {
      const auto* m = expr->As<ast::nodes::Ident>();
      return scope->Resolve(file->Text(m));
    }
    case ast::NodeKind::CallExpr: {
      const auto* m = expr->As<ast::nodes::CallExpr>();
      const auto* fun = m->fun;
      return ResolveExprSymbol(file, scope, fun);
    }
    case ast::NodeKind::SelectorExpr: {
      const auto* m = expr->As<ast::nodes::SelectorExpr>();
      return detail::ResolveSelectorExprSymbol(file, scope, m);
    }
    case ast::NodeKind::IndexExpr: {
      const auto* m = expr->As<ast::nodes::IndexExpr>();
      return detail::ResolveIndexExprType(file, scope, m);
    }
    case ast::NodeKind::StructTypeDecl: {
      const auto* m = expr->As<ast::nodes::StructTypeDecl>();
      if (!m->name) [[unlikely]] {
        return nullptr;
      }
      return scope->ResolveDirect(file->Text(*m->name));
    }
    case ast::NodeKind::RefSpec: {
      const auto* m = expr->As<ast::nodes::RefSpec>();
      return ResolveExprSymbol(file, scope, m->x);
    }
    case ast::NodeKind::StructSpec:
    case ast::NodeKind::EnumSpec:
    case ast::NodeKind::ListSpec: {
      const auto* m = expr->As<ast::nodes::StructSpec>();
      const auto* parent = m->parent;
      if (parent->nkind != core::ast::NodeKind::Field) [[unlikely]] {
        return nullptr;
      }
      const auto* owner = parent->As<core::ast::nodes::Field>();
      if (!owner->name) {
        return nullptr;
      }
      const core::semantic::Symbol* containing_sym =
          ResolveExprSymbol(file, scope, owner->parent->As<ast::nodes::Expr>());
      if (!containing_sym || !(containing_sym->Flags() & core::semantic::SymbolFlags::kStructural)) {
        return nullptr;
      }
      return containing_sym->Members()->LookupShadow(file->Text(*owner->name));
    }
    default: {
      return nullptr;
    }
  }
}

// Value Declaration -> Effective Type
const semantic::Symbol* ResolveDeclarationType(const SourceFile* file, const semantic::Scope* scope,
                                               const ast::nodes::Decl* decl) {
  switch (decl->nkind) {
    case ast::NodeKind::Declarator: {
      const auto* m = decl->As<ast::nodes::Declarator>();
      const auto* vd = m->parent->As<ast::nodes::ValueDecl>();
      return ResolveExprType(file, scope, vd->type);
    }
    case ast::NodeKind::FormalPar: {
      const auto* m = decl->As<ast::nodes::FormalPar>();
      return ResolveExprType(file, scope, m->type);
    }
    case ast::NodeKind::Field: {
      const auto* m = decl->As<ast::nodes::Field>();
      return ResolveExprType(file, scope, m->type->As<ast::nodes::Expr>());  // TODO: it is not Expr though
    }
    case ast::NodeKind::ClassTypeDecl: {
      // we could come here only via Resolve("this")->Declaration()
      // use direct module symbol table for speed up
      const auto* m = decl->As<ast::nodes::ClassTypeDecl>();
      return file->module->scope->ResolveDirect(file->Text(*m->name));
    }
    case ast::NodeKind::FuncDecl: {
      const auto* m = decl->As<ast::nodes::FuncDecl>();
      const auto* declfile = ast::utils::SourceFileOf(m);
      return declfile->module->scope->ResolveDirect(declfile->Text(*m->name));
    }
    case ast::NodeKind::TemplateDecl: {
      const auto* m = decl->As<ast::nodes::TemplateDecl>();
      const auto* declfile = ast::utils::SourceFileOf(m);
      return declfile->module->scope->ResolveDirect(declfile->Text(*m->name));
    }
    default: {
      return nullptr;
    }
  }
}

// Callable Declaration -> Effective Type
const semantic::Symbol* ResolveCallableReturnType(const SourceFile* file, const semantic::Scope* scope,
                                                  const ast::nodes::Decl* decl) {
  switch (decl->nkind) {
    case ast::NodeKind::FuncDecl: {
      const auto* m = decl->As<ast::nodes::FuncDecl>();
      const auto* ret = m->ret;
      if (!ret) {
        return &symbols::kVoidType;
      }
      return ResolveExprType(file, scope, m->ret->type);
    }
    case ast::NodeKind::TemplateDecl: {
      const auto* m = decl->As<ast::nodes::TemplateDecl>();
      return ResolveExprType(file, scope, m->type);
    }
    case ast::NodeKind::ConstructorDecl: {
      const auto* m = decl->As<ast::nodes::FuncDecl>();
      const auto* classdecl = m->parent->parent->As<ast::nodes::ClassTypeDecl>();  // Def->ClassTypeDecl
      return file->module->scope->ResolveDirect(file->Text(*classdecl->name));
    }
    default: {
      return nullptr;
    }
  }
}

namespace {
const semantic::Symbol* ResolveExprTypeViaDecl(const SourceFile* file, const semantic::Scope* scope,
                                               const semantic::Symbol* decl_sym, const ast::nodes::Expr* expr) {
  if ((decl_sym->Flags() &
       (semantic::SymbolFlags::kType | semantic::SymbolFlags::kImportedModule | semantic::SymbolFlags::kEnumMember)) ||
      (expr->nkind != ast::NodeKind::CallExpr && (decl_sym->Flags() & semantic::SymbolFlags::kFunction))) {
    return decl_sym;
  }

  const auto* decl = decl_sym->Declaration()->As<ast::nodes::Decl>();
  const auto* decl_file = ast::utils::SourceFileOf(decl);
  const auto* decl_scope = decl_file->module->scope;

  if (expr->nkind == ast::NodeKind::CallExpr) {
    const auto* ret_sym = ResolveCallableReturnType(decl_file, decl_scope, decl);
    if (ret_sym == &symbols::kInferType) {
      const auto* ce = expr->As<ast::nodes::CallExpr>();
      if (ce->args->list.empty()) {
        return nullptr;
      }
      return ResolveExprType(file, scope, ce->args->list.front());
    }
    return ret_sym;
  }

  return ResolveDeclarationType(decl_file, decl_scope, decl);
}
}  // namespace

// Expression -> (Declaration) -> Effective Type
const semantic::Symbol* ResolveExprType(const SourceFile* file, const semantic::Scope* scope,
                                        const ast::nodes::Expr* expr) {
  const auto* decl_sym = ResolveExprSymbol(file, scope, expr);
  if (!decl_sym) {
    return nullptr;
  }
  return ResolveExprTypeViaDecl(file, scope, decl_sym, expr);
}

namespace {
InstantiatedType ResolveExprInstantiatedType(const SourceFile* file, const semantic::Scope* scope,
                                             const ast::nodes::Expr* expr) {
  const auto* decl_sym = ResolveExprSymbol(file, scope, expr);
  if (!decl_sym) {
    return {};
  }
  const auto* type_sym = ResolveExprTypeViaDecl(file, scope, decl_sym, expr);
  return {
      .sym = type_sym,
      .instance = [&] -> const ast::Node* {
        switch (expr->nkind) {
          case ast::NodeKind::IndexExpr:
            return expr;
          default:
            break;
        }

        if (decl_sym == type_sym) {
          return nullptr;
        };

        const auto* decl = decl_sym->Declaration();
        switch (decl->nkind) {
          case ast::NodeKind::FormalPar:
          case ast::NodeKind::Declarator:
            return decl;
          case ast::NodeKind::ClassTypeDecl:  // this.
            return expr;
          default:
            return nullptr;
        }
      }(),
  };
}
}  // namespace

class BasicTypeChecker {
 public:
  explicit BasicTypeChecker(SourceFile& sf)
      : sf_(sf), inspector_(ast::NodeInspector::create<BasicTypeChecker, &BasicTypeChecker::Inspect>(this)) {}

  void Check() {
    semantic::InspectScope(
        sf_.module->scope,
        [&](const semantic::Scope* scope_under_inspection) {
          scope_ = scope_under_inspection;
        },
        [&](const ast::Node* n) {
          return Inspect(n);
        });
  }

 private:
  void EmitError(TypeError&& err) {
    sf_.type_errors.emplace_back(std::move(err));
  }

  void MatchTypes(const ast::Range& range, const InstantiatedType& actual, const InstantiatedType& expected);

  InstantiatedType CheckType(const ast::Node* n, const semantic::Symbol* desired_type = nullptr);
  bool Inspect(const ast::Node*);

  void Introspect(const ast::Node* n) {
    n->Accept(inspector_);
  }
  void Visit(const ast::Node* n) {
    if (Inspect(n)) {
      Introspect(n);
    }
  }

  void EnsureIsAType(const semantic::Symbol* sym, const ast::Node* n) {
    if (sym && sym != &symbols::kTypeError && !(sym->Flags() & semantic::SymbolFlags::kType)) {
      EmitError({
          .range = n->nrange,
          .message = std::format("'{}' is not a type", sf_.Text(n)),
      });
    }
  }

  struct ArgumentsTypeCheckOptions {
    bool is_union{false};
    bool allow_missing_fields{false};
  };

  template <ast::IsNode TParamDescriptorNode, ArgumentsTypeCheckOptions Options>
  void PerformArgumentsTypeCheck(std::span<const ast::nodes::Expr* const> args, const ast::Range& args_range,
                                 const SourceFile* params_file, const semantic::Symbol* subject_type_sym,
                                 std::span<const TParamDescriptorNode* const> params,
                                 std::predicate<const TParamDescriptorNode*> auto is_param_required);

  const semantic::Scope* scope_;

  SourceFile& sf_;
  const ast::NodeInspector inspector_;
};

void BasicTypeChecker::MatchTypes(const ast::Range& range, const InstantiatedType& actual_,
                                  const InstantiatedType& expected) {
  if (!expected || expected.sym == &builtins::kAnytype) {
    return;
  }

  InstantiatedType actual{actual_};  // TODO
  if (actual && actual->Flags() & semantic::SymbolFlags::kTemplate) {
    const auto* file = ast::utils::SourceFileOf(actual->Declaration());
    actual.sym = ResolveCallableReturnType(file, file->module->scope, actual->Declaration()->As<ast::nodes::Decl>());
  }
  if (!actual) {
    // Seems like that is is too much as right side types that cannot be resolved
    // will be reported by the semantic analyzeer

    // EmitError(TypeError{
    //     .range = range,
    //     .message = std::format("expected argument of type '{}', got unknown type",
    //                            utils::GetReadableTypeName(expected)),
    // });
    return;
  }

  if (expected->Flags() & semantic::SymbolFlags::kEnum) {
    if (expected != actual && !expected->Members()->Has(actual->GetName())) {
      EmitError(TypeError{
          .range = range,
          .message = std::format("value '{}' does not belong to enum '{}'", actual->GetName(), expected->GetName()),
      });
    }
    return;
  }

  // TODO: read language spec and maybe make something more precise
  if (ResolvePotentiallyAliasedType(expected.sym) == ResolvePotentiallyAliasedType(actual.sym)) {
    return;
  }

  EmitError(TypeError{
      .range = range,
      .message = std::format("expected value of type '{}', got '{}'", utils::GetReadableTypeName(expected.sym),
                             utils::GetReadableTypeName(actual.sym)),
  });
}

template <ast::IsNode TParamDescriptorNode, BasicTypeChecker::ArgumentsTypeCheckOptions Options = {}>
void BasicTypeChecker::PerformArgumentsTypeCheck(std::span<const ast::nodes::Expr* const> args,
                                                 const ast::Range& args_range, const SourceFile* params_file,
                                                 const semantic::Symbol* subject_type_sym,
                                                 std::span<const TParamDescriptorNode* const> params,
                                                 std::predicate<const TParamDescriptorNode*> auto is_param_required) {
  const auto args_count = args.size();

  const auto minimal_args_cnt = static_cast<std::size_t>(std::ranges::count_if(params, is_param_required));
  const auto maximal_args_cnt = params.size();

  if constexpr (!Options.is_union && !Options.allow_missing_fields) {
    if (args_count < minimal_args_cnt || args_count > maximal_args_cnt) {
      EmitError(TypeError{
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
    const auto* exp_sym =  // TODO: TypeSpec (field->type) is not Expr actually
        ResolveExprType(params_file, params_file->module->scope, param->type->template As<ast::nodes::Expr>());
    const auto actual_instance = CheckType(n, exp_sym);
    MatchTypes(n->nrange, actual_instance, {.sym = exp_sym});
  };

  bool seen_named_argument{false};
  for (std::size_t i = 0; i < args_count; ++i) {
    const auto* argnode = args[i];
    switch (argnode->nkind) {
      case ast::NodeKind::AssignmentExpr: {
        const auto* ae = argnode->As<ast::nodes::AssignmentExpr>();
        seen_named_argument = true;

        if (ae->property->nkind != ast::NodeKind::Ident) [[unlikely]] {
          // most likely it is IndexExpr, e.g. { [0] := value }
          EmitError(TypeError{
              .range = ae->nrange,
              .message =
                  std::format("type '{}' is not array-like", utils::GetReadableTypeName(params_file, subject_type_sym)),
          });
          continue;
        }

        const auto property_name = sf_.Text(ae->property);
        const semantic::Symbol* property_sym = [&] {
          if constexpr (std::is_same_v<TParamDescriptorNode, ast::nodes::FormalPar>) {
            return subject_type_sym->OriginatedScope()->ResolveDirect(property_name);
          } else {
            return subject_type_sym->Members()->Lookup(property_name);
          }
        }();
        if (property_sym) {
          check_argument(property_sym->Declaration()->As<TParamDescriptorNode>(), ae->value);
        } else {
          EmitError(TypeError{
              .range = ae->property->nrange,
              .message = [&] -> std::string {
                if constexpr (std::is_same_v<TParamDescriptorNode, ast::nodes::FormalPar>) {
                  return std::format("callable '{}' does not have an argument with name '{}'",
                                     utils::GetReadableTypeName(params_file, subject_type_sym), property_name);
                } else {
                  return std::format("property '{}' does not exist on type '{}'", property_name,
                                     utils::GetReadableTypeName(params_file, subject_type_sym));
                }
              }(),
          });
          Visit(argnode);
        }

        break;
      }
      default: {
        if (seen_named_argument && !Options.is_union) {
          EmitError(TypeError{
              .range = argnode->nrange,
              .message = "positional arguments cannot follow named ones",
          });
        }
        if (i >= maximal_args_cnt) {
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
      EmitError(TypeError{
          .range = args_range,
          .message = std::format("exactly one named argument is expected in union"),
      });
    }
  }
}

InstantiatedType BasicTypeChecker::CheckType(const ast::Node* n, const semantic::Symbol* desired_type) {
  InstantiatedType resulting_type{};
  switch (n->nkind) {
    case ast::NodeKind::CallExpr: {
      const auto* m = n->As<ast::nodes::CallExpr>();
      const auto* callee_sym = CheckType(m->fun).sym;
      if (!callee_sym) {
        // error is raised by the semantic analyzer in such case
        Inspect(m->args);
        break;
      }

      if (!(callee_sym->Flags() & (semantic::SymbolFlags::kFunction | semantic::SymbolFlags::kTemplate))) {
        EmitError(TypeError{
            .range = m->fun->nrange,
            .message = std::format("'{}' is not callable", sf_.Text(m->fun)),
        });
        Inspect(m->args);
        break;
      }

      const auto* callee_decl = callee_sym->Declaration()->As<ast::nodes::Decl>();
      const auto* callee_file = ast::utils::SourceFileOf(callee_decl);

      //
      resulting_type.sym = ResolveCallableReturnType(callee_file, callee_file->module->scope, callee_decl);
      //

      const ast::nodes::FormalPars* params = ast::utils::GetCallableDeclParams(callee_decl);
      if (!params) [[unlikely]] {
        // this is guaranteed to be possible due to parser leniency
        Inspect(m->args);
        break;
      }

      //
      if (resulting_type.sym == &symbols::kInferType && !m->args->list.empty()) {
        // do not call CheckType for it to avoid double error emitting
        resulting_type = ResolveExprType(&sf_, scope_, m->args->list.front());
      }
      //

      PerformArgumentsTypeCheck<ast::nodes::FormalPar>(m->args->list, m->args->nrange, callee_file, callee_sym,
                                                       params->list, [](const ast::nodes::FormalPar* param) {
                                                         return param->value == nullptr;
                                                       });

      break;
    }

    case ast::NodeKind::UnaryExpr: {
      const auto* m = n->As<ast::nodes::UnaryExpr>();

      const auto x_type = CheckType(m->x);

      switch (m->op.kind) {
        case core::ast::TokenKind::INC:
        case core::ast::TokenKind::ADD:
        case core::ast::TokenKind::SUB:
        case core::ast::TokenKind::DEC:
        case core::ast::TokenKind::MUL:
        case core::ast::TokenKind::DIV:
          if (x_type && x_type.sym != &builtins::kInteger && x_type.sym != &builtins::kFloat) [[unlikely]] {
            EmitError(TypeError{
                .range = m->x->nrange,
                .message = std::format("integer or float expected, got '{}'", x_type->GetName()),
            });
            break;
          }
          //
          resulting_type = x_type;
          //
          break;
        default:
          break;
      }
      break;
    }
    case ast::NodeKind::BinaryExpr: {
      const auto* m = n->As<ast::nodes::BinaryExpr>();

      auto x_type = CheckType(m->x);
      const auto y_type = CheckType(m->y);

      const auto match_both = [&](const semantic::Symbol* expected_sym) {
        //
        resulting_type = expected_sym;
        //
        MatchTypes(m->x->nrange, x_type, {.sym = expected_sym});
        MatchTypes(m->y->nrange, y_type, {.sym = expected_sym});
      };

      if (x_type) [[likely]] {
        // TODO: 1) maybe just abort early if !x_sym
        //       2) resolve alias only if really needed, it's done by MatchTypes otherwise
        x_type = ResolvePotentiallyAliasedType(x_type.sym);
      }

      switch (m->op.kind) {
        case ast::TokenKind::EQ:
        case ast::TokenKind::NE: {
          match_both(x_type.sym);
          break;
        }
        case ast::TokenKind::CONCAT: {
          if (x_type && !(x_type->Flags() & semantic::SymbolFlags::kBuiltinStringType)) {
            EmitError({
                .range = m->x->nrange,
                .message = std::format("string type expected, got '{}'", x_type->GetName()),
            });
          } else {
            match_both(x_type.sym);
          }
          break;
        }
        case ast::TokenKind::INC:
        case ast::TokenKind::ADD:
        case ast::TokenKind::SUB:
        case ast::TokenKind::DEC:
        case ast::TokenKind::MUL:
        case ast::TokenKind::DIV:
          if (x_type && x_type.sym != &builtins::kInteger && x_type.sym != &builtins::kFloat &&
              !(x_type->Flags() & semantic::SymbolFlags::kEnumMember)) [[unlikely]] {
            EmitError(TypeError{
                .range = m->x->nrange,
                .message = std::format("integer or float expected, got '{}'", x_type->GetName()),
            });
            break;
          }
          match_both(x_type.sym);
          break;
        case ast::TokenKind::SHL:
        case ast::TokenKind::SHR:
        case ast::TokenKind::ROL:
        case ast::TokenKind::ROR:
          //
          resulting_type = x_type;
          //
          if (x_type && !(x_type->Flags() & semantic::SymbolFlags::kBuiltinStringType)) {
            EmitError({
                .range = m->x->nrange,
                .message = std::format("string type expected, got '{}'", x_type->GetName()),
            });
            break;
          }
          MatchTypes(m->y->nrange, y_type, {.sym = &builtins::kInteger});
          break;
        case ast::TokenKind::LT:
        case ast::TokenKind::LE:
        case ast::TokenKind::GT:
        case ast::TokenKind::GE:
          if (x_type && x_type.sym != &builtins::kInteger && x_type.sym != &builtins::kFloat &&
              !(x_type.sym->Flags() & semantic::SymbolFlags::kEnum)) [[unlikely]] {
            EmitError(TypeError{
                .range = m->x->nrange,
                .message = std::format("integer or float expected, got '{}'", x_type->GetName()),
            });
            break;
          }
          //
          resulting_type = &builtins::kBoolean;
          MatchTypes(m->y->nrange, y_type, x_type);
          //
          break;
        case ast::TokenKind::AND:
        case ast::TokenKind::OR:
        case ast::TokenKind::XOR:
        case ast::TokenKind::NOT:
          match_both(&builtins::kBoolean);
          break;
        case ast::TokenKind::DECODE:
          //
          resulting_type = y_type;
          //
          break;
        default:
          break;
      }
      break;
    }

    case ast::NodeKind::CompositeLiteral: {
      const auto* m = n->As<ast::nodes::CompositeLiteral>();
      if (!desired_type) {
        Introspect(m);
        break;
      }

      if (!(desired_type->Flags() & (semantic::SymbolFlags::kStructural | semantic::SymbolFlags::kSubtype))) {
        //
        resulting_type = &symbols::kTypeError;
        //
        break;
      }

      //
      resulting_type = desired_type;
      //

      {
        const auto match_listspec = [&](const ast::nodes::ListSpec* ls, const SourceFile* ls_file) {
          if (ls->length) {
            if (const auto& bounds_opt = detail::GetLengthExprBounds(ls_file, ls->length); bounds_opt) {
              const auto& [min_args, max_args] = *bounds_opt;
              const auto args_count = m->list.size();
              if (args_count < min_args || args_count > max_args) {
                EmitError(TypeError{
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
            const auto* inner_type_sym = ls_file->module->scope->Resolve(ls_file->Text(rs->x));
            for (const auto* arg : m->list) {
              const auto actual_sym = CheckType(arg, inner_type_sym);
              MatchTypes(arg->nrange, actual_sym, {.sym = inner_type_sym});
            }
          }
        };
        if (desired_type->Flags() & semantic::SymbolFlags::kSubtype) {
          const auto* subtype_sym = desired_type;
          const auto* subtype_decl = subtype_sym->Declaration()->As<ast::nodes::SubTypeDecl>();
          if (!subtype_decl) {
            Introspect(m);
            break;
          }
          const auto* subtype_file = ast::utils::SourceFileOf(subtype_decl);

          const auto* f = subtype_decl->field;
          if (f->type->nkind == ast::NodeKind::ListSpec) {
            match_listspec(f->type->As<ast::nodes::ListSpec>(), subtype_file);
          }

          break;
        }
        if (desired_type->Flags() & semantic::SymbolFlags::kList) {
          match_listspec(desired_type->Declaration()->As<ast::nodes::ListSpec>(),
                         ast::utils::SourceFileOf(desired_type->Declaration()));
          break;
        }
      }

      const auto* record_sym = desired_type;
      const auto* record_decl = record_sym->Declaration();
      if (!record_decl) {
        Introspect(m);
        break;
      }
      const auto& fields = *ast::utils::GetStructFields(record_decl);

      const auto* record_file = ast::utils::SourceFileOf(record_decl);

      if (record_sym->Flags() & semantic::SymbolFlags::kUnion) {
        PerformArgumentsTypeCheck<ast::nodes::Field, {.is_union = true}>(m->list, m->nrange, record_file, record_sym,
                                                                         fields, [](const auto*) {
                                                                           return false;
                                                                         });
      } else {
        // TODO: cleanup, calls below differ only in compile-time opts (.allow_missing_fields = true)
        if (m->parent->nkind == ast::NodeKind::TemplateDecl && m->parent->As<ast::nodes::TemplateDecl>()->base) {
          PerformArgumentsTypeCheck<ast::nodes::Field, {.allow_missing_fields = true}>(
              m->list, m->nrange, record_file, record_sym, fields, [](const ast::nodes::Field* field) {
                return !field->optional;
              });
        } else {
          PerformArgumentsTypeCheck<ast::nodes::Field>(m->list, m->nrange, record_file, record_sym, fields,
                                                       [](const ast::nodes::Field* field) {
                                                         return !field->optional;
                                                       });
        }
      }

      break;
    }

    case ast::NodeKind::SelectorExpr: {
      const auto* se = n->As<ast::nodes::SelectorExpr>();

      detail::SelectorExprResolver resolver{detail::SelectorExprResolverOptions{
          .resolve_type =
              [&](const auto*, const auto*, const ast::nodes::Expr* expr) {
                return CheckType(expr);
              },
          .on_non_structural_type =
              [&](const ast::Node* sel, const semantic::Symbol* sym) {
                EmitError(TypeError{
                    .range =
                        ast::Range{
                            .begin = sel->nrange.begin - 1,
                            .end = sel->nrange.end,
                        },
                    .message = std::format("type '{}' is not structural", utils::GetReadableTypeName(sym)),
                });
              },
          .on_unknown_property =
              [&](const ast::nodes::SelectorExpr* se, const semantic::Symbol* sym) {
                EmitError(TypeError{
                    .range = se->sel->nrange,
                    .message = std::format("property '{}' does not exist on type '{}'", sf_.Text(se->sel),
                                           utils::GetReadableTypeName(sym)),
                });
              },
          .on_non_static_property_invalid_access =
              [&](const ast::nodes::SelectorExpr* se, const semantic::Symbol* sym) {
                EmitError(TypeError{
                    .range = se->sel->nrange,
                    .message = std::format("property '{}' of type '{}' is not static", sf_.Text(se->sel),
                                           utils::GetReadableTypeName(sym)),
                });
              },
          .on_static_property_invalid_access =
              [&](const ast::nodes::SelectorExpr* se, const semantic::Symbol* sym) {
                EmitError(TypeError{
                    .range = se->sel->nrange,
                    .message = std::format("property '{}' of type '{}' is not instance-bound", sf_.Text(se->sel),
                                           utils::GetReadableTypeName(sym)),
                });
              },
      }};

      const auto* property_sym = resolver.Resolve(&sf_, scope_, se);
      if (!property_sym) {
        break;
      }
      if (property_sym->Flags() &
          (semantic::SymbolFlags::kFunction | semantic::SymbolFlags::kBuiltin | semantic::SymbolFlags::kAnonymous)) {
        //
        resulting_type = property_sym;
        //
        break;
      }

      const auto* property_file = ast::utils::SourceFileOf(property_sym->Declaration());

      //
      resulting_type = ResolveDeclarationType(property_file, property_file->module->scope,
                                              property_sym->Declaration()->As<ast::nodes::Decl>());
      if (!resulting_type) {
        resulting_type = &symbols::kTypeError;
      }
      //
      break;
    }

    case ast::NodeKind::IndexExpr: {
      const auto* ie = n->As<ast::nodes::IndexExpr>();
      if (!ie->x) [[unlikely]] {
        CheckType(ie->index);
        break;
      }

      detail::IndexExprResolver resolver{detail::IndexExprResolverOptions{
          .resolve_type =
              [&](const auto*, const auto*, const ast::nodes::Expr* expr) {
                return CheckType(expr);
              },
          .on_non_subscriptable_type =
              [&](const ast::Node* x, const semantic::Symbol* sym) {
                EmitError(TypeError{
                    .range = x->nrange,
                    .message = std::format("type '{}' is not subscriptable", utils::GetReadableTypeName(sym)),
                });
              },
          .check_index =
              [&](const ast::nodes::Expr* index) {
                MatchTypes(index->nrange, CheckType(index, nullptr), {.sym = &builtins::kInteger});
              },
      }};

      //
      resulting_type = resolver.Resolve(&sf_, scope_, ie);
      if (!resulting_type) {
        resulting_type = &symbols::kTypeError;
      }
      resulting_type.instance = ie;
      //
      break;
    }

    case ast::NodeKind::Ident: {
      const auto* m = n->As<ast::nodes::Ident>();

      if (desired_type && (desired_type->Flags() & semantic::SymbolFlags::kEnum)) {
        resulting_type = ResolveExprSymbol(&sf_, scope_, m);

        if (resulting_type &&
            (resulting_type->Flags() & (semantic::SymbolFlags::kVariable | semantic::SymbolFlags::kArgument))) {
          const auto* type_sym = ResolveExprType(&sf_, scope_, m);
          if (type_sym && (type_sym->Flags() & semantic::SymbolFlags::kEnum)) {
            resulting_type = type_sym;
          }
        } else {
          resulting_type = desired_type->Members()->Lookup(sf_.Text(m));
          if (resulting_type) {
            // TODO: consider replacing vector with set for module->unresolved
            auto& vec = sf_.module->unresolved;
            vec.erase(std::ranges::remove(vec, m).begin(), vec.end());
          }
        }

        break;
      }

      resulting_type = ResolveExprInstantiatedType(&sf_, scope_, m);
      break;
    }

    case ast::NodeKind::ValueLiteral: {
      const auto* m = n->As<ast::nodes::ValueLiteral>();
      resulting_type.instance = m;
      resulting_type = [](ast::TokenKind kind) -> const semantic::Symbol* {
        switch (kind) {
          case ast::TokenKind::TRUE:
          case ast::TokenKind::FALSE:
            return &builtins::kBoolean;
          case ast::TokenKind::INT:
            return &builtins::kInteger;
          case ast::TokenKind::FLOAT:
            return &builtins::kFloat;
          case ast::TokenKind::BITSTRING:
            return &builtins::kBitstring;
          case ast::TokenKind::HEXSTRING:
            return &builtins::kHexstring;
          case ast::TokenKind::OCTETSTRING:
            return &builtins::kOctetstring;
          case ast::TokenKind::STRING:
            return &builtins::kCharstring;
          case ast::TokenKind::PASS:
          case ast::TokenKind::FAIL:
          case ast::TokenKind::INCONC:
            return &builtins::kVerdictType;
          case ast::TokenKind::MUL:
            return &symbols::kVoidTypekWildcardType;
          case ast::TokenKind::ANY:
            return &symbols::kQuestionType;
          default:
            return nullptr;
        }
      }(m->tok.kind);
    }

    default:
      break;
  }

  return resulting_type;
}

bool BasicTypeChecker::Inspect(const ast::Node* n) {
  switch (n->nkind) {
    case ast::NodeKind::CompositeLiteral: {
      return false;
    }

    case ast::NodeKind::SelectorExpr:
    case ast::NodeKind::IndexExpr:
    case ast::NodeKind::BinaryExpr:
    case ast::NodeKind::CallExpr: {
      CheckType(n);
      return false;
    }

    case ast::NodeKind::ValueDecl: {
      const auto* m = n->As<ast::nodes::ValueDecl>();

      const auto* expected_type = CheckType(m->type).sym;
      EnsureIsAType(expected_type, m->type);

      for (const auto* decl : m->decls) {
        if (!decl->value) [[unlikely]] {
          continue;
        }

        const auto actual_type = CheckType(decl->value, expected_type);
        MatchTypes(decl->nrange, actual_type, {.sym = expected_type, .instance = m});
      }
      return false;
    };

    case ast::NodeKind::FormalPar: {
      const auto* m = n->As<ast::nodes::FormalPar>();
      const auto* expected_type = sf_.module->scope->Resolve(sf_.Text(m->type));
      EnsureIsAType(expected_type, m->type);
      if (const auto* default_value = m->value; default_value) {
        const auto actual_type = CheckType(default_value, expected_type);
        MatchTypes(default_value->nrange, actual_type, {.sym = expected_type, .instance = m});
      }
      return false;
    };

    case ast::NodeKind::SubTypeDecl: {
      const auto* m = n->As<ast::nodes::SubTypeDecl>();
      const auto* f = m->field;

      if (f->type->nkind == ast::NodeKind::ListSpec) {
        const auto* ls = f->type->As<ast::nodes::ListSpec>();
        if (ls->length) {
          const auto& bounds_opt = detail::GetLengthExprBounds(&sf_, ls->length);
          if (bounds_opt) {
            const auto& [min_args, max_args] = *bounds_opt;
            if (min_args > max_args) {
              EmitError(TypeError{
                  .range = ls->length->nrange,
                  .message = "lower bound cannot exceed the highest",
              });
              break;
            }
          }
        }
      }

      if (f->value_constraint && f->type->nkind == ast::NodeKind::RefSpec) {
        const auto* sym = scope_->Resolve(sf_.Text(*f->type->As<ast::nodes::RefSpec>()->x));
        for (const auto* item : f->value_constraint->list) {
          CheckType(item, sym);
        }
      }

      return false;
    }

    case ast::NodeKind::TemplateDecl: {
      const auto* m = n->As<ast::nodes::TemplateDecl>();

      if (m->params) {
        CheckType(m->params);
      }

      const auto* expected_type = sf_.module->scope->Resolve(sf_.Text(m->type));
      const auto actual_type = CheckType(m->value, expected_type);
      MatchTypes(m->value->nrange, actual_type, {.sym = expected_type});

      return false;
    }

    case ast::NodeKind::ReturnStmt: {
      const auto* m = n->As<ast::nodes::ReturnStmt>();

      const auto* fdecl = ast::utils::GetPredecessor<ast::nodes::FuncDecl>(m);

      if (!fdecl) {
        // TODO: maybe do something with it - we somehow have return outside of a function! (really?)
        break;
      }

      if (!fdecl->ret) {
        if (m->result) {
          EmitError(TypeError{
              .range = m->result->nrange,
              .message = "void function should not return a value",
          });
        }
        break;
      }
      if (!m->result) {
        EmitError(TypeError{
            .range = m->nrange,
            .message = "return value expected",
        });
        break;
      }

      const auto* expected_type = sf_.module->scope->Resolve(sf_.Text(fdecl->ret->type));
      const auto actual_type = CheckType(m->result, expected_type);
      MatchTypes(m->result->nrange, actual_type, {.sym = expected_type, .instance = fdecl->ret});

      return false;
    }

    case ast::NodeKind::AssignmentExpr: {
      const auto* m = n->As<ast::nodes::AssignmentExpr>();

      if (m->parent->nkind == ast::NodeKind::CompositeLiteral) {
        Visit(m->value);
        return false;
      }

      const auto expected_type = CheckType(m->property);
      const auto actual_type = CheckType(m->value, expected_type.sym);
      MatchTypes(m->value->nrange, actual_type, expected_type);

      return false;
    }

    case ast::NodeKind::SelectStmt: {
      const auto* m = n->As<ast::nodes::SelectStmt>();
      if (!m->is_union) {
        return true;
      }

      const auto tag_type = CheckType(m->tag);
      if (!tag_type || !(tag_type->Flags() & semantic::SymbolFlags::kUnion)) {
        EmitError(TypeError{
            .range = m->tag->nrange,
            .message = "union type expected",
        });
        return true;
      }

      for (const auto* clause : m->clauses) {
        for (const auto* cond : clause->cond) {
          const auto property = sf_.Text(cond);
          if (!tag_type->Members()->Has(property)) {
            EmitError(TypeError{
                .range = cond->nrange,
                .message = std::format("property '{}' does not exist on type '{}'", property,
                                       utils::GetReadableTypeName(tag_type.sym)),
            });
          }
        }
        Inspect(clause->body);
      }

      return false;
    }

    case ast::NodeKind::EnumTypeDecl: {
      return false;
    }

    default:
      break;
  }

  return true;
}

void PerformTypeCheck(SourceFile& sf) {
  BasicTypeChecker(sf).Check();
}

}  // namespace checker
}  // namespace vanadium::core
