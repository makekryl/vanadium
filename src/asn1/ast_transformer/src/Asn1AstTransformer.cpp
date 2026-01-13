#include "vanadium/asn1/ast/Asn1AstTransformer.h"

#include <asn1c/libasn1parser/asn1parser_cxx.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/Scanner.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>
#include <vanadium/lib/StaticMap.h>

#include <optional>
#include <print>
#include <string_view>

#include "asn1c/libasn1parser/asn1p_expr.h"
#include "magic_enum/magic_enum.hpp"

namespace vanadium::asn1::ast {

namespace {
constexpr auto kBuiltinTypeMapping = lib::MakeStaticMap<asn1p_expr_type_e, std::string_view>({
    {ASN_BASIC_NULL, "NULL"},
    {ASN_BASIC_OBJECT_IDENTIFIER, "objid"},
    {ASN_BASIC_INTEGER, "integer"},
    {ASN_BASIC_REAL, "float"},
    {ASN_BASIC_BOOLEAN, "boolean"},
    {ASN_BASIC_BIT_STRING, "bitstring"},
    {ASN_BASIC_OCTET_STRING, "octetstring"},
    {ASN_BASIC_CHARACTER_STRING, "charstring"},
});
}

class AstTransformer {
 public:
  AstTransformer(const asn1p_t* ast, std::string_view src, lib::Arena& arena)
      : ast_(ast), original_src_(src), arena_(arena) {
    adjusted_src_.reserve(original_src_.length() + 256);
    adjusted_src_.append(original_src_);
  }

  TransformedAsn1Ast Transform() {
    auto* root = TransformRoot();
    return {
        .adjusted_src = *arena_.Alloc<std::string>(std::move(adjusted_src_)),
        .root = root,
        .errors = {},
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
    std::println(stderr, "[OUTER] '{}': {} / {}", expr->Identifier, magic_enum::enum_name(expr->meta_type),
                 magic_enum::enum_name(expr->expr_type));
    switch (expr->meta_type) {
      case AMT_VALUE:
        return TransformValueDeclaration(expr);
      default:
        return TransformExpr<true>(expr);
    }
  }

  template <bool Outermost = false>
  ttcn_ast::Node* TransformExpr(const asn1p_expr_t* expr) {
    std::println(stderr, "'{}': {} / {}", expr->Identifier, magic_enum::enum_name(expr->meta_type),
                 magic_enum::enum_name(expr->expr_type));
    switch (expr->expr_type) {
#define TRANSFORM_STRUCT_CASE(ASN1C_EXPR_TYPE, TTCN_STRUCT_KIND)                \
  case ASN1C_EXPR_TYPE:                                                         \
    if constexpr (Outermost) {                                                  \
      return TransformStruct(ttcn_ast::TokenKind::TTCN_STRUCT_KIND, expr);      \
    } else {                                                                    \
      return TransformInnerStruct(ttcn_ast::TokenKind::TTCN_STRUCT_KIND, expr); \
    }
      TRANSFORM_STRUCT_CASE(ASN_CONSTR_SEQUENCE, RECORD)
      TRANSFORM_STRUCT_CASE(ASN_CONSTR_SET, SET)
      TRANSFORM_STRUCT_CASE(ASN_CONSTR_CHOICE, UNION)
#undef TRANSFORM_STRUCT_CASE

      case A1TC_REFERENCE:
        return NewNode<ttcn_ast::nodes::RefSpec>([&](ttcn_ast::nodes::RefSpec& m) {
          m.x = TransformTypeExpr(expr->reference);
        });

      default:
        return NewNode<ttcn_ast::nodes::RefSpec>([&](ttcn_ast::nodes::RefSpec& m) {
          m.x = TransformBuiltinTypeExpr(expr);
        });
    }
  }

  ttcn_ast::nodes::RefSpec* TransformTypeReference(const asn1p_ref_t* ref) {
    return NewNode<ttcn_ast::nodes::RefSpec>([&](ttcn_ast::nodes::RefSpec& m) {
      m.x = TransformTypeExpr(ref);
    });
  }

  ttcn_ast::nodes::Expr* TransformTypeExpr(const asn1p_ref_t* ref) {
    if (ref->comp_count == 1) {
      return NewNode<ttcn_ast::nodes::Ident>([&](ttcn_ast::nodes::Ident& ident) {
        ident.nrange = ConsumeRange(ref->components[0]._name_range);
      });
    }

    auto* se = NewNode<ttcn_ast::nodes::SelectorExpr>([&](ttcn_ast::nodes::SelectorExpr& m) {

    });
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
        m.type = TransformTypeExpr(expr->reference);
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
        if (auto* dn = TransformComponent(se); dn != nullptr) {
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
        if (auto* dn = TransformComponent(se); dn != nullptr) {
          m.fields.push_back(dn);
        }
      }
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

  static ttcn_ast::Token Tok(ttcn_ast::TokenKind kind) {
    return {.kind = kind, .range = {}};
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
  lib::Arena& arena_;
};

TransformedAsn1Ast TransformAsn1Ast(const asn1p_t* ast, std::string_view src, lib::Arena& arena) {
  return AstTransformer(ast, src, arena).Transform();
}

}  // namespace vanadium::asn1::ast
