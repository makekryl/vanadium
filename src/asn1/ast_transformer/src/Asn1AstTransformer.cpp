#include "vanadium/asn1/ast/Asn1AstTransformer.h"

#include <asn1c/libasn1parser/asn1parser_cxx.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>

#include <optional>
#include <print>

#include "asn1c/libasn1parser/asn1p_expr.h"
#include "magic_enum/magic_enum.hpp"

namespace vanadium::asn1::ast {

class AstTransformer {
 public:
  AstTransformer(const asn1p_t* ast, std::string_view src, lib::Arena& arena) : ast_(ast), src_(src), arena_(arena) {}

  TransformedAsn1Ast Transform() {
    auto* root = TransformRoot();

    const auto source_len = src_.length();
    auto new_src = arena_.AllocStringBuffer(source_len + 1 + CalcVerSequenceRange(max_ver_ + 1).begin);
    new_src[source_len] = 0;
    std::ranges::copy(src_, new_src.begin());

    auto insertion_point = new_src.begin() + source_len + 1;
    //
    for (std::uint32_t i = 1; i <= max_ver_; i++) {
      std::ranges::copy(kVerPrefix, insertion_point);
      insertion_point += kVerPrefixSize;

      const auto result = std::to_chars(insertion_point.base(), new_src.end().base(), i);
      const auto length = result.ptr - insertion_point.base();
      insertion_point += length;
    }

    return {
        .adjusted_src = std::string_view{new_src},
        .root = root,
        .errors = {},
    };
  }

 private:
  ttcn3_ast::RootNode* TransformRoot() {
    const auto* mod = TQ_FIRST(&(ast_->modules));
    return NewNode<ttcn3_ast::RootNode>([&](ttcn3_ast::RootNode& m) {
      m.nrange = {.begin = 0, .end = static_cast<ttcn3_ast::pos_t>(src_.length())};
      m.nodes.push_back(TransformModule(mod));
    });
  }

  ttcn3_ast::nodes::Module* TransformModule(const asn1p_module_t* mod) {
    return NewNode<ttcn3_ast::nodes::Module>([&](ttcn3_ast::nodes::Module& m) {
      m.nrange = {.begin = 0, .end = static_cast<ttcn3_ast::pos_t>(src_.length())};
      m.name.emplace().nrange = {.begin = 0, .end = 5};

      asn1p_expr_t* tc;
      TQ_FOR(tc, &(mod->members), next) {
        if (auto* dn = TransformExpr<true>(tc); dn != nullptr) {
          m.defs.push_back(NewNode<ttcn3_ast::nodes::Definition>([&](ttcn3_ast::nodes::Definition& def) {
            def.def = dn;
            dn->parent = &def;
          }));
        }
      }
    });
  }

  template <bool Outermost = false>
  ttcn3_ast::Node* TransformExpr(const asn1p_expr_t* expr) {
    std::println("'{}': {} / {}", expr->Identifier, magic_enum::enum_name(expr->meta_type),
                 magic_enum::enum_name(expr->expr_type));
    switch (expr->expr_type) {
      case ASN_CONSTR_SEQUENCE:
        if constexpr (Outermost) {
          return TransformSequence(ttcn3_ast::TokenKind::RECORD, expr);
        } else {
          return TransformInnerSequence(ttcn3_ast::TokenKind::RECORD, expr);
        }
      case ASN_CONSTR_CHOICE:
        if constexpr (Outermost) {
          return TransformSequence(ttcn3_ast::TokenKind::UNION, expr);
        } else {
          return TransformInnerSequence(ttcn3_ast::TokenKind::UNION, expr);
        }
      case ASN_BASIC_INTEGER:
        return TransformBuiltinTypeRef(expr);
      default:
        return nullptr;
    }
  }

