#include "vanadium/core/TypeChecker.h"

#include <algorithm>
#include <cstdlib>
#include <format>
#include <string_view>
#include <type_traits>
#include <utility>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/utils/ASTUtils.h>

#include "vanadium/core/Builtins.h"
#include "vanadium/core/BuiltinsSuperbases.h"
#include "vanadium/core/Program.h"
#include "vanadium/core/Semantic.h"
#include "vanadium/core/utils/ScopedNodeVisitor.h"
#include "vanadium/core/utils/SemanticUtils.h"

namespace vanadium::core {
namespace checker {

namespace symbols {
const semantic::Symbol kTypeError{"<error-type>", nullptr, semantic::SymbolFlags::kBuiltinType};

const semantic::Symbol kVoidType{"<void>", nullptr, semantic::SymbolFlags::kBuiltinType};

const semantic::Symbol kInferType{
    "<infer>", nullptr,
    semantic::SymbolFlags::Value(semantic::SymbolFlags::kBuiltinType | semantic::SymbolFlags::kAnonymous)};
const semantic::Symbol kVarargsType{
    "<varargs>", nullptr,
    semantic::SymbolFlags::Value(semantic::SymbolFlags::kBuiltinType | semantic::SymbolFlags::kAnonymous)};
const semantic::Symbol kAltstepType{
    "<altstep>", nullptr,
    semantic::SymbolFlags::Value(semantic::SymbolFlags::kBuiltinType | semantic::SymbolFlags::kAnonymous)};

const semantic::Symbol kTemplateWildcardType{
    "<*>", nullptr,
    semantic::SymbolFlags::Value(semantic::SymbolFlags::kBuiltinType | semantic::SymbolFlags::kTemplateSpec)};
const semantic::Symbol kTemplateOptionalType{
    "<?>", nullptr,
    semantic::SymbolFlags::Value(semantic::SymbolFlags::kBuiltinType | semantic::SymbolFlags::kTemplateSpec)};
const semantic::Symbol kTemplateOmitType{
    "<omit>", nullptr,
    semantic::SymbolFlags::Value(semantic::SymbolFlags::kBuiltinType | semantic::SymbolFlags::kTemplateSpec)};
const semantic::Symbol kTemplateNotUsedType{
    "<->", nullptr,
    semantic::SymbolFlags::Value(semantic::SymbolFlags::kBuiltinType | semantic::SymbolFlags::kTemplateSpec)};
}  // namespace symbols

[[nodiscard]] inline TemplateRestrictionKind ParseTemplateRestriction(const ast::nodes::RestrictionSpec* spec) {
  if (!spec) {
    return TemplateRestrictionKind::kNone;
  }
  switch (spec->type.kind) {
    case ast::TokenKind::OMIT:
      return TemplateRestrictionKind::kOmit;
    case ast::TokenKind::VALUE:
      return TemplateRestrictionKind::kValue;
    case ast::TokenKind::PRESENT:
      return TemplateRestrictionKind::kPresent;
    default:
      return TemplateRestrictionKind::kRegular;
  }
}

namespace utils {
const ast::nodes::FormalPars* ResolveCallableParams(const SourceFile* file, const semantic::Scope* scope,
                                                    const ast::nodes::ParenExpr* pe) {
  if (pe->parent->nkind != ast::NodeKind::CallExpr) {
    // such cases should be checked, I don't have code samples though
    return nullptr;
  }

  const auto* ce = pe->parent->As<ast::nodes::CallExpr>();
  const auto fun_sym = ResolveExprType(file, scope, ce->fun);
  if (!fun_sym || !(fun_sym->Flags() & (semantic::SymbolFlags::kFunction | semantic::SymbolFlags::kTemplate))) {
    return nullptr;
  }

  return ast::utils::GetCallableDeclParams(fun_sym->Declaration()->As<ast::nodes::Decl>());
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
concept ErrorEmitterFn = std::invocable<F, TypeError&&>;

template <typename F>
concept ErrorEmitterProviderFn =
    requires(F&& with_error_emitter) { with_error_emitter([](ErrorEmitterFn auto emit_error) {}); };

template <typename F>
concept InstantiatedTypeResolverFn =
    std::is_invocable_r_v<InstantiatedType, F, const SourceFile*, const semantic::Scope*, const ast::nodes::Expr*>;

template <InstantiatedTypeResolverFn InstantiatedTypeResolver, typename OnNonStructuralType, typename OnUnknownProperty,
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

  InstantiatedType Resolve(const SourceFile* sf, const semantic::Scope* scope, const ast::nodes::SelectorExpr* se) {
    sf_ = sf;
    scope_ = scope;
    se_tgt_ = se;

    const auto* sym = ResolveSelectorExpr(se);
    return InstantiatedType{
        .sym = sym,
        .restriction = restriction_,
        .is_instance = !mode_static_,
    };
  }

 private:
  TemplateRestrictionKind restriction_;
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
      restriction_ = type.restriction;
      mode_static_ = !type.is_instance;

      if (x_sym) {
        if (const auto* superbase = builtins::GetSuperbase(x_sym); superbase) {
          x_sym = superbase;
        } else {
          x_sym = ResolveTerminalType(x_sym);
        }
      }
    }

    if (!x_sym || x_sym == &symbols::kTypeError) {
      return nullptr;
    }

    if (x_sym == &builtins::kAnytype) {
      // TODO: if we get anytype, resolve to any visible type
      return nullptr;
    }

    if (x_sym->Flags() & semantic::SymbolFlags::kTemplate) {
      mode_static_ = false;
      const auto* tdecl = x_sym->Declaration();
      const auto* tfile = ast::utils::SourceFileOf(tdecl);
      x_sym = ResolveCallableReturnType(tfile, tdecl->As<ast::nodes::Decl>()).sym;
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
    if (!property_sym || (property_sym->Flags() & semantic::SymbolFlags::kThis)) {
      // TODO: remove file lookup from contract
      options_.on_unknown_property(se, x_sym);
      return nullptr;
    }

    if (mode_static_ && !(x_sym->Flags() & semantic::SymbolFlags::kImportedModule) &&
        !(property_sym->Flags() & semantic::SymbolFlags::kVisibilityStatic)) [[unlikely]] {
      if (x_sym->Flags() & semantic::SymbolFlags::kStructural) [[likely]] {
        // property_sym.Flags() & semantic::SymbolFlags::kField === true
        const auto* fnode = property_sym->Declaration()->As<ast::nodes::Field>();
        if (fnode->type->nkind == ast::NodeKind::RefSpec) {
          const auto* fnode_file = ast::utils::SourceFileOf(fnode);
          return ResolveTypeSpecSymbol(fnode_file, fnode->type);
        }
        return x_sym->Members()->LookupShadow(property_name);
      }
      options_.on_non_static_property_invalid_access(se, x_sym);
      return nullptr;
    } else if (!mode_static_ && (property_sym->Flags() & semantic::SymbolFlags::kVisibilityStatic)) [[unlikely]] {
      // TODO: figure out how to forward static-flag and uncomment this
      // options_.on_static_property_invalid_access(se, x_sym);
      // return nullptr;
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
    return ResolveDeclarationType(property_file, property_decl->As<ast::nodes::Decl>()).sym;
  }

  const SourceFile* sf_;
  const semantic::Scope* scope_;
  const ast::nodes::SelectorExpr* se_tgt_;
  Options options_;
};
InstantiatedType ResolveSelectorExprSymbol(const SourceFile* sf, const semantic::Scope* scope,
                                           const ast::nodes::SelectorExpr* se) {
  SelectorExprResolver resolver{SelectorExprResolverOptions{
      .resolve_type = ResolveExprType,
      .on_non_structural_type = [](const auto*, auto) {},
      .on_unknown_property = [](const auto*, auto) {},
      .on_non_static_property_invalid_access = [](const auto*, auto) {},
      .on_static_property_invalid_access = [](const auto*, auto) {},
  }};
  return resolver.Resolve(sf, scope, se);
}

template <InstantiatedTypeResolverFn InstantiatedTypeResolver, std::invocable<const ast::nodes::Expr*> IndexChecker,
          ErrorEmitterProviderFn ErrorEmitterProvider>
struct IndexExprResolverOptions {
  InstantiatedTypeResolver resolve_type;
  IndexChecker check_index;
  ErrorEmitterProvider with_error_emitter;
};

template <typename Options>
class IndexExprResolver {
 public:
  IndexExprResolver(Options options) : options_(std::move(options)) {}

