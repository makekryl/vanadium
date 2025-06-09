#pragma once

#include <cassert>
#include <string_view>

#include "AST.h"
#include "ASTNodes.h"

namespace vanadium::core {
namespace ast {

// NOLINTBEGIN(misc-no-recursion)
template <class Impl>
class Dumper {
 private:
  Dumper(Impl impl) : impl_(std::move(impl)) {}

  void Dump(std::string_view name, const vanadium::core::ast::Token& tok);

  void DumpIdent(const vanadium::core::ast::nodes::Ident&) {}
  void Dump(std::string_view, const vanadium::core::ast::nodes::Ident&) {}

  template <typename T>
  void Dump(std::string_view name, const std::vector<T>& v);

  void DumpPresence(std::string_view);

  void DumpKey(std::string_view);

  template <typename F>
  void DumpGroup(std::string_view name, F f);

  template <typename T>
  void Dump(std::string_view name, const std::optional<T>& opt) {
    if (opt.has_value()) {
      Dump(name, *opt);
    }
  }
  void Dump(std::string_view name, const vanadium::core::ast::Token* tok) {
    if (tok != nullptr) {
      Dump(name, *tok);
    }
  }
  void Dump(std::string_view name, bool b) {
    if (b) {
      DumpPresence(name);
    }
  }

  void Dump(std::string_view name, const vanadium::core::ast::Node* n) {
    if (n == nullptr) {
      return;
    }

    DumpKey(name);

    using namespace vanadium::core::ast;
    switch (n->nkind) {
      case NodeKind::Ident: {
        DumpIdent(*n->As<nodes::Ident>());
        break;
      }
      case NodeKind::DeclStmt: {
        DumpGroup("DeclStmt", [&] {
          Dump("decl", n->As<nodes::DeclStmt>()->decl);
        });
        break;
      }
      case NodeKind::ExprStmt: {
        DumpGroup("ExprStmt", [&] {
          Dump("expr", n->As<nodes::ExprStmt>()->expr);
        });
        break;
      }
#include "gen/Dumper.inc"
      default:
        assert(false && "Unhandled node");
        break;
    }
  }

  void Dump(AST& ast) {
    Dump("Root", ast.root.nodes);
  }

  Impl impl_;
  friend Impl;

 public:
  template <typename F>
  void Apply(F f) {
    impl_.dumper_ = this;
    f(impl_);
  }
};
// NOLINTEND(misc-no-recursion)

}  // namespace ast
}  // namespace vanadium::core
