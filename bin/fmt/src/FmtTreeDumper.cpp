#include "vanadium/bin/fmt/FmtTreeDumper.h"

#include <ostream>
#include <ranges>
#include <string>
#include <string_view>

#include <magic_enum/magic_enum.hpp>

#include <vanadium/lib/AnsiColors.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/Metaprogramming.h>

#include "../include/vanadium/format/AstSerializer.h"  // TODO(?): move to the interface

namespace vanadium::bin::fmt {

namespace {
class SerializedTreeDumper {
 public:
  SerializedTreeDumper(std::ostream& out) : out_(out) {};

  void Dump(const ast::AST& ast, const ast::Node* n) {
    indent_ = 0;

    lib::Arena arena;
    DumpUnit(format::SerializeAst(ast, n, arena));
    out_ << "\n";
  }

 private:
  void WriteKey(std::string_view name) {
    out_ << "\n";

    out_ << ansicolors::kWhite;
    for (std::size_t i = 0; i < indent_; i++) {
      out_ << "·  ";
    }
    out_ << ansicolors::kReset;
    out_ << ansicolors::kBrightGray << name << ": " << ansicolors::kReset;
  }
  void WriteValue(std::string_view value) {
    out_ << ansicolors::kBrightCyan << value << ansicolors::kReset;
  }

  void DumpUnit(const format::Unit& u) {
    const auto visitor = mp::Overloaded{
        [&](const format::Sequence* seq) -> void {
          out_ << "Sequence";
          if (seq->attributes != format::Sequence::Attribute::kNone) {
            out_ << "(";
            WriteValue(magic_enum::enum_name(seq->attributes));
            out_ << ")";
          }
          out_ << ": ";
          ++indent_;
          for (const auto& [idx, child] : seq->units | std::views::enumerate) {
            WriteKey(std::format("[{}]", idx));
            DumpUnit(child);
          }
          --indent_;
        },
        [&](format::PrintDirective pd) -> void {
          out_ << "PrintDirective::";
          WriteValue(magic_enum::enum_name(pd));
        },
        [&](std::string_view text) -> void {
          WriteValue(text);
        },
        [&](const format::Comment& comment) -> void {
          out_ << "Comment: ";
          WriteValue(comment.content);
        },
        [&](const format::PreferredNewlines& pnl) -> void {
          out_ << "PreferredNewlines(";
          WriteValue(std::to_string(pnl.count));
          out_ << ")";
        },
        [&](format::EmptyUnit) -> void {
          out_ << "Empty";
        },
    };
    std::visit(visitor, u);
  }

  std::ostream& out_;
  std::size_t indent_{0};
};
}  // namespace

void DumpSerializedTree(std::ostream& out, const ast::AST& ast, const ast::Node* n) {
  SerializedTreeDumper(out).Dump(ast, n);
}

}  // namespace vanadium::bin::fmt
