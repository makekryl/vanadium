#include "vanadium/asn1/ast/Asn1AstTransformer.h"

#include <asn1c/libasn1common/genhash.h>
#include <asn1c/libasn1parser/asn1parser_cxx.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/Scanner.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>
#include <vanadium/lib/ScopedValue.h>
#include <vanadium/lib/StaticMap.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <format>
#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <print>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "vanadium/asn1/ast/ClassSetResolver.h"

// For readability reasons, it is preferable NOT to use auto type detection for variables of asn1p types

namespace vanadium::asn1::ast {

namespace {
constexpr auto kBuiltinTypeMapping = lib::MakeStaticMap<asn1p_expr_type_e, std::string_view>({
    {ASN_BASIC_NULL, "___asn1_NULL_t"},
    {ASN_BASIC_OBJECT_IDENTIFIER, "objid"},
    {ASN_BASIC_INTEGER, "integer"},
    {ASN_BASIC_REAL, "float"},
    {ASN_BASIC_BOOLEAN, "boolean"},
    {ASN_BASIC_BIT_STRING, "bitstring"},
    {ASN_BASIC_OCTET_STRING, "octetstring"},
    {ASN_BASIC_CHARACTER_STRING, "charstring"},
});

void NormalizeToken(ttcn_ast::Range& range, std::string& s) {
  std::replace(s.begin() + range.begin, s.begin() + range.end, '-', '_');

  auto token = range.String(s);
  if (ttcn_ast::parser::IsKeyword(token)) {
    s[range.end] = '_';
    ++range.end;
  }
}
}  // namespace

#define DEBUG(...)                     \
  do {                                 \
    std::println(stderr, __VA_ARGS__); \
  } while (0);
#define DEBUG(...) ;

// asn1c helper stuff
namespace {

const asn1p_expr_t* ResolveModuleMember(const asn1p_module_t* mod, const char* member_name) {
  const auto* expr = (asn1p_expr_t*)genhash_get(mod->members_hash, member_name);

  DEBUG(" -> Resolve({})", member_name);
  if (expr) {
    DEBUG(" &&& '{}': {} / {}", expr->Identifier ? expr->Identifier : "((EMPTY))",
          magic_enum::enum_name(expr->meta_type), magic_enum::enum_name(expr->expr_type));
  } else {
    DEBUG(" &&& FAILED to resolve");
  }

  return expr;
}
const asn1p_expr_t* ResolveExprMember(const asn1p_expr_t* expr, const char* member_name) {
  asn1p_expr_t* child;
  TQ_FOR(child, &(expr->members), next) {
    if (child->Identifier && std::strcmp(child->Identifier, member_name) == 0) {
      return child;
    }
  }

  return nullptr;
}
const asn1p_constraint_t* FindConstraint(const asn1p_expr_t* expr, asn1p_constraint_type_e type) {
  for (int i = 0; i < expr->constraints->el_count; ++i) {
    if (expr->constraints->elements[i]->type == type) {
      return expr->constraints->elements[i];
    }
  }
  return nullptr;
}

void DumpExpr(std::string_view prefix, const asn1p_expr_t* expr) {
  DEBUG("{}'{}': meta={} / etype={}, rspecs?={}", prefix, expr->Identifier ? expr->Identifier : "<EMPTY>",
        magic_enum::enum_name(expr->meta_type), magic_enum::enum_name(expr->expr_type), !!expr->rhs_pspecs);
}
}  // namespace

class AstTransformer {
 public:
  AstTransformer(const asn1p_t* ast, std::string_view src, lib::Arena& arena, Asn1pModuleProvider module_provider)
      : ast_(ast), original_src_(src), arena_(arena), get_module_(std::move(module_provider)) {
    adjusted_src_.reserve(original_src_.length() + 256);
    adjusted_src_.append(original_src_);
  }

  TransformedAsn1Ast Transform() {
    auto* root = TransformRoot();
    return {
        .adjusted_src = *arena_.Alloc<std::string>(std::move(adjusted_src_)),
        .root = root,
        .errors = std::move(errors_),
    };
  }

