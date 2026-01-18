#include "vanadium/asn1/ast/Asn1AstTransformer.h"

#include <asn1c/libasn1common/genhash.h>
#include <asn1c/libasn1parser/asn1parser_cxx.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/Scanner.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>
#include <vanadium/lib/StaticMap.h>

#include <cstring>
#include <format>
#include <optional>
#include <print>
#include <string_view>

#include "magic_enum/magic_enum.hpp"

namespace vanadium::asn1::ast {

namespace {
constexpr auto kBuiltinTypeMapping = lib::MakeStaticMap<asn1p_expr_type_e, std::string_view>({
    {ASN_BASIC_NULL, "__NULL_t"},
    {ASN_BASIC_OBJECT_IDENTIFIER, "objid"},
    {ASN_BASIC_INTEGER, "integer"},
    {ASN_BASIC_REAL, "float"},
    {ASN_BASIC_BOOLEAN, "boolean"},
    {ASN_BASIC_BIT_STRING, "bitstring"},
    {ASN_BASIC_OCTET_STRING, "octetstring"},
    {ASN_BASIC_CHARACTER_STRING, "charstring"},
});

inline void ExtendByIncorporatedNode(ttcn_ast::Node* n, ttcn_ast::Node* x) {
  n->parent = std::exchange(x->parent, n);
  n->nrange.begin = x->nrange.begin;
}

void DumpExpr(std::string_view prefix, const asn1p_expr_t* expr) {
  std::println(stderr, "{}'{}': meta={} / etype={}", prefix, expr->Identifier ? expr->Identifier : "<EMPTY>",
               magic_enum::enum_name(expr->meta_type), magic_enum::enum_name(expr->expr_type));
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
    switch (expr->meta_type) {
      case AMT_VALUE:
        return TransformValueDeclaration(expr);
      case AMT_OBJECTCLASS:
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
        return TransformSubTypeDecl(ttcn_ast::TokenKind::RECORD, expr);
      case ASN_CONSTR_SET_OF:
        return TransformSubTypeDecl(ttcn_ast::TokenKind::SET, expr);

      case ASN_BASIC_ENUMERATED:
        return TransformEnumeration(expr);

      default: {
        break;
      }
    }

    return TransformExpr(expr);
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

  ttcn_ast::nodes::RefSpec* TransformTypeReference(const asn1p_expr_t* expr) {
    return NewNode<ttcn_ast::nodes::RefSpec>([&](ttcn_ast::nodes::RefSpec& m) {
      m.x = TransformTypeExpr(expr);
    });
  }

  ttcn_ast::nodes::Expr* TransformTypeExpr(const asn1p_expr_t* expr) {
    const asn1p_ref_t* ref = expr->reference;

    if (ref->comp_count == 1) {
      return NewNode<ttcn_ast::nodes::Ident>([&](ttcn_ast::nodes::Ident& ident) {
        ident.nrange = ConsumeRange(ref->components[0]._name_range);
      });
    }

    if (ref->comp_count != 2) {
      // TODO: check it
      EmitError(ConsumeRange(ref->components[0]._name_range), "ref->comp_count != 2");
      return nullptr;
    }

    const asn1p_ref_s::asn1p_ref_component_s& clscomp = ref->components[0];
    const asn1p_ref_s::asn1p_ref_component_s& selcomp = ref->components[1];
    switch (selcomp.lex_type) {
      case RLT_AmpUppercase:
      case RLT_Amplowercase:
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

        if (expr->constraints->el_count == 0) {
          EmitError(ConsumeRange(selcomp._name_range), "component relation constraint expected");
          return nullptr;
        }

        const asn1p_constraint_t* constr = expr->constraints->elements[0];
        if (constr->type != ACT_CA_CRC) {
          // todo: check elements[1],...
          EmitError(ConsumeRange(selcomp._name_range), "component relation constraint expected");
          return nullptr;
        }

        for (int i = 0; i < constr->el_count; i++) {
          const auto* innerconstr = constr->elements[i];
          std::println(stderr, " >> constraint({}): type={}, valuetype={} ::::: '{}'", i,
                       magic_enum::enum_name(innerconstr->type), magic_enum::enum_name(innerconstr->value->type), [&] {
                         std::string buf;
                         for (int j = 0; j < innerconstr->value->value.reference->comp_count; j++) {
                           buf += innerconstr->value->value.reference->components[j].name;
                           buf += " .. ";
                         }
                         return buf;
                       }());
        }

        break;
      }
      default:
        EmitError(ConsumeRange(selcomp._name_range), "unsupported field expression type");
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
    return NewNode<ttcn_ast::nodes::ValueDecl>([&](ttcn_ast::nodes::ValueDecl& m) {
      if (expr->expr_type == A1TC_REFERENCE) {
        m.type = TransformTypeExpr(expr);
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

  ttcn_ast::nodes::SubTypeDecl* TransformSubTypeDecl(ttcn_ast::TokenKind kind, const asn1p_expr_t* expr) {
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
    errors_.emplace_back(TransformedAsn1Ast::Error{
        .range = std::move(range),
        .message = std::move(message),
    });
  }

  ttcn_ast::Range ConsumeRange(const asn1p_src_range_t& range) {
    ttcn_ast::Range ttcn_range{.begin = range.begin, .end = range.end};

    auto token = ttcn_range.String(adjusted_src_);
    std::replace(adjusted_src_.begin() + ttcn_range.begin, adjusted_src_.begin() + ttcn_range.end, '-', '_');

    if (ttcn_ast::parser::IsKeyword(token)) {
      adjusted_src_[ttcn_range.end] = '_';
      ++ttcn_range.end;
    }

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

    appended_ranges_.emplace(std::move(s), range);

    return range;
  }

  ttcn_ast::Node* last_node_{nullptr};

  const asn1p_t* ast_;
  std::string adjusted_src_;
  std::string_view original_src_;
  std::vector<TransformedAsn1Ast::Error> errors_;
  lib::Arena& arena_;
  Asn1pModuleProvider get_module_;

  //

  static const asn1p_expr_t* ResolveModuleMember(const asn1p_module_t* mod, const char* member_name) {
    const auto* expr = (asn1p_expr_t*)genhash_get(mod->members_hash, member_name);

    std::println(stderr, " -> Resolve({})", member_name);
    if (expr) {
      std::println(stderr, " &&& '{}': {} / {}", expr->Identifier ? expr->Identifier : "((EMPTY))",
                   magic_enum::enum_name(expr->meta_type), magic_enum::enum_name(expr->expr_type));
    } else {
      std::println(stderr, " &&& FAILED to resolve");
    }

    return expr;
  }
  static const asn1p_expr_t* ResolveExprMember(const asn1p_expr_t* expr, const char* member_name) {
    asn1p_expr_t* child;
    TQ_FOR(child, &(expr->members), next) {
      if (child->Identifier && std::strcmp(child->Identifier, member_name) == 0) {
        return child;
      }
    }

    return nullptr;
  }

  static ttcn_ast::Token Tok(ttcn_ast::TokenKind kind) {
    return {.kind = kind, .range = {}};
  }
};

TransformedAsn1Ast TransformAsn1Ast(const asn1p_t* ast, std::string_view src, lib::Arena& arena,
                                    Asn1pModuleProvider module_provider) {
  return AstTransformer(ast, src, arena, std::move(module_provider)).Transform();
}

}  // namespace vanadium::asn1::ast
