#include "vanadium/format/AstPrinter.h"

#include <cassert>
#include <ranges>
#include <string_view>
#include <utility>

#include <vanadium/ast/AST.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/Metaprogramming.h>

#include "vanadium/format/AstSerializer.h"

namespace vanadium::format {

namespace {

bool IsLineComment(const Comment& comment) {
  return comment.content.starts_with("//");
}

class SerialAstPrinter {
 public:
  SerialAstPrinter(std::string_view src, PrintOptions options) : src_(src), options_(std::move(options)) {}

  std::string Print(const Unit& u) {
    AppendUnit(u, Wrap::kAuto);
    return std::move(buf_);
  }

 private:
  enum class Wrap : std::uint8_t {
    kAuto,
    kEnabled,
  };

  [[nodiscard]] std::size_t Width(const Sequence&);
  void AppendUnit(const Unit&, Wrap);

  std::size_t consecutive_newlines_{0};
  void WriteNewline() {
    buf_ += "\n";

    const auto sl = (indent_ * options_.tab_width);
    for (std::size_t i = 0; i < sl; ++i) {
      buf_ += ' ';
    }
    line_length_ = sl;

    ++consecutive_newlines_;
  }
  void Write(std::string_view s) {
    buf_ += s;
    line_length_ += s.length();

    consecutive_newlines_ = 0;
  }

  [[nodiscard]] bool IsCurrentLineEmpty() const {
    return consecutive_newlines_ > 0;
  }

  std::string buf_;

  std::size_t indent_{0};
  std::size_t line_length_{0};

  std::string_view src_;
  const PrintOptions options_;
};

std::size_t SerialAstPrinter::Width(const Sequence& seq) {
  const auto visitor = mp::Overloaded{
      [&](const Sequence* seq) {
        return Width(*seq);
      },
      [&](PrintDirective pd) {
        switch (pd) {
          case PrintDirective::kSpace:
          case PrintDirective::kSpaceOrLine:
            return 1UL;
          case PrintDirective::kHardLine:
            return options_.print_width;
          case PrintDirective::kSoftLine:
            return 0UL;
          case PrintDirective::kSemicolon:
            return 1UL;
        }
      },
      [&](std::string_view text) {
        return text.length();
      },
      [&](const Comment& comment) {
        return IsLineComment(comment) ? options_.print_width : comment.content.length();
      },
      [&](const PreferredNewlines&) {
        return options_.print_width;
      },
      [&](EmptyUnit) {
        return 0UL;
      },
  };

  std::size_t width = 0;
  for (const auto& u : seq.units) {
    width += std::visit(visitor, u);
  }
  return width;
}

void SerialAstPrinter::AppendUnit(const Unit& cu, Wrap wrap) {
  const auto visitor = mp::Overloaded{
      [&](const Sequence* seq) -> void {
        if (seq->attributes & Sequence::Attribute::kIndented) {
          ++indent_;
        }

        Wrap children_wrap = wrap;
        if (children_wrap == Wrap::kAuto && (seq->attributes & Sequence::Attribute::kGrouped)) {
          const auto width = Width(*seq);
          if (width > options_.print_width) {
            children_wrap = Wrap::kEnabled;
          }
        }

        for (const auto& u : seq->units) {
          AppendUnit(u, children_wrap);
        }

        if (seq->attributes & Sequence::Attribute::kIndented) {
          --indent_;
        }
      },
      [&](PrintDirective pd) -> void {
        switch (pd) {
          case PrintDirective::kSpace: {
            Write(" ");
            break;
          }
          case PrintDirective::kSpaceOrLine: {
            if (wrap == Wrap::kEnabled) {
              WriteNewline();
            } else {
              Write(" ");
            }
            break;
          }
          case PrintDirective::kHardLine: {
            WriteNewline();
            break;
          }
          case PrintDirective::kSoftLine: {
            if (wrap == Wrap::kEnabled) {
              WriteNewline();
            }
            break;
          }
          case PrintDirective::kSemicolon: {
            Write(";");
            break;
          }
        }
      },
      [&](std::string_view text) -> void {
        Write(text);
      },
      [&](const Comment& comment) -> void {
        if (IsLineComment(comment)) {
          if (!IsCurrentLineEmpty()) {
            Write("  ");
          }
          Write(comment.content);
        } else {
          if (!IsCurrentLineEmpty()) {
            Write(" ");
          }
          auto lines = comment.content | std::views::split('\n');
          auto it = lines.begin();
          auto end = lines.end();
          while (it != end) {
            std::string_view line{*(it++)};
            while (!line.empty() && std::isspace(line.front())) {
              line.remove_prefix(1);
            }
            while (!line.empty() && std::isspace(line.back())) {
              line.remove_suffix(1);
            }
            if (line.starts_with('*')) {
              Write(" ");
            }
            Write(line);
            if (it != end) {
              WriteNewline();
            }
          }
        }
      },
      [&](const PreferredNewlines& pnl) -> void {
        auto count = pnl.count;
        if (IsCurrentLineEmpty()) {
          --count;
        }
        count = std::min(count, options_.max_newlines);
        for (std::size_t i = 0; i < count; ++i) {
          WriteNewline();
        }
      },
      [&](EmptyUnit) -> void {},
  };
  std::visit(visitor, cu);
}
}  // namespace

std::string PrintAst(const ast::AST& ast, const ast::Node* n, PrintOptions options) {
  lib::Arena arena;  // todo: accept as an argument
  auto unit = SerializeAst(ast, n, arena);

  return SerialAstPrinter(ast.src, options).Print(unit);
}

}  // namespace vanadium::format
