#pragma once

#include <cassert>
#include <string_view>

#include "vanadium/ast/AST.h"
#include "vanadium/ast/ASTNodes.h"
#include "vanadium/ast/ASTTypes.h"

namespace vanadium::ast {

// NOLINTBEGIN(misc-no-recursion)
template <class Impl>
class Dumper {
 private:
  Dumper(Impl impl) : impl_(std::move(impl)) {}

  void Dump(std::string_view name, const vanadium::ast::Token& tok);

  void DumpIdent(const vanadium::ast::nodes::Ident&) {}
  void Dump(std::string_view, const vanadium::ast::nodes::Ident&) {}

  template <typename T>
  void Dump(std::string_view name, const std::vector<T>& v);

  void DumpPresence(std::string_view);

  void DumpKey(std::string_view, const Range&);

  template <typename F>
  void DumpGroup(std::string_view name, F f);

  template <typename T>
  void Dump(std::string_view name, const std::optional<T>& opt) {
    if (opt.has_value()) {
      Dump(name, *opt);
    }
  }
  void Dump(std::string_view name, const vanadium::ast::Token* tok) {
    if (tok != nullptr) {
      Dump(name, *tok);
    }
  }
  void Dump(std::string_view name, bool b) {
    if (b) {
      DumpPresence(name);
    }
  }

  void Dump(std::string_view name, const vanadium::ast::Node* n) {
    if (n == nullptr) {
      return;
    }

    DumpKey(name, n->nrange);

    using namespace vanadium::ast;
    switch (n->nkind) {
      case NodeKind::RootNode: {
        DumpGroup("RootNode", [&] {
          Dump("nodes", n->As<RootNode>()->nodes);
        });
        break;
      }
      case NodeKind::Ident: {
        DumpIdent(*n->As<nodes::Ident>());
        break;
      }
      case NodeKind::CompositeIdent: {
        DumpGroup("CompositeIdent", [&] {
          Dump("tok1", n->As<nodes::CompositeIdent>()->tok1);
          Dump("tok2", n->As<nodes::CompositeIdent>()->tok2);
        });
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
      case NodeKind::ErrorNode: {
        DumpGroup("ErrorNode", [&] {});
        break;
      }
#include "vanadium/ast/gen/ASTDumper.inc"
      default:
        assert(false && "Unhandled node");
        break;
    }
  }

  void Dump(AST& ast) {
    Dump("", ast.root);
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

}  // namespace vanadium::ast
