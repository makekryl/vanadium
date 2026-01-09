#include <asn1c/libasn1parser/asn1parser_cxx.h>
#include <vanadium/asn1/ast/Asn1cAstWrapper.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>

#include <print>

#include "vanadium/asn1/ast/Asn1ModuleBasket.h"

namespace vanadium::asn1::ast {

class AstTransformer {
 public:
  AstTransformer(const asn1p_t* ast, std::string_view src, lib::Arena& arena) : ast_(ast), src_(src), arena_(arena) {}

  ttcn3_ast::RootNode* Transform() {
    const auto* mod = TQ_FIRST(&(ast_->modules));
    return NewNode<ttcn3_ast::RootNode>([&](ttcn3_ast::RootNode& m) {
      m.nrange = {.begin = 0, .end = 5};
      m.nodes.push_back(TransformModule(mod));
    });
  }

 private:
  ttcn3_ast::nodes::Module* TransformModule(const asn1p_module_t* mod) {
    return NewNode<ttcn3_ast::nodes::Module>([&](ttcn3_ast::nodes::Module& m) {
      m.nrange = {.begin = 0, .end = 5};
      std::println(stderr, " ****** module: '{}'", m.nrange.String(src_));
      m.name.emplace();
      m.name->nrange = {.begin = 0, .end = 5};
    });
  }

  //

  template <ttcn3_ast::IsNode T>
  T* NewNode(std::invocable<T&> auto f) {
    auto* p = arena_.Alloc<T>();
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

  ttcn3_ast::Node* last_node_{nullptr};

  const asn1p_t* ast_;
  std::string_view src_;
  lib::Arena& arena_;
};

ttcn3_ast::AST Asn1ModuleBasket::TransformAST(Asn1ModuleBasketItem& item, lib::Arena& arena) {
  (void)this;
  if (!item.ast) {
    std::string errstr;
    for (auto& err : item.errors) {
      errstr += err.message + ", ";
    }
    std::println(stderr, "bad module: '{}': {}", item.src.substr(0, 20), errstr);
    return {
        .src = item.src,
        .root = arena.Alloc<ttcn3_ast::RootNode>(),
        .lines = item.lines,
        .errors = {},
    };
  }
  auto* root = AstTransformer(item.ast->Get(), item.src, arena).Transform();
  return {
      .src = item.src, .root = root, .lines = item.lines, .errors = {},
      // .errors = item.errors | std::views::transform([](const auto& err) {
      //             return ttcn3_ast::SyntaxError{
      //                 .range = {.begin = err.pos, .end = err.pos},
      //                 .description = err.message,
      //             };
      //           }) |
      //           std::ranges::to<std::vector>(),
  };
}

}  // namespace vanadium::asn1::ast
