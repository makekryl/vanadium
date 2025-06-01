#pragma once

#include <cassert>
#include <format>
#include <iostream>
#include <ostream>
#include <ranges>
#include <string_view>

#include "AST.h"
#include "ASTNodes.h"
#include "Dumper.h"

class TextASTDumper {
 private:
  TextASTDumper(vanadium::core::ast::AST& ast, std::ostream& out) : ast_(ast), out_(out) {}

  void WriteLine() {
    out_ << "\n";
    WriteIndent();
  }

  void WriteIndent() {
    out_ << "\e[38;5;255m";
    for (std::size_t i = 0; i < indent_; i++) {
      out_ << "·  ";
    }
    out_ << "\e[0m";
  }

  void WriteName(std::string_view name) {
    WriteLine();
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

  vanadium::core::ast::AST& ast_;
  std::ostream& out_;

  friend class vanadium::core::ast::Dumper<TextASTDumper>;
  vanadium::core::ast::Dumper<TextASTDumper>* dumper_;

 public:
  void Dump() {
    dumper_->Dump(ast_);
    out_ << '\n';
  }

  static vanadium::core::ast::Dumper<TextASTDumper> Create(vanadium::core::ast::AST& ast, std::ostream& out) {
    return {TextASTDumper{ast, out}};
  }
};

template <>
inline void vanadium::core::ast::Dumper<TextASTDumper>::Dump(std::string_view name,
                                                             const vanadium::core::ast::Token& tok) {
  impl_.WriteName(name);
  impl_.WriteValue(tok.On(impl_.ast_.src));
}

template <>
inline void vanadium::core::ast::Dumper<TextASTDumper>::DumpIdent(const vanadium::core::ast::nodes::Ident& ident) {
  impl_.WriteValue(ident.On(impl_.ast_.src));
}

template <>
inline void vanadium::core::ast::Dumper<TextASTDumper>::Dump(std::string_view name,
                                                             const vanadium::core::ast::nodes::Ident& ident) {
  impl_.WriteName(name);
  DumpIdent(ident);
}

template <>
template <typename T>
inline void vanadium::core::ast::Dumper<TextASTDumper>::Dump(std::string_view name, const std::vector<T>& v) {
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
inline void vanadium::core::ast::Dumper<TextASTDumper>::DumpPresence(std::string_view name) {
  impl_.WriteName(name);
  impl_.WriteValue("true");
}

template <>
inline void vanadium::core::ast::Dumper<TextASTDumper>::DumpKey(std::string_view name) {
  impl_.WriteName(name);
}

template <>
template <typename F>
inline void vanadium::core::ast::Dumper<TextASTDumper>::DumpGroup(std::string_view name, F f) {
  impl_.out_ << name;
  impl_.Indented([&] {
    f();
  });
}