 private:
  ttcn_ast::RootNode* TransformRoot() {
    const auto* mod = TQ_FIRST(&(ast_->modules));
    return NewNode<ttcn_ast::RootNode>([&](ttcn_ast::RootNode& m) {
      m.nrange = {.begin = 0, .end = static_cast<ttcn_ast::pos_t>(original_src_.length())};
      m.nodes.push_back(TransformModule(mod));
    });
  }

  ttcn_ast::nodes::Module* TransformModule(const asn1p_module_t* mod) {
    return NewNode<ttcn_ast::nodes::Module>([&](ttcn_ast::nodes::Module& m) {
      m.nrange = {.begin = 0, .end = static_cast<ttcn_ast::pos_t>(original_src_.length())};
      m.name.emplace().nrange = ConsumeRange(mod->_ModuleName_Range);

      asn1p_expr_t* tc;
      TQ_FOR(tc, &(mod->members), next) {
        if (auto* dn = TransformOutermostExpr(tc); dn != nullptr) {
          m.defs.push_back(NewNode<ttcn_ast::nodes::Definition>([&](ttcn_ast::nodes::Definition& def) {
            def.def = dn;
            dn->parent = &def;
          }));
        }
      }
    });
  }

  ttcn_ast::Node* TransformOutermostExpr(const asn1p_expr_t* expr) {
    DumpExpr("[OUTER] ", expr);

    if (expr->lhs_params) {
      // [top-level def] if the type is parametrizable, do not emit it
      return nullptr;
    }

    switch (expr->meta_type) {
      case AMT_VALUE:
        return TransformValueDeclaration(expr);

      case AMT_OBJECTCLASS:
      case AMT_VALUESET:
        // those are not present in TTCN-3
        return nullptr;

      default:
        break;
    }

    switch (expr->expr_type) {
      case ASN_CONSTR_SEQUENCE:
        return TransformStruct(ttcn_ast::TokenKind::RECORD, expr);
      case ASN_CONSTR_SET:
        return TransformStruct(ttcn_ast::TokenKind::SET, expr);
      case ASN_CONSTR_CHOICE:
        return TransformStruct(ttcn_ast::TokenKind::UNION, expr);

      case ASN_CONSTR_SEQUENCE_OF:
        return TransformListSubTypeDecl(ttcn_ast::TokenKind::RECORD, expr);
      case ASN_CONSTR_SET_OF:
        return TransformListSubTypeDecl(ttcn_ast::TokenKind::SET, expr);

      case ASN_BASIC_ENUMERATED:
        return TransformEnumeration(expr);

      default: {
        break;
      }
    }

    auto* n = TransformExpr(expr);
    if (n && n->nkind == ttcn_ast::NodeKind::RefSpec) {
      auto* rs = NewNode<ttcn_ast::nodes::SubTypeDecl>([&](ttcn_ast::nodes::SubTypeDecl& m) {
        m.field = NewNode<ttcn_ast::nodes::Field>([&](ttcn_ast::nodes::Field& f) {
          f.type = n->As<ttcn_ast::nodes::RefSpec>();
          f.name.emplace().nrange = ConsumeRange(expr->_Identifier_Range);
        });
      });
      rs->parent = std::exchange(n->parent, rs);
      rs->nrange = n->nrange;  // TODO(range): maybe better take from the expr, but it may break binsearch
      return rs;
    }

    return n;
  }

  ttcn_ast::Node* TransformExpr(const asn1p_expr_t* expr) {
    DumpExpr("      * ", expr);
    switch (expr->expr_type) {
      case ASN_CONSTR_SEQUENCE:
        return TransformInnerStruct(ttcn_ast::TokenKind::RECORD, expr);
      case ASN_CONSTR_SET:
        return TransformInnerStruct(ttcn_ast::TokenKind::SET, expr);
      case ASN_CONSTR_CHOICE:
        return TransformInnerStruct(ttcn_ast::TokenKind::UNION, expr);

      case ASN_CONSTR_SEQUENCE_OF:
        return TransformListSpec(ttcn_ast::TokenKind::RECORD, expr);
      case ASN_CONSTR_SET_OF:
        return TransformListSpec(ttcn_ast::TokenKind::SET, expr);

      case ASN_BASIC_ENUMERATED:
        return TransformInnerEnumeration(expr);

      case A1TC_REFERENCE:
        return TransformTypeReference(expr);

      case A1TC_EXTENSIBLE:
        return nullptr;

      default: {
        if (auto* builtin_typename_node = TransformBuiltinTypeExpr(expr); builtin_typename_node) {
          return NewNode<ttcn_ast::nodes::RefSpec>([&](ttcn_ast::nodes::RefSpec& m) {
            m.x = builtin_typename_node;
            builtin_typename_node->parent = &m;
          });
        }
        return nullptr;
      }
    }
  }

