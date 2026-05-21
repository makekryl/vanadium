#include "vanadium/compiler/ErrorFormatter.h"

#include <string_view>

#include <magic_enum/magic_enum.hpp>

#include <vanadium/lib/ColoredFmt.h>

#include "vanadium/ast/ASTTypes.h"

namespace vanadium::compiler {

void FormatError(std::string& buf, const core::SourceFile& sf, std::string_view path, const ast::Range& range,
                 std::string_view message) {
  const auto loc_begin = sf.ast.lines.Translate(range.begin);
  const auto loc_end = sf.ast.lines.Translate(range.end);

  cfmt::format_to(std::back_inserter(buf), cfmt::emphasis::bold, "{}:{}:{}: ", path, loc_begin.line + 1,
                  loc_begin.column + 1);
  cfmt::format_to(std::back_inserter(buf), cfmt::emphasis::bold | cfmt::fg(cfmt::terminal_color::red), "error: ");
  buf += message;
  buf += '\n';

  const auto append_source_line = [&](std::size_t line_no, std::string_view line, std::size_t marker_begin,
                                      std::size_t marker_end) {
    cfmt::format_to(std::back_inserter(buf), cfmt::emphasis::faint, " {:6} | ", line_no + 1);
    buf += line;
    buf += '\n';

    cfmt::format_to(std::back_inserter(buf), cfmt::emphasis::faint, " {:6} | ", "");
    buf += cfmt::detail::make_foreground_color<char>(cfmt::terminal_color::cyan);
    for (std::size_t i = 0; i < line.size(); ++i) {
      char ch = ' ';
      if (marker_begin == marker_end) {
        if (i == marker_begin) {
          ch = '^';
        }
      } else if (i >= marker_begin && i < marker_end) {
        ch = '~';
      }
      buf += ch;
    }
    cfmt::detail::reset_color(std::back_inserter(buf));
    buf += '\n';
  };

  if (loc_begin.line == loc_end.line) {
    const auto line = sf.ast.lines.RangeOf(loc_begin.line).String(sf.src);
    append_source_line(loc_begin.line, line, loc_begin.column, std::max(loc_begin.column + 1, loc_end.column));
    return;
  }

  for (std::size_t line_no = loc_begin.line; line_no <= loc_end.line; ++line_no) {
    const auto line = sf.ast.lines.RangeOf(line_no).String(sf.src);
    const auto begin = line_no == loc_begin.line ? loc_begin.column : 0;
    const auto end = line_no == loc_end.line ? loc_end.column : line.size();
    append_source_line(line_no, line, begin, end);
  }
}

std::size_t PrintErrors(std::string& buf, const core::SourceFile& sf, std::string_view path,
                        lib::Consumer<const std::string&> flush) {
  const auto push_err = [&](const ast::Range& range, std::string_view message) {
    buf.clear();
    FormatError(buf, sf, path, range, message);
    flush(buf);
  };

  std::size_t count = 0;
  //
  for (const auto& err : sf.ast.errors) {
    push_err(err.range, err.description);
  }
  count += sf.ast.errors.size();
  //
  for (const auto& err : sf.semantic_errors) {
    push_err(err.range, magic_enum::enum_name(err.type));
  }
  count += sf.semantic_errors.size();
  //
  for (const auto& ident : sf.module->unresolved) {
    // TODO: format to pre-allocated buffer
    push_err(ident->nrange, std::format("unknown symbol '{}'", sf.Text(ident)));
  }
  count += sf.module->unresolved.size();
  //
  for (const auto& err : sf.type_errors) {
    push_err(err.range, err.message);
  }
  count += sf.type_errors.size();

  return count;
}

}  // namespace vanadium::compiler
