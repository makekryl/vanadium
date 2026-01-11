#include "vanadium/asn1/ast/Asn1AstTransformer.h"

#include <asn1c/libasn1parser/asn1parser_cxx.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>

#include <print>

#include "asn1c/libasn1parser/asn1p_expr.h"

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
      m.name.emplace();
      m.name->nrange = {.begin = 0, .end = 5};

      asn1p_expr_t* tc;
      TQ_FOR(tc, &(mod->members), next) {
        if (auto* def_node = TransformExpr(tc); def_node != nullptr) {
          m.defs.push_back(NewNode<ttcn3_ast::nodes::Definition>([&](ttcn3_ast::nodes::Definition& def) {
            def.def = def_node;
            def_node->parent = &def;
          }));
        }
      }
    });
  }

  ttcn3_ast::Node* TransformExpr(const asn1p_expr_t* expr) {
    switch (expr->expr_type) {
      case ASN_CONSTR_SEQUENCE:
        return TransformSequence(expr);
      default:
        return nullptr;
    }
  }

  ttcn3_ast::nodes::StructTypeDecl* TransformSequence(const asn1p_expr_t* expr) {
    return NewNode<ttcn3_ast::nodes::StructTypeDecl>([&](ttcn3_ast::nodes::StructTypeDecl& m) {
      m.kind = Tok(ttcn3_ast::TokenKind::RECORD);

      m.name.emplace();
      m.name->nrange = ConvertRange(expr->_Identifier_Range);
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