  ttcn_ast::nodes::TypeSpec* TransformTypeReference(const asn1p_expr_t* expr) {
    auto* n = TransformTypeName(expr);
    if (!n) {
      return nullptr;
    }
    if (ttcn_ast::nodes::TypeSpec::IsTypeSpec(n)) {
      return n->As<ttcn_ast::nodes::TypeSpec>();
    }

    auto* rs = NewNode<ttcn_ast::nodes::RefSpec>([&](ttcn_ast::nodes::RefSpec& m) {
      m.x = n->As<ttcn_ast::nodes::Expr>();
    });
    rs->parent = std::exchange(n->parent, rs);
    rs->nrange = n->nrange;  // TODO(range): maybe better take from the expr, but it may break binsearch
    return rs;
  }

  // It will return either an Ident, which can be wrapped in RefSpec (if needed, e.g. in ValueDecl it will stay Ident),
  // or a TypeSpec (class sets are translated into an UNION StructSpec)
  // So, this is a VERY VERY smart ass method that implements maybe the 75% of the "semantic" part of the transformer
  ttcn_ast::Node* TransformTypeName(const asn1p_expr_t* expr) {
    DumpExpr(" ## TransformTypeName ", expr);
    for (int i = 0; i < expr->reference->comp_count; i++) {
      DEBUG("     - comp[{}].name = '{}'", i, expr->reference->components[i].name);
    }

    const asn1p_ref_t* ref = expr->reference;

    if (ref->comp_count == 1) {  // errorInfo     ErrorInfoType
      if (expr->rhs_pspecs) {
        // parametrized type instantiation requested, we need to inline it
        const asn1p_expr_t* memexpr = ResolveReference(ref);
        if (!memexpr) {
          return nullptr;
        }
        return Parametrize(memexpr, expr, [&] {
          return TransformExpr(memexpr);
        });
      }
      return NewNode<ttcn_ast::nodes::Ident>([&](ttcn_ast::nodes::Ident& ident) {
        ident.nrange = ConsumeRange(ref->components[0]._name_range);
      });
    }

    if (ref->comp_count != 2) {
      // TODO: check it
      EmitError(ConsumeRange(ref->components[0]._name_range), "ref->comp_count != 2");
      return nullptr;
    }

    // errorInfo     ERROR-CLASS.&Type
    const asn1p_ref_s::asn1p_ref_component_s& clscomp = ref->components[0];
    const asn1p_ref_s::asn1p_ref_component_s& selcomp = ref->components[1];
    switch (selcomp.lex_type) {
      case RLT_AmpUppercase:  // &Type
      case RLT_Amplowercase:  // &code
        break;
      default:
        EmitError(ConsumeRange(selcomp._name_range), "expression does not look like a CLASS field reference");
        return nullptr;
    }

    const asn1p_expr_t* clsexpr = ResolveModuleMember(ref->module, clscomp.name);
    if (!clsexpr) {
      EmitError(ConsumeRange(clscomp._name_range), std::format("unresolved reference to '{}'", clscomp.name));
      return nullptr;
    }
    if (clsexpr->expr_type != A1TC_CLASSDEF) {
      EmitError(ConsumeRange(clscomp._name_range), std::format("'{}' is not a CLASS", clscomp.name));
      return nullptr;
    }

    const asn1p_expr_t* fieldexpr = ResolveExprMember(clsexpr, selcomp.name);
    if (!fieldexpr) {
      EmitError(ConsumeRange(clscomp._name_range),
                std::format("unresolved reference to field '{}' of CLASS '{}'", selcomp.name, clscomp.name));
      return nullptr;
    }
    DumpExpr("[FIELD] ", fieldexpr);
    if (fieldexpr->meta_type != AMT_OBJECTFIELD) [[unlikely]] {
      // TODO: i guess this will never happen
      EmitError(ConsumeRange(selcomp._name_range), std::format("'{}' is not a field", selcomp.name));
      return nullptr;
    }

    switch (fieldexpr->expr_type) {
      case A1TC_CLASSFIELD_FTVFS: {  // &code, fixed type
        const asn1p_expr_t* reftype = TQ_FIRST(&(fieldexpr->members));
        return TransformExpr(reftype)->As<ttcn_ast::nodes::Expr>();
      }
      case A1TC_CLASSFIELD_TFS: {  // &Type, open type
        // we need to inspect the expr itself to determine what is it constrained to

        // TODO: pass proper ranges to ConsumeRange calls below

        const asn1p_constraint_t* comprel_constr = FindConstraint(expr, ACT_CA_CRC);  // ({ErrorSet}{@errorCategory})
        if (!comprel_constr) {
          EmitError(ConsumeRange(selcomp._name_range), "component relation constraint required");
          return nullptr;
        }
        assert(comprel_constr->el_count >= 1);  // should always be true for ACT_CA_CRC

        const asn1p_constraint_t* clsvals_constr = comprel_constr->elements[0];
        if (!clsvals_constr->value || (clsvals_constr->value->type != asn1p_value_s::ATV_REFERENCED))
            [[unlikely]] {  // should not happen though
          EmitError(ConsumeRange(selcomp._name_range), "reference required");
          return nullptr;
        }
        if (clsvals_constr->value->value.reference->comp_count != 1) {
          // TODO: support cases where clsvals_constr->value->value.reference->comp_count > 1
          EmitError(ConsumeRange(selcomp._name_range), "clsvals_constr->value->value.reference->comp_count != 1");
          return nullptr;
        }

        const asn1p_expr_t* clsvals_expr = ResolveReference(clsvals_constr->value->value.reference);
        if (!clsvals_expr) {
          return nullptr;
        }

        if ((clsvals_expr->reference->comp_count != 1) ||
            (std::strcmp(clsvals_expr->reference->components[0].name, clsexpr->Identifier) != 0)) {
          EmitError(ConsumeRange(clscomp._name_range),
                    std::format("referenced set is not of type '{}', but of type '{}'", clsexpr->Identifier,
                                clsvals_expr->reference->components[0].name));
          return nullptr;
        }

        return NewNode<ttcn_ast::nodes::StructSpec>([&](ttcn_ast::nodes::StructSpec& m) {
          m.kind = Tok(ttcn_ast::TokenKind::UNION);
          m.fields.reserve(clsvals_expr->constraints->el_count);

          const auto accept_row = [&](const ClassObjectRow& row) {
            if (row.name != selcomp.name) {
              return true;  // continue search
            }

            // todo: e.g. 'OCTET STRING' is translated to { name = 'oCTET STRING', type = 'OCTET STRING' },
            //       while it is expected to be { name = 'oCTET_STRING', type = 'octetstring' }
            // todo: parse the value like asn1c does and validate it

            m.fields.emplace_back(NewNode<ttcn_ast::nodes::Field>([&](ttcn_ast::nodes::Field& f) {
              // TODO: optimize
              f.name.emplace().nrange = AppendSource(std::string(row.value));
              f.type = NewNode<ttcn_ast::nodes::RefSpec>([&](ttcn_ast::nodes::RefSpec& rs) {
                rs.x = NewNode<ttcn_ast::nodes::Ident>([&](ttcn_ast::nodes::Ident& ident) {
                  std::string ptypecpy(row.value);  // MEGA SHIT
                  ptypecpy[0] = std::tolower(ptypecpy[0]);
                  ident.nrange = AppendSource(ptypecpy);  // TODO: oh shi... (x2)
                });
              });
            }));

            return false;
          };

          // this cannot be written more cleanly due to FunctionRef lifetime requirements
          ResolveClassSet(clsvals_expr, clsexpr,
                          {
                              .resolve =
                                  [&](const asn1p_ref_t* ref) {
                                    return ResolveReference(ref);
                                  },
                              .accept_class =
                                  [&](const auto& accept_accept_class_object_consumer) {
                                    accept_accept_class_object_consumer({
                                        .accept_row = accept_row,
                                        .emit_error =
                                            [&](const auto& asn1c_range, std::string message) {
                                              EmitError(ConsumeRange(asn1c_range), std::move(message));
                                            },
                                    });
                                    return true;
                                  },
                              .emit_error =
                                  [&](const auto& asn1c_range, std::string message) {
                                    EmitError(ConsumeRange(asn1c_range), std::move(message));
                                  },
                          });
        });
      }
      default:
        EmitError(ConsumeRange(selcomp._name_range),
                  std::format("unexpected field expression type: {}", magic_enum::enum_name(fieldexpr->expr_type)));
        return nullptr;
    }

    return nullptr;
  }