  ttcn3_ast::nodes::StructTypeDecl* TransformSequence(ttcn3_ast::TokenKind kind, const asn1p_expr_t* expr) {
    return NewNode<ttcn3_ast::nodes::StructTypeDecl>([&](ttcn3_ast::nodes::StructTypeDecl& m) {
      m.kind = Tok(kind);

      m.name.emplace().nrange = ConvertRange(expr->_Identifier_Range);

      asn1p_expr_t* se;
      TQ_FOR(se, &(expr->members), next) {
        if (auto* dn = TransformComponent(se); dn != nullptr) {
          m.fields.push_back(dn);
        }
      }
    });
  }

  ttcn3_ast::nodes::StructSpec* TransformInnerSequence(ttcn3_ast::TokenKind kind, const asn1p_expr_t* expr) {
    return NewNode<ttcn3_ast::nodes::StructSpec>([&](ttcn3_ast::nodes::StructSpec& m) {
      m.kind = Tok(ttcn3_ast::TokenKind::RECORD);

      asn1p_expr_t* se;
      TQ_FOR(se, &(expr->members), next) {
        if (auto* dn = TransformComponent(se); dn != nullptr) {
          m.fields.push_back(dn);
        }
      }
    });
  }

  ttcn3_ast::nodes::Field* TransformComponent(const asn1p_expr_t* se) {
    auto* c = TransformExpr(se);
    if (!c) {
      return nullptr;
    }

    return NewNode<ttcn3_ast::nodes::Field>([&](ttcn3_ast::nodes::Field& m) {
      m.name.emplace().nrange = ConvertRange(se->_Identifier_Range);

      if ((se->marker.flags & asn1p_expr_s::asn1p_expr_marker_s::EM_DEFAULT) ==
          asn1p_expr_s::asn1p_expr_marker_s::EM_DEFAULT) {
        // safe_printf(" DEFAULT ");
        // asn1print_value(se->marker.default_value, flags);
      } else if ((se->marker.flags & asn1p_expr_s::asn1p_expr_marker_s::EM_OPTIONAL) ==
                 asn1p_expr_s::asn1p_expr_marker_s::EM_OPTIONAL) {
        m.optional = true;
      }

      m.type = c->As<ttcn3_ast::nodes::TypeSpec>();
    });
  }

  ttcn3_ast::nodes::RefSpec* TransformBuiltinTypeRef(const asn1p_expr_t* se) {
    return NewNode<ttcn3_ast::nodes::RefSpec>([&](ttcn3_ast::nodes::RefSpec& m) {
      m.x = NewNode<ttcn3_ast::nodes::Ident>([&](ttcn3_ast::nodes::Ident& ident) {
        ident.nrange = {1, 5};
      });
    });
  }

  //

  template <ttcn3_ast::IsNode T>
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

  static ttcn3_ast::Token Tok(ttcn3_ast::TokenKind kind) {
    return {.kind = kind, .range = {}};
  }

  static ttcn3_ast::Range ConvertRange(const asn1p_src_range_t& range) {
    return {.begin = range.begin, .end = range.end};
  }

  static constexpr std::string_view kVerPrefix{"ver"};
  static constexpr ttcn3_ast::pos_t kVerPrefixSize{kVerPrefix.size()};
  static ttcn3_ast::Range CalcVerSequenceRange(std::uint32_t N) {
    ttcn3_ast::pos_t pos = 1;

    std::uint32_t power = 1;
    std::uint32_t digits = 1;
    while ((power * 10) <= N) {
      pos += (9 * power) * (kVerPrefixSize + digits);
      power *= 10;
      digits++;
    }
    pos += (N - power) * (kVerPrefixSize + digits);

    return {
        .begin = pos,
        .end = kVerPrefixSize + digits,
    };
  };

  ttcn3_ast::Node* last_node_{nullptr};
  std::uint32_t max_ver_{1};

  const asn1p_t* ast_;
  std::string_view src_;
  lib::Arena& arena_;
};

TransformedAsn1Ast TransformAsn1Ast(const asn1p_t* ast, std::string_view src, lib::Arena& arena) {
  return AstTransformer(ast, src, arena).Transform();
}

}  // namespace vanadium::asn1::ast
