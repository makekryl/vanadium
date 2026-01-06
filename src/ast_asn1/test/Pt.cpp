#include <gtest/gtest.h>

#include <cassert>
#include <cmath>
#include <format>
#include <fstream>
#include <iostream>
#include <ostream>
#include <ranges>
#include <string_view>

#include "vanadium/asn1/ast/Asn1AST.h"
#include "vanadium/asn1/ast/Asn1ASTNodes.h"
#include "vanadium/asn1/ast/Asn1ASTTypes.h"
#include "vanadium/asn1/ast/Asn1Parser.h"

namespace vanadium::asn1::ast {

// NOLINTBEGIN(misc-no-recursion)
template <class Impl>
class Dumper {
 private:
  Dumper(Impl impl) : impl_(std::move(impl)) {}

  void Dump(std::string_view name, const Token& tok);

  void DumpIdent(const nodes::Ident&) {}
  void Dump(std::string_view, const nodes::Ident&) {}

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
  void Dump(std::string_view name, const Token* tok) {
    if (tok != nullptr) {
      Dump(name, *tok);
    }
  }
  void Dump(std::string_view name, bool b) {
    if (b) {
      DumpPresence(name);
    }
  }

  void Dump(std::string_view name, const Node* n) {
    if (n == nullptr) {
      return;
    }

    DumpKey(name, n->nrange);

    switch (n->nkind) {
      case NodeKind::RootNode: {
        DumpGroup("RootNode", [&] {
          Dump("nodes", n->As<RootNode>()->nodes);
        });
        break;
      }
#include "vanadium/asn1/ast/gen/ASTDumper.inc"
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
}  // namespace vanadium::asn1::ast

class TextASTDumper {
 private:
  TextASTDumper(vanadium::asn1::ast::AST& ast, std::ostream& out) : ast_(ast), out_(out) {}

  void WriteIndent() {
    out_ << "\e[38;5;255m";
    for (std::size_t i = 0; i < indent_; i++) {
      out_ << "·  ";
    }
    out_ << "\e[0m";
  }

  void WriteName(std::string_view name, std::optional<vanadium::asn1::ast::Range> range = std::nullopt) {
    out_ << "\n";

    std::size_t padding{16};
    if (range) {
      out_ << "[" << range->begin << "; " << range->end << ")";

      padding -= 1;
      padding -= range->begin > 0 ? std::size_t(std::log10(double(range->begin))) + 1 : 1;
      padding -= 2;
      padding -= range->end > 0 ? std::size_t(std::log10(double(range->end))) + 1 : 1;
      padding -= 1;
    }
    for (std::size_t i = 0; i < padding; i++) {
      out_ << ' ';
    }

    WriteIndent();
    out_ << "\e[1;90m" << name << ":\e[0m ";
  }

  void WriteValue(std::string_view value) {
    out_ << "\e[0;96m" << value << "\e[0m";
  }

  template <typename F>
  void Indented(F f) {
    ++indent_;
    f();
    --indent_;
  }

  std::size_t indent_{0};

  vanadium::asn1::ast::AST& ast_;
  std::ostream& out_;

  friend class vanadium::asn1::ast::Dumper<TextASTDumper>;
  vanadium::asn1::ast::Dumper<TextASTDumper>* dumper_;

 public:
  void Dump() {
    dumper_->Dump(ast_);
    out_ << '\n';
  }

  static vanadium::asn1::ast::Dumper<TextASTDumper> Create(vanadium::asn1::ast::AST& ast, std::ostream& out) {
    return {TextASTDumper{ast, out}};
  }
};

template <>
inline void vanadium::asn1::ast::Dumper<TextASTDumper>::Dump(std::string_view name,
                                                             const vanadium::asn1::ast::Token& tok) {
  impl_.WriteName(name, tok.range);
  impl_.WriteValue(tok.On(impl_.ast_.src));
}

template <>
inline void vanadium::asn1::ast::Dumper<TextASTDumper>::DumpIdent(const vanadium::asn1::ast::nodes::Ident& ident) {
  impl_.WriteValue(ident.On(impl_.ast_.src));
}

template <>
inline void vanadium::asn1::ast::Dumper<TextASTDumper>::Dump(std::string_view name,
                                                             const vanadium::asn1::ast::nodes::Ident& ident) {
  impl_.WriteName(name, ident.nrange);
  DumpIdent(ident);
}

template <>
template <typename T>
inline void vanadium::asn1::ast::Dumper<TextASTDumper>::Dump(std::string_view name, const std::vector<T>& v) {
  if (v.empty()) {
    return;
  }

  impl_.WriteName(name);
  impl_.Indented([&] {
    for (auto&& [idx, n] : v | std::views::enumerate) {
      const auto s = std::format("[{}]", idx);
      Dump(s, n);
    }
  });
}

template <>
inline void vanadium::asn1::ast::Dumper<TextASTDumper>::DumpPresence(std::string_view name) {
  impl_.WriteName(name);
  impl_.WriteValue("true");
}

template <>
inline void vanadium::asn1::ast::Dumper<TextASTDumper>::DumpKey(std::string_view name, const Range& range) {
  impl_.WriteName(name, range);
}

template <>
template <typename F>
inline void vanadium::asn1::ast::Dumper<TextASTDumper>::DumpGroup(std::string_view name, F f) {
  impl_.out_ << name;
  impl_.Indented([&] {
    f();
  });
}

namespace {
std::optional<std::string> ReadFile(const char* path) {
  // TODO: err
  if (auto f = std::ifstream(path)) {
    std::stringstream buf;
    buf << f.rdbuf();
    return buf.str();
  }
  return std::nullopt;
}
}  // namespace

TEST(Asn1AST, Dump) {
  const auto src = ReadFile("/workspaces/vanadium/VIRAG.asn");
  if (!src) {
    FAIL();
  }

  const auto parse_fn = vanadium::asn1::ast::Parse;

  vanadium::lib::Arena arena;
  auto ast = parse_fn(arena, *src);

  // TODO: display errors
  if (!ast.errors.empty()) {
    std::cout << std::format(" (!) {} errors\n", ast.errors.size());
    for (const auto& err : ast.errors) {
      std::cout << std::format("{}:{} - {}", err.range.begin, err.range.end, err.description) << std::endl;
    }
  }

  TextASTDumper::Create(ast, std::cout).Apply([](auto& d) {
    d.Dump();
  });
}