  ttcn_ast::nodes::Expr* TransformBuiltinTypeExpr(const asn1p_expr_t* expr) {
    if (const auto& ttcn_typename = kBuiltinTypeMapping.get(expr->expr_type); ttcn_typename) {
      return NewNode<ttcn_ast::nodes::Ident>([&](ttcn_ast::nodes::Ident& ident) {
        ident.nrange = AppendSource(std::string(*ttcn_typename));  // TODO: oh shi...
      });
    }
    return nullptr;
  }

  ttcn_ast::nodes::ValueDecl* TransformValueDeclaration(const asn1p_expr_t* expr) {
    if (expr->expr_type == A1TC_REFERENCE && IsAClassRef(expr->reference)) {
      return nullptr;
    }

    return NewNode<ttcn_ast::nodes::ValueDecl>([&](ttcn_ast::nodes::ValueDecl& m) {
      if (expr->expr_type == A1TC_REFERENCE) {
        auto* type = TransformTypeName(expr);
        assert(ttcn_ast::nodes::Expr::IsExpr(type));
        m.type = type->As<ttcn_ast::nodes::Expr>();
      } else if (auto* builtin_type_expr = TransformBuiltinTypeExpr(expr); builtin_type_expr) {
        m.type = builtin_type_expr;
      }
      m.kind = arena_.Alloc<ttcn_ast::Token>(Tok(ttcn_ast::TokenKind::CONST));

      m.decls.push_back(NewNode<ttcn_ast::nodes::Declarator>([&](ttcn_ast::nodes::Declarator& d) {
        d.name.emplace().nrange = ConsumeRange(expr->_Identifier_Range);
      }));
    });
  }