  InstantiatedType Resolve(const SourceFile* sf, const semantic::Scope* scope, const ast::nodes::IndexExpr* ie) {
    sf_ = sf;
    scope_ = scope;

    const auto* sym = ResolveIndexExpr(ie);
    return InstantiatedType{
        .sym = sym,
        .restriction = head_type_.restriction,
        .is_instance = !mode_static_,
        .depth = static_cast<std::uint32_t>(depth_),
    };
  }

 private:
  std::size_t depth_{0};
  InstantiatedType head_type_;
  bool mode_static_{false};
  const semantic::Symbol* ResolveIndexExpr(const ast::nodes::IndexExpr* ie) {
    const semantic::Symbol* x_sym{nullptr};
    if (ie->x->nkind == ast::NodeKind::IndexExpr) {
      x_sym = ResolveIndexExpr(ie->x->As<ast::nodes::IndexExpr>());
      if (depth_ > 0) [[unlikely]] {
        --depth_;
        return x_sym;
      }
    } else {
      head_type_ = options_.resolve_type(sf_, scope_, ie->x);
      x_sym = head_type_.sym;

      if (!x_sym) {
        options_.check_index(ie->index);
        // ReportNonSubscriptableType(
        //     [&] {
        //       const auto* tgt_err_node = ie->x;
        //       if (tgt_err_node->nkind == ast::NodeKind::SelectorExpr) {
        //         tgt_err_node = tgt_err_node->As<ast::nodes::SelectorExpr>()->sel;
        //       }
        //       return tgt_err_node;
        //     }(),
        //     &symbols::kTypeError);
        return nullptr;
      }

      if (x_sym == &symbols::kTypeError) {
        options_.check_index(ie->index);
        return nullptr;
      }

      const bool is_inferrence_expr = "-" == sf_->Text(ie->index);
      if (!head_type_.is_instance) {
        // RoTypeId[-]
        if (is_inferrence_expr) {
          if (!(x_sym->Flags() & semantic::SymbolFlags::kList)) {
            ReportNonSubscriptableType(ie->x, x_sym);
            return nullptr;
          }
          mode_static_ = true;
          return ResolveListElementType(x_sym);
        }

        options_.check_index(ie->index);
        ReportNonSubscriptableType(ie->x, x_sym);
        return nullptr;
      }

      if (is_inferrence_expr) {
        options_.with_error_emitter([&](ErrorEmitterFn auto emit_error) {
          emit_error(TypeError{
              .range = ie->x->nrange,
              .message = std::format("'{}' is not a type reference", sf_->Text(ie->x)),
          });
        });
        return nullptr;
      }

      options_.check_index(ie->index);

      if (head_type_.depth != 0) {
        depth_ = head_type_.depth - 1;
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
      x_sym = ResolveExprType(tf, tf->module->scope, td->type).sym;
      if (!x_sym) {
        return nullptr;
      }
    }

    if (!(x_sym->Flags() & semantic::SymbolFlags::kList)) {
      ReportNonSubscriptableType(ie->x, x_sym);
      return nullptr;
    }

    return ResolveListElementType(x_sym);
  }

  void ReportNonSubscriptableType(const ast::Node* x, const semantic::Symbol* sym) {
    options_.with_error_emitter([&](ErrorEmitterFn auto emit_error) {
      emit_error(TypeError{
          .range = x->nrange,
          .message = std::format("type '{}' is not subscriptable", semantic::utils::GetReadableTypeName(sym)),
      });
    });
  }

  const SourceFile* sf_;
  const semantic::Scope* scope_;
  Options options_;
};
InstantiatedType ResolveIndexExprType(const SourceFile* sf, const semantic::Scope* scope,
                                      const ast::nodes::IndexExpr* se) {
  IndexExprResolver resolver{IndexExprResolverOptions{
      .resolve_type = ResolveExprType,
      .check_index = [](const auto*) {},
      .with_error_emitter = [](const auto&) {},
  }};
  return resolver.Resolve(sf, scope, se);
}

}  // namespace detail

const semantic::Symbol* ResolveAliasedType(const semantic::Symbol* sym) {
  const ast::Node* decl = sym->Declaration()->As<ast::nodes::SubTypeDecl>()->field->type;
  if (decl->nkind == ast::NodeKind::RefSpec) {
    const auto* file = ast::utils::SourceFileOf(decl);
    sym = ResolveExprSymbol(file, file->module->scope, decl->As<ast::nodes::RefSpec>()->x).sym;
    if (!sym) [[unlikely]] {
      return nullptr;
    }
    decl = sym->Declaration();
    if (decl && decl->nkind == ast::NodeKind::SubTypeDecl) {
      return ResolveAliasedType(sym);
    }
  }

  return sym;
}
}  // namespace

const semantic::Symbol* ResolveTerminalType(const semantic::Symbol* sym) {
  if (!(sym->Flags() & semantic::SymbolFlags::kSubtype)) [[likely]] {
    return sym;
  }
  return ResolveAliasedType(sym);
}

namespace {
const ast::nodes::ListSpec* ExtractListSpecNode(const semantic::Symbol* sym) {
  const auto* decl = sym->Declaration();
  switch (decl->nkind) {
    case ast::NodeKind::Field:
      return decl->As<ast::nodes::Field>()->type->As<ast::nodes::ListSpec>();
    case ast::NodeKind::ListSpec:
      return decl->As<ast::nodes::ListSpec>();
    default:
      return nullptr;
  }
}
}  // namespace
const semantic::Symbol* ResolveListElementType(const semantic::Symbol* sym) {
  // assert sym.flags & kList
  if (sym->Members()) {
    if (const auto* isym = sym->Members()->LookupShadow(""); isym) {
      return isym;
    }
  }

  const ast::Node* decl = ExtractListSpecNode(sym);
  if (decl) {
    const auto* decl_file = ast::utils::SourceFileOf(decl);
    sym = ResolveTypeSpecSymbol(decl_file, decl->As<ast::nodes::ListSpec>()->elemtype);
    return sym;
  }
  return nullptr;
}

namespace ext {
InstantiatedType ResolveAssignmentTarget(const SourceFile* file, const semantic::Scope* scope,
                                         const ast::nodes::AssignmentExpr* n) {
  const auto* property = n->property;
  const auto property_name = file->Text(property);

  switch (n->parent->nkind) {
    case ast::NodeKind::ParenExpr: {
      const auto* m = n->parent->As<ast::nodes::ParenExpr>();

      const auto* ce = m->parent->As<ast::nodes::CallExpr>();
      const auto callee_sym = ResolveExprSymbol(file, scope, ce->fun);
      if (!callee_sym ||
          !(callee_sym->Flags() & (core::semantic::SymbolFlags::kFunction | semantic::SymbolFlags::kTemplate))) {
        return InstantiatedType::None();
      }

      return {.sym = callee_sym->OriginatedScope()->ResolveDirect(property_name)};
    }
    case ast::NodeKind::CompositeLiteral: {
      const auto* m = n->parent->As<ast::nodes::CompositeLiteral>();
      auto cl_type = ext::DeduceCompositeLiteralType(file, scope, m);
      if (!cl_type) {
        return InstantiatedType::None();
      }
      cl_type.sym = ResolveTerminalType(cl_type.sym);
      if (!(cl_type->Flags() & semantic::SymbolFlags::kStructural)) {
        return InstantiatedType::None();
      }
      return cl_type.Derive(cl_type->Members()->Lookup(property_name));
    }
    default: {
      return ResolveExprSymbol(file, scope, property);
    }
  }
}
InstantiatedType DeduceCompositeLiteralType(const SourceFile* file, const semantic::Scope* scope,
                                            const ast::nodes::CompositeLiteral* n,
                                            const InstantiatedType& parent_hint) {
  switch (n->parent->nkind) {
    case ast::NodeKind::CompositeLiteral: {
      const auto* cl = n->parent->As<ast::nodes::CompositeLiteral>();

      const auto parent_type = parent_hint ? parent_hint : DeduceCompositeLiteralType(file, scope, cl);
      if (!parent_type) {
        return InstantiatedType::None();
      }

      if (parent_type.depth > 0) {
        auto type{parent_type};
        --type.depth;
        return type;
      }
      if (parent_type->Flags() & semantic::SymbolFlags::kList) {
        return parent_type.Derive(ResolveListElementType(parent_type.sym));
      }

      const auto* decl = parent_type->Declaration();
      if (!decl) [[unlikely]] {  // anytype, etc.
        return InstantiatedType::None();
      }
      if (decl->nkind != ast::NodeKind::StructTypeDecl && decl->nkind != ast::NodeKind::StructSpec) {
        return InstantiatedType::None();
      }

      const auto& fields = *ast::utils::GetStructFields(decl);

      const auto param_index = std::ranges::find(cl->list, n) - cl->list.begin();
      if (param_index >= std::ssize(fields)) {
        return InstantiatedType::None();
      }

      const auto* field = fields[param_index];
      const auto* decl_file = ast::utils::SourceFileOf(decl);

      if (field->type->nkind != ast::NodeKind::RefSpec && field->name) {
        return parent_type.Derive(parent_type->Members()->LookupShadow(decl_file->Text(*field->name)));
      }

      return parent_type.Derive(ResolveTypeSpecSymbol(decl_file, field->type));
    }
    case ast::NodeKind::AssignmentExpr: {
      const auto* ae = n->parent->As<ast::nodes::AssignmentExpr>();
      const auto* property = ae->property;
      const auto property_name = file->Text(property);
      switch (ae->parent->nkind) {
        case ast::NodeKind::CompositeLiteral: {
          const auto* cl = ae->parent->As<ast::nodes::CompositeLiteral>();
          auto cl_sym = parent_hint ? parent_hint : DeduceCompositeLiteralType(file, scope, cl);
          if (!cl_sym) {
            return InstantiatedType::None();
          }

          if (cl_sym->Flags() & semantic::SymbolFlags::kSubtype) {
            cl_sym.sym = ResolveAliasedType(cl_sym.sym);
          }

          if (cl_sym->Flags() & semantic::SymbolFlags::kList) {
            return cl_sym.Derive(ResolveListElementType(cl_sym.sym));
          }

          if (!(cl_sym->Flags() & semantic::SymbolFlags::kStructural)) {
            return InstantiatedType::None();
          }

          const auto* property_sym = cl_sym->Members()->Lookup(property_name);
          if (!property_sym) {
            return InstantiatedType::None();
          }
          const auto* property_file = ast::utils::SourceFileOf(property_sym->Declaration());

          return ResolveDeclarationType(property_file, property_sym->Declaration()->As<ast::nodes::Decl>());
        }
        case ast::NodeKind::ParenExpr: {
          const auto* pe = ae->parent->As<ast::nodes::ParenExpr>();
          const auto* params = utils::ResolveCallableParams(file, scope, pe);
          if (!params) {
            return InstantiatedType::None();
          }

          const auto* params_file = ast::utils::SourceFileOf(params);

          const auto it = std::ranges::find_if(params->list, [&](const ast::nodes::FormalPar* par) {
            return par->name && params_file->Text(*par->name) == property_name;
          });
          if (it == params->list.end()) {
            return InstantiatedType::None();
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
      return InstantiatedType::None();
    }
    case ast::NodeKind::ParenExpr: {
      const auto* pe = n->parent->As<ast::nodes::ParenExpr>();
      const auto* params = utils::ResolveCallableParams(file, scope, pe);
      if (!params) {
        return InstantiatedType::None();
      }

      const auto param_index = std::ranges::find(pe->list, n) - pe->list.begin();
      if (param_index >= std::ssize(params->list)) {
        return InstantiatedType::None();
      }

      const auto* param = params->list[param_index];
      const auto* params_file = ast::utils::SourceFileOf(params);

      return ResolveExprType(params_file, params_file->module->scope, param->type);
    }
    case ast::NodeKind::Declarator:
    case ast::NodeKind::FormalPar: {
      // TODO: remove duplication
      auto restype = ResolveDeclarationType(file, n->parent);
      if (restype && (restype->Flags() & semantic::SymbolFlags::kSubtype)) {
        restype.sym = ResolveAliasedType(restype.sym);
      }
      return restype;
    }
    case ast::NodeKind::TemplateDecl: {
      // TODO: remove duplication
      auto restype = ResolveCallableReturnType(file, n->parent->As<ast::nodes::TemplateDecl>());
      if (restype && (restype->Flags() & semantic::SymbolFlags::kSubtype)) {
        restype.sym = ResolveAliasedType(restype.sym);
      }
      return restype;
    }
    case ast::NodeKind::ReturnStmt: {
      const auto* rs = n->parent->As<ast::nodes::ReturnSpec>();
      const auto* decl = ast::utils::GetPredecessor<ast::nodes::FuncDecl>(rs);
      // TODO: remove duplication
      auto restype = ResolveCallableReturnType(file, decl);
      if (restype && (restype->Flags() & semantic::SymbolFlags::kSubtype)) {
        restype.sym = ResolveAliasedType(restype.sym);
      }
      return restype;
    }
    default:
      return InstantiatedType::None();
  }
}

InstantiatedType DeduceExpectedType(const SourceFile* file, const semantic::Scope* scope, const ast::Node* n) {
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

      if ((ae->property->nkind == ast::NodeKind::IndexExpr) && (ae->parent->nkind == ast::NodeKind::CompositeLiteral)) {
        const auto cl_type = DeduceCompositeLiteralType(file, scope, ae->parent->As<ast::nodes::CompositeLiteral>());
        if (!(cl_type->Flags() & semantic::SymbolFlags::kList)) {
          return InstantiatedType::None();
        }
        return cl_type.Derive(ResolveListElementType(cl_type.sym));
      }

      const auto decl_sym = ResolveAssignmentTarget(file, scope, ae);
      if (!decl_sym) {
        break;
      }

      const auto* decl = decl_sym->Declaration()->As<ast::nodes::Decl>();
      if (!decl) [[unlikely]] {
        break;
      }
      const auto* decl_file = ast::utils::SourceFileOf(decl);

      return ResolveDeclarationType(decl_file, decl);
    }
    case ast::NodeKind::BinaryExpr: {
      const auto* be = parent->As<ast::nodes::BinaryExpr>();
      if (n == be->y) [[likely]] {
        return ResolveExprType(file, scope, be->x);
      }
      /* (n == be->x) */
      return InstantiatedType::None();
    }
    case ast::NodeKind::ParenExpr: {
      const auto* pe = parent->As<ast::nodes::ParenExpr>();

      const auto* params = utils::ResolveCallableParams(file, file->module->scope, pe);
      if (!params) {
        break;
      }

      const auto idx = std::ranges::find(pe->list, n) - pe->list.begin();
      if (idx >= std::ssize(params->list)) {
        return InstantiatedType::None();
      }

      const auto* params_file = ast::utils::SourceFileOf(params);
      const auto* params_scope = params_file->module->scope;

      const auto type = ResolveExprType(params_file, params_scope, params->list[idx]->type);
      if (type.sym == &symbols::kInferType && n != pe->list.back()) [[unlikely]] {
        return ResolveExprType(file, scope, pe->list.back());
      }
      return type;
    }
    case ast::NodeKind::CompositeLiteral: {
      const auto* cl = parent->As<ast::nodes::CompositeLiteral>();

      const auto type = DeduceCompositeLiteralType(file, scope, cl);
      if (!type) {
        return InstantiatedType::None();
      }
      if (!(type->Flags() & semantic::SymbolFlags::kStructural)) {
        if (type->Flags() & semantic::SymbolFlags::kList) {
          const auto* ldecl = type->Declaration();
          if (ldecl->nkind == ast::NodeKind::Field) {  // from SubTypeDecl
            ldecl = ldecl->As<ast::nodes::Field>()->type;
          }
          const auto* ldecl_file = ast::utils::SourceFileOf(ldecl);
          switch (ldecl->nkind) {
            case ast::NodeKind::ListSpec:
              return type.Derive(ResolveTypeSpecSymbol(ldecl_file, ldecl->As<ast::nodes::ListSpec>()->elemtype));
            default:
              return InstantiatedType::None();
          }
        }
        // should not be generally possible
        return type;
      }
      const auto* decl = type->Declaration();

      const auto* fields_ptr = ast::utils::GetStructFields(decl);
      if (!fields_ptr) {
        break;
      }
      const auto& fields = *fields_ptr;

      const auto idx = std::ranges::find(cl->list, n) - cl->list.begin();
      if (idx >= std::ssize(fields)) {
        return InstantiatedType::None();
      }

      const auto* fields_file = ast::utils::SourceFileOf(decl);
      return type.Derive(ResolveTypeSpecSymbol(fields_file, fields[idx]->type));
    }
    case ast::NodeKind::TemplateDecl: {
      const auto* m = parent->As<ast::nodes::TemplateDecl>();
      if (n != m->value) {
        break;
      }
      return ResolveExprType(file, file->module->scope, m->type);
    }
    case ast::NodeKind::CaseClause: {
      const auto* m = parent->As<ast::nodes::CaseClause>();
      const auto* ss = m->parent->As<ast::nodes::SelectStmt>();

      const core::semantic::Scope* scope = semantic::utils::FindScope(file->module->scope, parent);
      return ResolveExprType(file, scope, ss->tag);
    }
    case ast::NodeKind::ReturnStmt: {
      const auto* m = n->As<ast::nodes::ReturnStmt>();
      const auto* fdecl = ast::utils::GetPredecessor<ast::nodes::FuncDecl>(m);
      if (!fdecl || !fdecl->ret) {
        return InstantiatedType::None();
      }
      return ResolveCallableReturnType(file, fdecl);
    }
    default:
      break;
  }
  return InstantiatedType::None();
}

}  // namespace ext

namespace {
const semantic::Symbol* TryResolveExprSymbolViaHierarchy(const SourceFile* file, const semantic::Scope*,
                                                         const ast::nodes::Expr* expr) {
  switch (expr->parent->nkind) {
    case ast::NodeKind::CaseClause: {
      const auto* m = expr->parent->As<ast::nodes::CaseClause>();
      const auto* ss = m->parent->As<ast::nodes::SelectStmt>();

      const core::semantic::Scope* scope = semantic::utils::FindScope(file->module->scope, expr);
      const auto tag_sym = ResolveExprType(file, scope, ss->tag);
      if (!tag_sym) {
        return nullptr;
      }

      if (ss->is_union) {
        if (!tag_sym || !(tag_sym->Flags() & core::semantic::SymbolFlags::kUnion)) {
          return nullptr;
        }
        return tag_sym->Members()->Lookup(file->Text(expr));
      }
      if (tag_sym->Flags() & semantic::SymbolFlags::kEnum) {
        return tag_sym->Members()->Lookup(file->Text(expr));
      }

      return nullptr;
    }
    default:
      return nullptr;
  }
}

const semantic::Symbol* ResolveStructTypeDeclSymbol(const SourceFile* file, const ast::nodes::StructTypeDecl* decl) {
  if (!decl->name) [[unlikely]] {
    return nullptr;
  }
  return file->module->scope->ResolveDirect(file->Text(*decl->name));
}
}  // namespace

const semantic::Symbol* ResolveTypeSpecSymbol(const SourceFile* file, const ast::nodes::TypeSpec* spec) {
  switch (spec->nkind) {
    case ast::NodeKind::StructTypeDecl: {
      return ResolveStructTypeDeclSymbol(file, spec->As<ast::nodes::StructTypeDecl>());
    }
    case ast::NodeKind::RefSpec: {
      const auto* m = spec->As<ast::nodes::RefSpec>();
      return ResolveExprSymbol(file, file->module->scope, m->x).sym;
    }
    case ast::NodeKind::ListSpec:
    case ast::NodeKind::StructSpec:
    case ast::NodeKind::EnumSpec: {
      const auto* parent = spec->parent;
      switch (parent->nkind) {
        case ast::NodeKind::ListSpec: {
          const auto* ls_sym = ResolveTypeSpecSymbol(file, parent->As<ast::nodes::ListSpec>());
          if (!ls_sym) {
            return nullptr;
          }
          return ResolveListElementType(ls_sym);
        }
        case ast::NodeKind::Field: {
          const auto* owner = parent->As<ast::nodes::Field>();
          if (!owner->name) {
            return nullptr;
          }

          if (owner->parent->nkind == ast::NodeKind::SubTypeDecl) {
            return file->module->scope->ResolveDirect(file->Text(*owner->name));
          }

          const auto* containing_sym = ResolveTypeSpecSymbol(file, owner->parent->As<ast::nodes::TypeSpec>());
          if (!containing_sym || !(containing_sym->Flags() & core::semantic::SymbolFlags::kStructural)) {
            return nullptr;
          }
          return containing_sym->Members()->LookupShadow(file->Text(*owner->name));
        }
        default:
          break;
      }
      return nullptr;
    }
    default:
      return nullptr;
  }
}

// Expression -> Declaration
InstantiatedType ResolveExprSymbol(const SourceFile* file, const semantic::Scope* scope, const ast::nodes::Expr* expr) {
  switch (expr->nkind) {
    case ast::NodeKind::Ident: {
      const auto* m = expr->As<ast::nodes::Ident>();
      const auto label = file->Text(m);

      const auto* sym = scope->Resolve(label);
      if (!sym) {
        auto expected_type = ext::DeduceExpectedType(file, scope, expr);
        if (expected_type && expected_type->Flags() & semantic::SymbolFlags::kList) {
          expected_type = ResolveListElementType(expected_type.sym);
        }
        if (expected_type && (expected_type->Flags() & semantic::SymbolFlags::kEnum)) {
          sym = expected_type->Members()->Lookup(label);
        }
      }
      if (!sym) [[unlikely]] {
        return {.sym = TryResolveExprSymbolViaHierarchy(file, scope, expr)};
      }
      return {.sym = sym};
    }
    case ast::NodeKind::CallExpr: {
      const auto* m = expr->As<ast::nodes::CallExpr>();
      const auto* fun = m->fun;
      return ResolveExprSymbol(file, scope, fun);
    }
    case ast::NodeKind::AssignmentExpr: {
      const auto* m = expr->As<ast::nodes::AssignmentExpr>();
      return ResolveExprSymbol(file, scope, m->property);
    }
    case ast::NodeKind::SelectorExpr: {
      const auto* m = expr->As<ast::nodes::SelectorExpr>();
      return detail::ResolveSelectorExprSymbol(file, scope, m);
    }
    case ast::NodeKind::IndexExpr: {
      const auto* m = expr->As<ast::nodes::IndexExpr>();
      return detail::ResolveIndexExprType(file, scope, m);
    }
    default: {
      return {.sym = TryResolveExprSymbolViaHierarchy(file, scope, expr)};
    }
  }
}

// Value Declaration -> Effective Type
InstantiatedType ResolveDeclarationType(const SourceFile* file, const ast::Node* decl) {
  switch (decl->nkind) {
    case ast::NodeKind::Declarator: {
      const auto* m = decl->As<ast::nodes::Declarator>();
      const auto* vd = m->parent->As<ast::nodes::ValueDecl>();
      return InstantiatedType{
          .sym = ResolveExprSymbol(file, file->module->scope, vd->type).sym,
          .restriction = ParseTemplateRestriction(vd->template_restriction),
          .is_instance = true,
          .depth = static_cast<std::uint32_t>(ast::utils::GetArrayDef(m)->size()),
      };
    }
    case ast::NodeKind::FormalPar: {
      const auto* m = decl->As<ast::nodes::FormalPar>();
      return InstantiatedType{
          .sym = ResolveExprSymbol(file, file->module->scope, m->type).sym,
          .restriction = ParseTemplateRestriction(m->restriction),
          .is_instance = true,
          .depth = static_cast<std::uint32_t>(ast::utils::GetArrayDef(m)->size()),
      };
    }
    case ast::NodeKind::Field: {
      const auto* m = decl->As<ast::nodes::Field>();
      return {
          .sym = ResolveTypeSpecSymbol(file, m->type),
          .restriction = m->optional ? TemplateRestrictionKind::kOptionalField : TemplateRestrictionKind::kNone,
          .is_instance = true,
          .depth = static_cast<std::uint32_t>(m->arraydef.size()),
      };
    }
    case ast::NodeKind::StructTypeDecl: {
      return {
          .sym = ResolveStructTypeDeclSymbol(file, decl->As<ast::nodes::StructTypeDecl>()),
          .is_instance = false,
      };
    }
    case ast::NodeKind::ClassTypeDecl: {
      // we could come here only via Resolve("this")->Declaration()
      // use direct module symbol table for speed up
      const auto* m = decl->As<ast::nodes::ClassTypeDecl>();
      return {
          .sym = file->module->scope->ResolveDirect(file->Text(*m->name)),
          .is_instance = true,
      };
    }
    case ast::NodeKind::FuncDecl: {
      const auto* m = decl->As<ast::nodes::FuncDecl>();
      const auto* declfile = ast::utils::SourceFileOf(m);
      return {.sym = declfile->module->scope->ResolveDirect(declfile->Text(*m->name))};
    }
    case ast::NodeKind::TemplateDecl: {
      const auto* m = decl->As<ast::nodes::TemplateDecl>();
      const auto* declfile = ast::utils::SourceFileOf(m);
      return {
          .sym = declfile->module->scope->ResolveDirect(declfile->Text(*m->name)),
          .is_instance = true,
      };
    }
    default: {
      return InstantiatedType::None();
    }
  }
}

// Callable Declaration -> Effective Type
InstantiatedType ResolveCallableReturnType(const SourceFile* file, const ast::nodes::Decl* decl) {
  switch (decl->nkind) {
    case ast::NodeKind::FuncDecl: {
      const auto* m = decl->As<ast::nodes::FuncDecl>();
      const auto* ret = m->ret;
      if (!ret) {
        return {.sym = &symbols::kVoidType};
      }
      return {
          .sym = ResolveExprSymbol(file, file->module->scope, m->ret->type).sym,
          .restriction = ParseTemplateRestriction(ret->restriction),
          .is_instance = true,
      };
    }
    case ast::NodeKind::TemplateDecl: {
      const auto* m = decl->As<ast::nodes::TemplateDecl>();
      return {
          .sym = ResolveExprSymbol(file, file->module->scope, m->type).sym,
          .restriction = ParseTemplateRestriction(m->restriction),
          .is_instance = true,
      };
    }
    case ast::NodeKind::ConstructorDecl: {
      const auto* m = decl->As<ast::nodes::ConstructorDecl>();
      const auto* classdecl = m->parent->parent->As<ast::nodes::ClassTypeDecl>();  // Def->ClassTypeDecl
      return {
          .sym = file->module->scope->ResolveDirect(file->Text(*classdecl->name)),
          .is_instance = true,
      };
    }
    default: {
      return InstantiatedType::None();
    }
  }
}

// Expression -> (Declaration) -> Effective Type
InstantiatedType ResolveExprType(const SourceFile* file, const semantic::Scope* scope, const ast::nodes::Expr* expr) {
  const auto decl_type = ResolveExprSymbol(file, scope, expr);
  if (!decl_type) {
    return InstantiatedType::None();
  }

  if ((decl_type->Flags() &
       (semantic::SymbolFlags::kType | semantic::SymbolFlags::kImportedModule | semantic::SymbolFlags::kEnumMember)) ||
      (expr->nkind != ast::NodeKind::CallExpr && (decl_type->Flags() & semantic::SymbolFlags::kFunction))) {
    return decl_type;
  }

  const auto* decl = decl_type->Declaration()->As<ast::nodes::Decl>();
  const auto* decl_file = ast::utils::SourceFileOf(decl);

  if (expr->nkind == ast::NodeKind::CallExpr) {
    auto itype = ResolveCallableReturnType(decl_file, decl);
    if (itype.sym == &symbols::kInferType) {
      const auto* ce = expr->As<ast::nodes::CallExpr>();
      if (ce->args->list.empty()) {
        return InstantiatedType::None();
      }
      itype.sym = ResolveExprType(file, scope, ce->args->list.back()).sym;
    }
    return itype;
  }

  return ResolveDeclarationType(decl_file, decl);
}

class BasicTypeChecker {
 public:
  explicit BasicTypeChecker(SourceFile& sf)
      : sf_(sf), inspector_(ast::NodeInspector::create<BasicTypeChecker, &BasicTypeChecker::Inspect>(this)) {}

