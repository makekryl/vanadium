#pragma once

#include <cstdint>

#include "AST.h"
#include "ASTTypes.h"
#include "LSProtocol.h"

namespace vanadium::ls {

namespace conv {

inline lsp::Position ToLSPPosition(const core::ast::Location& loc) {
  // TODO: UTF-16
  return lsp::Position{
      .line = loc.line,
      .character = loc.column,
  };
}

inline lsp::Position ToLSPosition(const core::ast::AST& ast, core::ast::pos_t pos) {
  const core::ast::pos_t line = ast.lines.LineOf(pos);

  std::uint32_t character{0};
  // for (char c : ast.src.substr(ast.lines.StartOf(line), pos)) {
  //   if (c >= 0xd800 && c <= 0xdbff)
  //     string_pos += 2;
  //   else
  //     ++string_pos;
  // }

  return {
      .line = line,
      .character = character,
  };
}

inline lsp::Range ToLSPRange(const core::ast::Range& range, const core::ast::AST& ast) {
  return lsp::Range{
      .start = ToLSPPosition(ast.lines.Translate(range.begin)),
      .end = ToLSPPosition(ast.lines.Translate(range.end)),
  };
}

}  // namespace conv

}  // namespace vanadium::ls