  ttcn_ast::nodes::StructTypeDecl* TransformStruct(ttcn_ast::TokenKind kind /* RECORD/SET/UNION */,
                                                   const asn1p_expr_t* expr) {
    return NewNode<ttcn_ast::nodes::StructTypeDecl>([&](ttcn_ast::nodes::StructTypeDecl& m) {
      m.kind = Tok(kind);

      m.name.emplace().nrange = ConsumeRange(expr->_Identifier_Range);

      asn1p_expr_t* se;
      TQ_FOR(se, &(expr->members), next) {
        if (auto* dn = TransformComponent(se); dn) {
          m.fields.push_back(dn);
        }
      }
    });
  }

  ttcn_ast::nodes::StructSpec* TransformInnerStruct(ttcn_ast::TokenKind kind /* RECORD/SET/UNION */,
                                                    const asn1p_expr_t* expr) {
    return NewNode<ttcn_ast::nodes::StructSpec>([&](ttcn_ast::nodes::StructSpec& m) {
      m.kind = Tok(kind);

      asn1p_expr_t* se;
      TQ_FOR(se, &(expr->members), next) {
        if (auto* dn = TransformComponent(se); dn) {
          m.fields.push_back(dn);
        }
      }
    });
  }

  ttcn_ast::nodes::EnumTypeDecl* TransformEnumeration(const asn1p_expr_t* expr) {
    return NewNode<ttcn_ast::nodes::EnumTypeDecl>([&](ttcn_ast::nodes::EnumTypeDecl& m) {
      m.name.emplace().nrange = ConsumeRange(expr->_Identifier_Range);

      asn1p_expr_t* member;
      TQ_FOR(member, &(expr->members), next) {
        m.values.push_back(NewNode<ttcn_ast::nodes::Ident>([&](ttcn_ast::nodes::Ident& iv) {
          iv.nrange = ConsumeRange(member->_Identifier_Range);
        }));
      }
    });
  }