  void Check() {
    if (!sf_.module) [[unlikely]] {
      return;
    }
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

  void MatchTypes(const ast::Range& range, InstantiatedType actual, const InstantiatedType& expected);

  InstantiatedType CheckType(const ast::Node* n, InstantiatedType desired_type = InstantiatedType::None());
  bool Inspect(const ast::Node*);

  void Introspect(const ast::Node* n) {
    n->Accept(inspector_);
  }
  void Visit(const ast::Node* n) {
    if (Inspect(n)) {
      Introspect(n);
    }
  }
  void ScopedVisit(const ast::nodes::BlockStmt* n) {
    // TODO: embed into existing ScopedVisitor somehow

    const semantic::Scope* block_scope{nullptr};
    for (const auto* child_scope : scope_->GetChildren()) {
      if (child_scope->Container()->Contains(n)) {
        block_scope = child_scope;
      }
    }

    if (!block_scope) [[unlikely]] {
      return;
    }

    semantic::InspectScope(
        block_scope,
        [&](const semantic::Scope* scope_under_inspection) {
          scope_ = scope_under_inspection;
        },
        [&](const ast::Node* n) {
          return Inspect(n);
        },
        scope_);
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
  void CheckArguments(std::span<const ast::nodes::Expr* const> args, const ast::Range& args_range,
                      const SourceFile* params_file, const semantic::Symbol* subject_type_sym,
                      std::span<const TParamDescriptorNode* const> params,
                      std::predicate<const TParamDescriptorNode*> auto is_param_required,
                      TemplateRestrictionKind inherited_restriction_kind = TemplateRestrictionKind::kNone);

  const semantic::Scope* scope_;

  SourceFile& sf_;
  const ast::NodeInspector inspector_;
};

void BasicTypeChecker::MatchTypes(const ast::Range& range, InstantiatedType actual, const InstantiatedType& expected) {
  if (!expected || expected.sym == &symbols::kTypeError || expected.sym == &builtins::kAnytype) {
    return;
  }

  if (actual && actual->Flags() & semantic::SymbolFlags::kTemplate) {
    const auto* file = ast::utils::SourceFileOf(actual->Declaration());
    actual.sym = ResolveCallableReturnType(file, actual->Declaration()->As<ast::nodes::Decl>()).sym;
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

  if (actual.depth != expected.depth) {
    EmitError(TypeError{
        .range = range,
        .message = std::format("expected value of array depth {}, got with depth {}", actual.depth, expected.depth),
    });
    return;
  }

  const auto template_spec_providen = bool(actual->Flags() & semantic::SymbolFlags::kTemplateSpec);
  const bool got_template_instead_of_value{
      (expected.restriction == TemplateRestrictionKind::kNone) &&
      (template_spec_providen || (actual.restriction != TemplateRestrictionKind::kNone))  //
  };
  if (got_template_instead_of_value) {
    const bool is_legit{(actual.restriction == TemplateRestrictionKind::kOptionalField) ||
                        ((expected.restriction & TemplateRestrictionKind::kOptionalField) &&
                         (actual.sym == &symbols::kTemplateOmitType))};
    if (!is_legit) {
      EmitError(TypeError{
          .range = range,
          .message = std::format("expected value, got template"),
      });
    }
  }
  if (template_spec_providen) {
    return;
  }

  if (expected->Flags() & semantic::SymbolFlags::kEnum && (actual->Flags() & semantic::SymbolFlags::kEnumMember)) {
    return;
  }

  if (expected.sym == actual.sym) {
    return;
  }

  // TODO: read language spec and maybe make something more precise
  const auto* real_expected_sym = ResolveTerminalType(expected.sym);
  actual.sym = ResolveTerminalType(actual.sym);
  if (real_expected_sym == actual.sym || !real_expected_sym || !actual.sym) {
    return;
  }

  if ((real_expected_sym->Flags() & semantic::SymbolFlags::kList) &&
      (actual.sym->Flags() & semantic::SymbolFlags::kList)) {
    const auto* actual_element_sym = ResolveListElementType(actual.sym);
    const auto* expected_element_sym = ResolveListElementType(real_expected_sym);
    if (actual_element_sym == expected_element_sym) {
      return;
    }
    if (actual_element_sym && expected_element_sym) {
      actual_element_sym = ResolveTerminalType(actual_element_sym);
      expected_element_sym = ResolveTerminalType(expected_element_sym);
      if (actual_element_sym == expected_element_sym) {
        return;
      }
    }
    // TODO: recursive deep check the full hierarchy
  }

  EmitError(TypeError{
      .range = range,
      .message =
          std::format("expected value of type '{}', got '{}'", semantic::utils::GetReadableTypeName(expected.sym),
                      semantic::utils::GetReadableTypeName(actual.sym)),
  });
}

template <ast::IsNode TParamDescriptorNode, BasicTypeChecker::ArgumentsTypeCheckOptions Options = {}>
void BasicTypeChecker::CheckArguments(std::span<const ast::nodes::Expr* const> args, const ast::Range& args_range,
                                      const SourceFile* params_file, const semantic::Symbol* subject_type_sym,
                                      std::span<const TParamDescriptorNode* const> params,
                                      std::predicate<const TParamDescriptorNode*> auto is_param_required,
                                      TemplateRestrictionKind inherited_restriction_kind) {
  const auto args_count = args.size();

  const auto minimal_args_cnt = static_cast<std::size_t>(std::ranges::count_if(params, is_param_required));
  const auto maximal_args_cnt = params.size();

  if constexpr (!Options.is_union) {
    if ((!Options.allow_missing_fields && (args_count < minimal_args_cnt)) || args_count > maximal_args_cnt) {
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
    const auto expected_type = [&] -> InstantiatedType {
      if constexpr (std::is_same_v<TParamDescriptorNode, ast::nodes::Field>) {
        auto restype = ResolveDeclarationType(params_file, param);
        restype.restriction |= inherited_restriction_kind;
        return restype;
      } else if constexpr (std::is_same_v<TParamDescriptorNode, ast::nodes::FormalPar>) {
        auto restype = ResolveDeclarationType(params_file, param);
        if (restype.sym == &symbols::kInferType) {
          restype.sym = ResolveExprType(&sf_, scope_, args.back()).sym;
        }
        return restype;
      } else {
        std::unreachable();
      }
    }();

    const auto actual_instance = CheckType(n, expected_type);
    MatchTypes(n->nrange, actual_instance, expected_type);
  };

  bool seen_named_argument{false};
  std::int64_t last_named_argument_idx{0};
  for (std::size_t i = 0; i < args_count; ++i) {
    const auto* argnode = args[i];
    switch (argnode->nkind) {
      case ast::NodeKind::AssignmentExpr: {
        const auto* ae = argnode->As<ast::nodes::AssignmentExpr>();
        seen_named_argument = true;

        if (ae->property->nkind != ast::NodeKind::Ident) [[unlikely]] {
          // most likely it is IndexExpr, e.g. { [0] := value }
          EmitError(TypeError{
              .range = ae->value->nrange,
              .message = std::format("type '{}' is not array-like",
                                     semantic::utils::GetReadableTypeName(params_file, subject_type_sym)),
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
          // looking string up it table seems to be faster than iterating, searching ptr should be a good tradeoff
          const auto idx = std::ranges::find(params, property_sym->Declaration()) - params.begin();
          if (last_named_argument_idx > idx) {
            EmitError(TypeError{
                .range = ae->nrange,
                .message = std::format("argument '{}' is out of order, it should precede '{}'", property_name,
                                       params_file->Text(*params[last_named_argument_idx]->name)),
            });
          }
          last_named_argument_idx = idx;
          check_argument(property_sym->Declaration()->As<TParamDescriptorNode>(), ae->value);
        } else {
          EmitError(TypeError{
              .range = ae->property->nrange,
              .message = [&] -> std::string {
                if constexpr (std::is_same_v<TParamDescriptorNode, ast::nodes::FormalPar>) {
                  return std::format("callable '{}' does not have an argument with name '{}'",
                                     semantic::utils::GetReadableTypeName(params_file, subject_type_sym),
                                     property_name);
                } else {
                  return std::format("property '{}' does not exist on type '{}'", property_name,
                                     semantic::utils::GetReadableTypeName(params_file, subject_type_sym));
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

InstantiatedType BasicTypeChecker::CheckType(const ast::Node* n, InstantiatedType desired_type) {
  InstantiatedType resulting_type{};
  switch (n->nkind) {
    case ast::NodeKind::CallExpr: {
      const auto* m = n->As<ast::nodes::CallExpr>();
      const auto* callee_sym = CheckType(m->fun).sym;
      if (!callee_sym) {
        // error is raised by the semantic analyzer in such case
        Visit(m->args);
        break;
      }

      if (!(callee_sym->Flags() & (semantic::SymbolFlags::kFunction | semantic::SymbolFlags::kTemplate))) {
        const auto* tgt_errnode = m->fun;
        if (tgt_errnode->nkind == ast::NodeKind::SelectorExpr) {
          tgt_errnode = tgt_errnode->As<ast::nodes::SelectorExpr>()->sel;
        }
        EmitError(TypeError{
            .range = tgt_errnode->nrange,
            .message = callee_sym == &symbols::kTypeError
                           ? std::format("'<error-type>.{}' is not callable", sf_.Text(tgt_errnode))
                           : std::format("'{}' is not callable", sf_.Text(tgt_errnode)),
        });
        Visit(m->args);
        break;
      }

      const auto* callee_decl = callee_sym->Declaration()->As<ast::nodes::Decl>();
      const auto* callee_file = ast::utils::SourceFileOf(callee_decl);

      //
      resulting_type = ResolveCallableReturnType(callee_file, callee_decl);
      //

      const ast::nodes::FormalPars* params = ast::utils::GetCallableDeclParams(callee_decl);
      if (!params) [[unlikely]] {
        // this is guaranteed to be possible due to parser leniency
        Visit(m->args);
        break;
      }

      //
      if (resulting_type.sym == &symbols::kInferType && !m->args->list.empty()) {
        // do not call CheckType for it to avoid double error emitting
        resulting_type.sym = ResolveExprType(&sf_, scope_, m->args->list.back()).sym;
      }
      //

      if (params->list.size() == 1 && callee_file->Text(params->list.back()->type) == "__vargs_t") {
        Visit(m->args);
        break;
      }

      CheckArguments<ast::nodes::FormalPar>(m->args->list, m->args->nrange, callee_file, callee_sym, params->list,
                                            [](const ast::nodes::FormalPar* param) {
                                              return param->value == nullptr;
                                            });

      break;
    }

    case ast::NodeKind::ParenExpr: {
      const auto* m = n->As<ast::nodes::ParenExpr>();
      if (m->list.size() == 1) [[likely]] {
        resulting_type = CheckType(m->list.front(), desired_type);
      }
      break;
    }

    case ast::NodeKind::UnaryExpr: {
      const auto* m = n->As<ast::nodes::UnaryExpr>();

      const auto x_type = CheckType(m->x);

      switch (m->op.kind) {
        case ast::TokenKind::INC:
        case ast::TokenKind::ADD:
        case ast::TokenKind::SUB:
        case ast::TokenKind::DEC:
        case ast::TokenKind::MUL:
        case ast::TokenKind::DIV:
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
      const auto y_type = CheckType(m->y, x_type);

      const auto match_both = [&](const semantic::Symbol* expected_sym) {
        //
        resulting_type.sym = expected_sym;
        resulting_type.is_instance = true;
        //
        MatchTypes(m->x->nrange, x_type, {.sym = expected_sym});
        MatchTypes(m->y->nrange, y_type, {.sym = expected_sym});
      };

      if (x_type) [[likely]] {
        if (x_type->Flags() & semantic::SymbolFlags::kTemplate) {
          const auto* tdecl = x_type->Declaration()->As<ast::nodes::TemplateDecl>();
          x_type.sym = ResolveCallableReturnType(ast::utils::SourceFileOf(tdecl), tdecl).sym;
        }

        if (x_type) {
          // TODO: 1) maybe just abort early if !x_sym
          //       2) resolve alias only if really needed, it's done by MatchTypes otherwise
          x_type.sym = ResolveTerminalType(x_type.sym);
        }
      }

      switch (m->op.kind) {
        case ast::TokenKind::COLON: {  // Type:Value
          match_both(x_type.sym);
          resulting_type.restriction = desired_type.restriction;
          break;
        }
        case ast::TokenKind::EQ:
        case ast::TokenKind::NE: {
          match_both(x_type.sym);
          resulting_type.sym = &builtins::kBoolean;
          break;
        }
        case ast::TokenKind::CONCAT: {
          if (x_type && !(x_type->Flags() & (semantic::SymbolFlags::kBuiltinString | semantic::SymbolFlags::kList))) {
            EmitError({
                .range = m->x->nrange,
                .message = std::format("string or list type expected, got '{}'", x_type->GetName()),
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

    case ast::NodeKind::AssignmentExpr: {
      const auto* m = n->As<ast::nodes::AssignmentExpr>();

      if (m->parent->nkind == ast::NodeKind::CompositeLiteral) {
        if (m->property->nkind == ast::NodeKind::IndexExpr) {
          MatchTypes(m->value->nrange, CheckType(m->value, desired_type), desired_type);
        }
        Visit(m->value);
        break;
      }

      const auto expected_type = CheckType(m->property);
      const auto actual_type = CheckType(m->value, expected_type);
      MatchTypes(m->value->nrange, actual_type, expected_type);

      break;
    }

    case ast::NodeKind::CompositeLiteral: {
      const auto* m = n->As<ast::nodes::CompositeLiteral>();
      if (!desired_type) {
        Introspect(m);
        break;
      }

      //
      resulting_type = desired_type;
      //

      desired_type.sym = ResolveTerminalType(desired_type.sym);
      if (!desired_type) {  // todo: this was a quick fix.
        Introspect(m);
        break;
      }

      if (desired_type.depth > 0) {
        for (const auto* arg : m->list) {
          // TODO: unify with identical code for Ro chk below
          if (arg->nkind == ast::NodeKind::AssignmentExpr &&
              arg->As<ast::nodes::AssignmentExpr>()->property->nkind != ast::NodeKind::IndexExpr) [[unlikely]] {
            EmitError({.range = arg->nrange, .message = "list element expected"});
          }
          const InstantiatedType expected_arg_sym{
              .sym = desired_type.sym,
              .restriction = desired_type.restriction,
              .is_instance = true,
              .depth = desired_type.depth - 1,
          };
          const auto actual_sym = CheckType(arg, expected_arg_sym);
          MatchTypes(arg->nrange, actual_sym, expected_arg_sym);
        }
        break;
      }

      if (!(desired_type->Flags() & (semantic::SymbolFlags::kStructural | semantic::SymbolFlags::kList))) {
        //
        resulting_type = &symbols::kTypeError;
        Introspect(m);
        //
        break;
      }

      if (desired_type->Flags() & semantic::SymbolFlags::kList) {
        const auto* ls = ExtractListSpecNode(desired_type.sym);
        const auto* ls_file = ast::utils::SourceFileOf(desired_type->Declaration());

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
                  .message = std::format("elements count is restricted to be in between {} and {}", min_args, max_args),
              });
            }
          }
        }

        const auto* element_type_sym = ResolveListElementType(desired_type.sym);
        for (const auto* arg : m->list) {
          if (arg->nkind == ast::NodeKind::AssignmentExpr &&
              arg->As<ast::nodes::AssignmentExpr>()->property->nkind != ast::NodeKind::IndexExpr) [[unlikely]] {
            EmitError({.range = arg->nrange, .message = "list element expected"});
          }
          const InstantiatedType expected_arg_sym{
              .sym = element_type_sym,
              .restriction = desired_type.restriction,
              .is_instance = true,
          };
          const auto actual_sym = CheckType(arg, expected_arg_sym);
          MatchTypes(arg->nrange, actual_sym, expected_arg_sym);
        }
        break;
      }

      const auto* record_sym = desired_type.sym;
      const auto* record_decl = record_sym->Declaration();
      if (!record_decl) {
        Introspect(m);
        break;
      }
      const auto& fields = *ast::utils::GetStructFields(record_decl);

      const auto* record_file = ast::utils::SourceFileOf(record_decl);

      if (record_sym->Flags() & semantic::SymbolFlags::kUnion) {
        CheckArguments<ast::nodes::Field, {.is_union = true}>(
            m->list, m->nrange, record_file, record_sym, fields,
            [](const auto*) {
              return false;
            },
            desired_type.restriction);
      } else {
        CheckArguments<ast::nodes::Field, {.allow_missing_fields = true}>(
            m->list, m->nrange, record_file, record_sym, fields,
            [](const ast::nodes::Field* field) {
              return !field->optional;
            },
            desired_type.restriction);
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
                    .message = std::format("type '{}' is not structural", semantic::utils::GetReadableTypeName(sym)),
                });
              },
          .on_unknown_property =
              [&](const ast::nodes::SelectorExpr* se, const semantic::Symbol* sym) {
                EmitError(TypeError{
                    .range = se->sel->nrange,
                    .message = std::format("property '{}' does not exist on type '{}'", sf_.Text(se->sel),
                                           semantic::utils::GetReadableTypeName(sym)),
                });
              },
          .on_non_static_property_invalid_access =
              [&](const ast::nodes::SelectorExpr* se, const semantic::Symbol* sym) {
                EmitError(TypeError{
                    .range = se->sel->nrange,
                    .message = std::format("property '{}' of type '{}' is not static", sf_.Text(se->sel),
                                           semantic::utils::GetReadableTypeName(sym)),
                });
              },
          .on_static_property_invalid_access =
              [&](const ast::nodes::SelectorExpr* se, const semantic::Symbol* sym) {
                EmitError(TypeError{
                    .range = se->sel->nrange,
                    .message = std::format("property '{}' of type '{}' is not instance-bound", sf_.Text(se->sel),
                                           semantic::utils::GetReadableTypeName(sym)),
                });
              },
      }};

      const auto resolution = resolver.Resolve(&sf_, scope_, se);
      const auto* property_sym = resolution.sym;
      if (!property_sym) {
        resulting_type = &symbols::kTypeError;
        break;
      }

      if (property_sym->Flags() &
          (semantic::SymbolFlags::kFunction | semantic::SymbolFlags::kType | semantic::SymbolFlags::kAnonymous)) {
        resulting_type = resolution;
        break;
      }

      const auto* property_file = ast::utils::SourceFileOf(property_sym->Declaration());

      //
      resulting_type = ResolveDeclarationType(property_file, property_sym->Declaration()->As<ast::nodes::Decl>());
      if (!resulting_type) {
        resulting_type = &symbols::kTypeError;
      }
      resulting_type.restriction |= resolution.restriction;
      resulting_type.is_instance = resolution.is_instance;
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
          .check_index =
              [&](const ast::nodes::Expr* index) {
                MatchTypes(index->nrange, CheckType(index, {}), {.sym = &builtins::kInteger});
              },
          .with_error_emitter =
              [&](auto accept_ee) {
                accept_ee([&](TypeError&& err) {
                  EmitError(std::move(err));
                });
              },
      }};

      //
      resulting_type = resolver.Resolve(&sf_, scope_, ie);
      if (!resulting_type) {
        resulting_type = &symbols::kTypeError;
      }
      //
      break;
    }

    case ast::NodeKind::FromExpr: {  // TODO: make this more suitable for the "permutation" builtin
      const auto* m = n->As<ast::nodes::FromExpr>();
      resulting_type = CheckType(m->x);
      if (resulting_type && (resulting_type->Flags() & semantic::SymbolFlags::kList)) [[likely]] {
        resulting_type.sym = ResolveListElementType(resulting_type.sym);
      }
      break;
    }

    case ast::NodeKind::Ident: {
      const auto* m = n->As<ast::nodes::Ident>();

      if (desired_type && (desired_type->Flags() & semantic::SymbolFlags::kEnum)) {
        resulting_type = ResolveExprSymbol(&sf_, scope_, m);

        if (resulting_type &&
            (resulting_type->Flags() & (semantic::SymbolFlags::kVariable | semantic::SymbolFlags::kArgument))) {
          const auto type_sym = ResolveExprType(&sf_, scope_, m);
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

      resulting_type = ResolveExprType(&sf_, scope_, m);
      break;
    }

    case ast::NodeKind::ValueLiteral: {
      const auto* m = n->As<ast::nodes::ValueLiteral>();
      resulting_type.is_instance = true;
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
            return &symbols::kTemplateWildcardType;
          case ast::TokenKind::ANY:
            return &symbols::kTemplateOptionalType;
          case ast::TokenKind::OMIT:
            return &symbols::kTemplateOmitType;
          case ast::TokenKind::SUB:
            return &symbols::kTemplateNotUsedType;
          default:
            return nullptr;
        }
      }(m->tok.kind);
      break;
    }

    default:
      break;
  }

  return resulting_type;
}

bool BasicTypeChecker::Inspect(const ast::Node* n) {
  switch (n->nkind) {
    case ast::NodeKind::SelectorExpr:
    case ast::NodeKind::IndexExpr:
    case ast::NodeKind::BinaryExpr:
    case ast::NodeKind::AssignmentExpr:
    case ast::NodeKind::CallExpr: {
      CheckType(n);
      return false;
    }

    case ast::NodeKind::ValueDecl: {
      const auto* m = n->As<ast::nodes::ValueDecl>();

      const auto expected_type = CheckType(m->type);
      EnsureIsAType(expected_type.sym, m->type);

      for (const auto* declarator : m->decls) {
        if (!declarator->value) [[unlikely]] {
          continue;
        }

        const InstantiatedType expected_decl_type{
            .sym = expected_type.sym,
            .restriction = ParseTemplateRestriction(m->template_restriction),
            .is_instance = true,
            .depth = static_cast<std::uint32_t>(declarator->arraydef.size()),
        };
        const auto actual_type = CheckType(declarator->value, expected_decl_type);
        MatchTypes(declarator->value->nrange, actual_type, expected_decl_type);
      }
      return false;
    };

    case ast::NodeKind::FormalPar: {
      const auto* m = n->As<ast::nodes::FormalPar>();

      auto expected_type = CheckType(m->type);
      expected_type.is_instance = true;
      expected_type.restriction = ParseTemplateRestriction(m->restriction);
      //
      EnsureIsAType(expected_type.sym, m->type);

      if (const auto* default_value = m->value; default_value) {
        const auto actual_type = CheckType(default_value, expected_type);
        MatchTypes(default_value->nrange, actual_type, expected_type);
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

      if (f->value_constraint && f->type->nkind == ast::NodeKind::RefSpec) {  // TODO
        const auto* sym = scope_->Resolve(sf_.Text(*f->type->As<ast::nodes::RefSpec>()->x));
        for (const auto* item : f->value_constraint->list) {
          CheckType(item, {.sym = sym});
        }
      }

      return false;
    }

    case ast::NodeKind::TemplateDecl: {
      const auto* m = n->As<ast::nodes::TemplateDecl>();

      if (m->params) {
        Visit(m->params);
      }

      auto expected_type = CheckType(m->type);
      expected_type.restriction = ParseTemplateRestriction(m->restriction);
      //
      const auto actual_type = CheckType(m->value, expected_type);
      MatchTypes(m->value->nrange, actual_type, expected_type);

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

      const auto expected_type = ResolveCallableReturnType(&sf_, fdecl);
      const auto actual_type = CheckType(m->result, expected_type);
      MatchTypes(m->result->nrange, actual_type, expected_type);

      return false;
    }

    case ast::NodeKind::IfStmt: {
      const auto* m = n->As<ast::nodes::IfStmt>();

      const InstantiatedType expected_type{.sym = &builtins::kBoolean};
      const auto cond_type = CheckType(m->cond, expected_type);
      MatchTypes(m->cond->nrange, cond_type, expected_type);

      ScopedVisit(m->consequent);
      if (m->alternate) {
        Visit(m->alternate);
      }

      return false;
    }
    case ast::NodeKind::WhileStmt: {
      const auto* m = n->As<ast::nodes::WhileStmt>();

      if (m->cond) [[likely]] {
        const InstantiatedType expected_type{.sym = &builtins::kBoolean};
        const auto cond_type = CheckType(m->cond, expected_type);
        MatchTypes(m->cond->nrange, cond_type, expected_type);
      }

      if (m->body) [[likely]] {
        ScopedVisit(m->body);
      }

      return false;
    }
    case ast::NodeKind::DoWhileStmt: {
      const auto* m = n->As<ast::nodes::WhileStmt>();

      if (m->cond) [[likely]] {
        const InstantiatedType expected_type{.sym = &builtins::kBoolean};
        const auto cond_type = CheckType(m->cond, expected_type);
        MatchTypes(m->cond->nrange, cond_type, expected_type);
      }

      if (m->body) [[likely]] {
        ScopedVisit(m->body);
      }

      return false;
    }
    case ast::NodeKind::ForStmt: {
      const auto* m = n->As<ast::nodes::ForStmt>();

      const InstantiatedType expected_type{.sym = &builtins::kBoolean};
      const auto cond_type = CheckType(m->cond, expected_type);
      MatchTypes(m->cond->nrange, cond_type, expected_type);

      if (m->init) [[likely]] {
        Visit(m->init);
      }
      if (m->post) [[likely]] {
        Visit(m->post);
      }
      if (m->body) [[likely]] {
        ScopedVisit(m->body);
      }

      return false;
    }

    case ast::NodeKind::SelectStmt: {
      const auto* m = n->As<ast::nodes::SelectStmt>();

      const auto tag_type = CheckType(m->tag);
      if (!tag_type) {
        return true;
      }

      if (m->is_union) {
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
                                         semantic::utils::GetReadableTypeName(tag_type.sym)),
              });
            }
          }
          ScopedVisit(clause->body);
        }
      } else {
        for (const auto* clause : m->clauses) {
          for (const auto* cond : clause->cond) {
            MatchTypes(cond->nrange, CheckType(cond, tag_type), tag_type);
          }
          ScopedVisit(clause->body);
        }
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
