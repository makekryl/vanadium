#pragma once

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTDumper.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/defs/magic_enum_defs.h>

#include <cassert>
#include <cmath>
#include <format>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <ostream>
#include <ranges>
#include <string_view>

#include "vanadium/bin/astdump/AsciiColors.h"

class TextASTDumper {
 private:
  TextASTDumper(vanadium::ast::AST& ast, std::ostream& out) : ast_(ast), out_(out) {}

  void WriteIndent() {
    out_ << asciicolors::kWhite;
    for (std::size_t i = 0; i < indent_; i++) {
      out_ << "·  ";
    }
    out_ << asciicolors::kReset;
  }

  void WriteName(std::string_view name, std::optional<vanadium::ast::Range> range = std::nullopt) {
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
    padding = std::min(padding, 128UL);
    for (std::size_t i = 0; i < padding; i++) {
      out_ << ' ';
    }

    WriteIndent();
    out_ << asciicolors::kBrightGray << name << ": " << asciicolors::kReset;
  }

  void WriteValue(std::string_view value) {
    if (value.length() > 8192) {
      out_ << asciicolors::kRed << value << asciicolors::kReset;
      return;
    }
    out_ << asciicolors::kBrightCyan << value << asciicolors::kReset;
  }

  template <typename F>
  void Indented(F f) {
    ++indent_;
    f();
    --indent_;
  }

  std::size_t indent_{0};

  vanadium::ast::AST& ast_;
  std::ostream& out_;

  friend class vanadium::ast::Dumper<TextASTDumper>;
  vanadium::ast::Dumper<TextASTDumper>* dumper_;

 public:
  void Dump() {
    dumper_->Dump(ast_);
    out_ << '\n';
  }

  static vanadium::ast::Dumper<TextASTDumper> Create(vanadium::ast::AST& ast, std::ostream& out) {
    return {TextASTDumper{ast, out}};
  }
};

template <>
inline void vanadium::ast::Dumper<TextASTDumper>::Dump(std::string_view name, const vanadium::ast::Token& tok) {
  impl_.WriteName(name, tok.range);
  if (tok.range.Length() > 0) {
    impl_.WriteValue(tok.On(impl_.ast_.src));
  } else {
    impl_.WriteValue(std::format("::{}", magic_enum::enum_name(TokenKind(tok.kind)), int(tok.kind)));
  }
}

template <>
inline void vanadium::ast::Dumper<TextASTDumper>::DumpIdent(const vanadium::ast::nodes::Ident& ident) {
  impl_.WriteValue(ident.On(impl_.ast_.src));
}

template <>
inline void vanadium::ast::Dumper<TextASTDumper>::Dump(std::string_view name,
                                                       const vanadium::ast::nodes::Ident& ident) {
  impl_.WriteName(name, ident.nrange);
  DumpIdent(ident);
}

template <>
template <typename T>
inline void vanadium::ast::Dumper<TextASTDumper>::Dump(std::string_view name, const std::vector<T>& v) {
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
inline void vanadium::ast::Dumper<TextASTDumper>::DumpPresence(std::string_view name) {
  impl_.WriteName(name);
  impl_.WriteValue("true");
}

template <>
inline void vanadium::ast::Dumper<TextASTDumper>::DumpKey(std::string_view name, const Range& range) {
  impl_.WriteName(name, range);
}

template <>
template <typename F>
inline void vanadium::ast::Dumper<TextASTDumper>::DumpGroup(std::string_view name, F f) {
  impl_.out_ << name;
  impl_.Indented([&] {
    f();
  });
}