  ttcn_ast::nodes::EnumSpec* TransformInnerEnumeration(const asn1p_expr_t* expr) {
    return NewNode<ttcn_ast::nodes::EnumSpec>([&](ttcn_ast::nodes::EnumSpec& m) {
      asn1p_expr_t* member;
      TQ_FOR(member, &(expr->members), next) {
        m.values.push_back(NewNode<ttcn_ast::nodes::Ident>([&](ttcn_ast::nodes::Ident& iv) {
          iv.nrange = ConsumeRange(member->_Identifier_Range);
        }));
      }
    });
  }

  ttcn_ast::nodes::ListSpec* TransformListSpec(ttcn_ast::TokenKind kind, const asn1p_expr_t* expr) {
    return NewNode<ttcn_ast::nodes::ListSpec>([&](ttcn_ast::nodes::ListSpec& m) {
      m.kind = Tok(kind);

      m.elemtype = TransformExpr(TQ_FIRST(&(expr->members)))->As<ttcn_ast::nodes::TypeSpec>();
    });
  }

  ttcn_ast::nodes::SubTypeDecl* TransformListSubTypeDecl(ttcn_ast::TokenKind kind, const asn1p_expr_t* expr) {
    return NewNode<ttcn_ast::nodes::SubTypeDecl>([&](ttcn_ast::nodes::SubTypeDecl& m) {
      m.field = NewNode<ttcn_ast::nodes::Field>([&](ttcn_ast::nodes::Field& f) {
        f.type = TransformListSpec(kind, expr);
        f.name.emplace().nrange = ConsumeRange(expr->_Identifier_Range);
      });
    });
  }

  ttcn_ast::nodes::Field* TransformComponent(const asn1p_expr_t* se) {
    auto* c = TransformExpr(se);
    if (!c) {
      return nullptr;
    }

    return NewNode<ttcn_ast::nodes::Field>([&](ttcn_ast::nodes::Field& m) {
      m.name.emplace().nrange = ConsumeRange(se->_Identifier_Range);

      if ((se->marker.flags & asn1p_expr_s::asn1p_expr_marker_s::EM_DEFAULT) ==
          asn1p_expr_s::asn1p_expr_marker_s::EM_DEFAULT) {
        // safe_printf(" DEFAULT ");
        // asn1print_value(se->marker.default_value, flags);
      } else if ((se->marker.flags & asn1p_expr_s::asn1p_expr_marker_s::EM_OPTIONAL) ==
                 asn1p_expr_s::asn1p_expr_marker_s::EM_OPTIONAL) {
        m.optional = true;
      }

      m.type = c->As<ttcn_ast::nodes::TypeSpec>();
    });
  }

  //

  template <ttcn_ast::IsNode T>
  T* NewNode(std::invocable<T&> auto f) {
    auto* p = arena_.Alloc<T>();
    p->nrange = {.begin = 0, .end = 0};
    //
    auto* top = last_node_;
    p->parent = top;
    last_node_ = p;
    //
    f(*p);
    //
    last_node_ = top;
    return p;
  }

  void EmitError(ttcn_ast::Range range, std::string&& message) {
    errors_.emplace_back(TransformedAsn1Ast::TransformationError{
        .range = std::move(range),
        .message = std::move(message),
    });
  }

  ttcn_ast::Range ConsumeRange(const asn1p_src_range_t& range) {
    ttcn_ast::Range ttcn_range{.begin = range.begin, .end = range.end};
    NormalizeToken(ttcn_range, adjusted_src_);
    return ttcn_range;
  }

