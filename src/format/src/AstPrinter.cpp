#include "vanadium/format/AstPrinter.h"

#include <cassert>
#include <string_view>
#include <utility>

#include <vanadium/lib/Arena.h>
#include <vanadium/lib/Metaprogramming.h>

#include "vanadium/format/AstSerializer.h"

namespace vanadium::format {

namespace {
class SerialAstPrinter {
 public:
  SerialAstPrinter(std::string_view src, PrinterOptions options) : src_(src), options_(std::move(options)) {}

  std::string Print(const Unit& u) {
    AppendUnit(u);
    return std::move(buf_);
  }

 private:
  void AppendUnit(const Unit&);

  void WriteNewline() {
    buf_ += "\n";
    line_length_ = 0;
  }
  void Write(std::string_view s) {
    buf_ += s;
    line_length_ += s.length();
  }

  std::string buf_;

  std::size_t indent_{0};
  std::size_t line_length_{0};

  std::string_view src_;
  const PrinterOptions options_;
};

void SerialAstPrinter::AppendUnit(const Unit& cu) {
  const auto visitor = mp::Overloaded{
      [&](const Sequence* seq) -> void {
        if (seq->attributes & Sequence::Attribute::kIndented) {
          ++indent_;
        }
        for (const auto& u : seq->units) {
          AppendUnit(u);
        }
        if (seq->attributes & Sequence::Attribute::kIndented) {
          --indent_;
        }
      },
      [&](PrintDirective pd) -> void {
        switch (pd) {
          case PrintDirective::kHardLine: {
            WriteNewline();
            break;
          }
          case PrintDirective::kSpaceOrLine: {
            buf_ += " ";
            break;
          }
          default: {
            assert(false && "Unhandled PrintDirective");
            break;
          }
        }
      },
      [&](std::string_view text) -> void {
        if (line_length_ < indent_ * options_.tab_width) {
          const auto sl = (indent_ * options_.tab_width) - line_length_;
          for (std::size_t i = 0; i < sl; ++i) {
            buf_ += ' ';
          }
          line_length_ += sl;
        }
        buf_ += text;
      },
      [&](EmptyUnit) -> void {},
  };
  std::visit(visitor, cu);
}
}  // namespace

std::string PrintAst(std::string_view src, const ast::Node* n, PrinterOptions options) {
  lib::Arena arena;  // todo: accept as an argument
  auto unit = SerializeAst(src, n, arena);

  return SerialAstPrinter(src, options).Print(unit);
}

}  // namespace vanadium::format
