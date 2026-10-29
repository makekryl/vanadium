#pragma once

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/ScopedValue.h>

#include "vanadium/ast/ASTTypes.h"

namespace vanadium::core {

class AstCoBuilder {
 public:
  AstCoBuilder(lib::Arena& arena, const ast::Node* insertion_point) : arena_{arena}, last_node_{insertion_point} {}

  void NewIdent(ast::nodes::Ident& ident, const ast::Range& range) {
    ident.nrange = range;
    ident.parent = last_node_;
  }

  ast::nodes::Ident* NewIdent(const ast::Range& range) {
    return NewNode<ast::nodes::Ident>([&](auto& ident) {
      ident.nrange = range;
    });
  }

  template <ast::IsNode T, typename Initializer>
    requires std::is_invocable_v<Initializer, T&>
  T* NewNode(Initializer f) {
    auto* p = arena_.Alloc<T>();
    p->parent = last_node_;
    p->nrange = last_node_->nrange;
    {
      lib::ScopedValue<const ast::Node*> guard(last_node_, p);
      f(*p);
    }
    return p;
  }

 private:
  lib::Arena& arena_;
  const ast::Node* last_node_;
};

}  // namespace vanadium::core