  std::unordered_map<std::string, ttcn_ast::Range> appended_ranges_;
  ttcn_ast::Range AppendSource(std::string s) {
    // TODO: optimize, but all strings should fall in the SSO buffer anyway
    // keeping an unordered_map is not good though

    if (auto it = appended_ranges_.find(s); it != appended_ranges_.end()) {
      return it->second;
    }

    ttcn_ast::Range range{.begin = static_cast<ttcn_ast::pos_t>(adjusted_src_.length())};
    range.end = range.begin + static_cast<ttcn_ast::pos_t>(s.length());

    adjusted_src_.append(s);
    NormalizeToken(range, adjusted_src_);

    appended_ranges_.emplace(std::move(s), range);

    return range;
  }

  std::unordered_set<std::string_view> known_class_names_;
  bool IsAClassRef(const asn1p_ref_t* ref) {
    const char* name = ref->components[0].name;
    if (known_class_names_.contains(name)) {
      return true;
    }

    const auto* referenced_expr = ResolveModuleMember(ref->module, name);
    if (referenced_expr && referenced_expr->meta_type == AMT_OBJECTCLASS) {
      known_class_names_.emplace(name);
      return true;
    }

    return false;
  }

  struct ParametrizationContext {
    const ParametrizationContext* parent;
    const asn1p_expr_t *target, *provider;
  };
  const ParametrizationContext* active_parametrization_ctx_{nullptr};
  // This method is used by the semantic analyser part of the translator
  // (resolving is not needed for AST transformation, but we need it to
  //   instantiate parametrized types, resolve class constraints, etc.)
  // It also takes care of the "unresolved reference" error emission
  //
  // TODO: maybe we would like to split up the transformer
  // into the dump, pure transformer and the smart analyser parts
  const asn1p_expr_t* ResolveReference(const asn1p_ref_t* ref) {
    if (ref->comp_count != 1) {
      // TODO: support complex cases
      return nullptr;
    }

    if (const auto* expr = TryResolveReferenceViaParameters(ref, active_parametrization_ctx_); expr) {
      if (expr->expr_type == A1TC_REFERENCE) {
        return ResolveReferenceViaModule(expr->reference);
      }
      return expr;
    }
    if (const auto* expr = ResolveReferenceViaModule(ref); expr) {
      return expr;
    }

    EmitError(ConsumeRange(ref->components[0]._name_range),
              std::format("unresolved reference to '{}'", ref->components[0].name));
    return nullptr;
  }
  const asn1p_expr_t* ResolveReferenceViaModule(const asn1p_ref_t* ref) {
    // TODO: support complex cases
    return ResolveReferenceViaModule(ref->module, ref->components[0].name);
  }
  const asn1p_expr_t* ResolveReferenceViaModule(const asn1p_module_t* own_module, const char* name) {
    if (const auto* expr = ResolveModuleMember(own_module, name); expr) {
      return expr;
    }

    // TODO: check ambigious references, check modules' OID too
    const asn1p_xports_t* xp;
    TQ_FOR(xp, &(own_module->imports), xp_next) {
      const asn1p_expr_t* tc;
      TQ_FOR(tc, &(xp->xp_members), next) {
        if (std::strcmp(name, tc->Identifier) != 0) {
          continue;
        }

        const asn1p_module_t* provider_module = get_module_(xp->fromModuleName);
        if (!provider_module) {
          return nullptr;
        }
        return ResolveReferenceViaModule(provider_module, name);
      }
    }

    return nullptr;
  }
  const asn1p_expr_t* TryResolveReferenceViaParameters(const asn1p_ref_t* ref, const ParametrizationContext* ctx) {
    if (!ctx) {
      return nullptr;
    }

    // TODO: support complex cases
    const char* name = ref->components[0].name;

    for (int i = 0; i < ctx->target->lhs_params->params_count; i++) {
      const asn1p_paramlist_s::asn1p_param_s& slot = ctx->target->lhs_params->params[i];
      if (std::strcmp(slot.argument, name) != 0) {
        continue;
      }

      // we have the guarantee of Parametrize(...) that provided & required parameters count match
      const asn1p_expr_t* arg = TQ_FIRST(&(ctx->provider->rhs_pspecs->members));
      for (int j = 0; j < i; j++) {
        arg = TQ_NEXT(arg, next);
      }
      const auto* ret = [&] -> const asn1p_expr_t* {
        switch (arg->expr_type) {
          case A1TC_VALUESET: {
            if (arg->constraints->type == ACT_EL_TYPE) {
              return arg->constraints->containedSubtype->value.v_type;
            }
            return nullptr;
          }
          default:
            return arg;
        }
      }();
      if (ret) {
        if (ret->expr_type == A1TC_REFERENCE) {
          // resolve a chain of parameters, e.g.
          //    A { Type } ::= SEQUENCE { t Type }
          //    B { Type } ::= A { Type }
          //    C ::= B { INTEGER }
          // inside A, Type is INTEGER through B
          if (const auto* terminal = TryResolveReferenceViaParameters(ret->reference, ctx->parent); terminal) {
            ret = terminal;
          }
        }
      }
      return ret;
    }
    return TryResolveReferenceViaParameters(ref, ctx->parent);
  }
  ttcn_ast::Node* Parametrize(const asn1p_expr_t* target, const asn1p_expr_t* provider,
                              std::invocable /*<ttcn_ast::Node*()>*/ auto f) {
    const auto check_type_match = [&](const asn1p_paramlist_s::asn1p_param_s& slot, const asn1p_expr_t* arg) {
      assert(arg->reference);
      const asn1p_expr_t* referenced_expr = ResolveReference(arg->reference);
      if (!referenced_expr) {
        return;
      }
      DumpExpr("  referenced_expr  /// ", referenced_expr);

      if (referenced_expr->expr_type != A1TC_REFERENCE) {
        // interesting... TODO: check later if it is possible
        return;
      }
      assert(referenced_expr->reference);

      if (asn1p_ref_compare(slot.governor, referenced_expr->reference) != 0) {
        // todo: maybe deep compare (not just lexicographically), also print ALL components everywhere
        EmitError(ConsumeRange(arg->reference->components[0]._name_range),
                  std::format("expected parameter of type '{}', got '{}'", slot.governor->components[0].name,
                              referenced_expr->reference->components[0].name));
      }
    };

    const asn1p_expr_t* arg = TQ_FIRST(&(provider->rhs_pspecs->members));

    int i;
    for (i = 0; (i < target->lhs_params->params_count) && arg; i++, arg = TQ_NEXT(arg, next)) {
      const asn1p_paramlist_s::asn1p_param_s& slot = target->lhs_params->params[i];

      DEBUG(" slot.governor='{}', slot.argument='{}'",
            slot.governor ? slot.governor->components[0].name : "NO GOVERNOR", slot.argument);
      DumpExpr("   ARG TO SLOT ^ : ", arg);

      switch (arg->expr_type) {
        case A1TC_VALUESET: {
          if (arg->constraints->type == ACT_EL_TYPE) {
            check_type_match(slot, arg->constraints->containedSubtype->value.v_type);
          }
          break;
        }

        default:
          // idk what to do
          break;
      }
    }
    if (i != target->lhs_params->params_count) {
      // TODO(range): provide range
      EmitError({}, std::format("expected {} parameters, got {}", target->lhs_params->params_count, i));
      return nullptr;
    }

    const ParametrizationContext ctx{
        .parent = active_parametrization_ctx_,
        .target = target,
        .provider = provider,
    };
    lib::ScopedValue guard(active_parametrization_ctx_, &ctx);
    return f();
  }

  ttcn_ast::Node* last_node_{nullptr};

  const asn1p_t* ast_;
  std::string adjusted_src_;
  std::string_view original_src_;
  std::vector<TransformedAsn1Ast::TransformationError> errors_;
  lib::Arena& arena_;
  Asn1pModuleProvider get_module_;

  //

  static ttcn_ast::Token Tok(ttcn_ast::TokenKind kind) {
    return {.kind = kind, .range = {}};
  }
};

TransformedAsn1Ast TransformAsn1Ast(const asn1p_t* ast, std::string_view src, lib::Arena& arena,
                                    Asn1pModuleProvider module_provider) {
  return AstTransformer(ast, src, arena, std::move(module_provider)).Transform();
}

}  // namespace vanadium::asn1::ast
