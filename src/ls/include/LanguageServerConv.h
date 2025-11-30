#pragma once

#include "AST.h"
#include "ASTTypes.h"
#include "LSProtocol.h"

namespace vanadium::ls {
namespace conv {

[[nodiscard]] inline lsp::Position ToLSPPosition(const ast::Location& loc) {
  // TODO: UTF-16
  return lsp::Position{
      .line = loc.line,
      .character = loc.column,
  };
}

[[nodiscard]] inline lsp::Range ToLSPRange(const ast::Range& range, const ast::AST& ast) {
  return lsp::Range{
      .start = ToLSPPosition(ast.lines.Translate(range.begin)),
      .end = ToLSPPosition(ast.lines.Translate(range.end)),
  };
}

[[nodiscard]] inline ast::Location FromLSPPosition(const lsp::Position& pos) {
  // TODO: UTF-16
  return ast::Location{
      .line = pos.line,
      .column = pos.character,
  };
}

[[nodiscard]] inline ast::Range FromLSPRange(const lsp::Range& range, const ast::AST& ast) {
  return ast::Range{
      .begin = ast.lines.GetPosition(FromLSPPosition(range.start)),
      .end = ast.lines.GetPosition(FromLSPPosition(range.end)),
  };
}

}  // namespace conv
}  // namespace vanadium::ls
