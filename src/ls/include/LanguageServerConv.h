#pragma once

#include "AST.h"
#include "ASTTypes.h"
#include "LSProtocol.h"

namespace vanadium::ls {

namespace conv {

[[nodiscard]] inline lsp::Position ToLSPPosition(const core::ast::Location& loc) {
  // TODO: UTF-16
  return lsp::Position{
      .line = loc.line,
      .character = loc.column,
  };
}

[[nodiscard]] inline lsp::Range ToLSPRange(const core::ast::Range& range, const core::ast::AST& ast) {
  return lsp::Range{
      .start = ToLSPPosition(ast.lines.Translate(range.begin)),
      .end = ToLSPPosition(ast.lines.Translate(range.end)),
  };
}

[[nodiscard]] inline core::ast::Location FromLSPPosition(const lsp::Position& pos) {
  // TODO: UTF-16
  return core::ast::Location{
      .line = pos.line,
      .column = pos.character,
  };
}

[[nodiscard]] inline core::ast::Range FromLSPRange(const lsp::Range& range, const core::ast::AST& ast) {
  return core::ast::Range{
      .begin = ast.lines.GetPosition(FromLSPPosition(range.start)),
      .end = ast.lines.GetPosition(FromLSPPosition(range.end)),
  };
}

}  // namespace conv

}  // namespace vanadium::ls
