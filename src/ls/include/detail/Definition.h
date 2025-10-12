#pragma once

#include "ASTNodes.h"
#include "LSProtocol.h"
#include "Program.h"
#include "Semantic.h"
#include "TypeChecker.h"

namespace vanadium::ls {
namespace detail {

const core::ast::Node* FindNode(const core::SourceFile*, lsp::Position pos);

struct SymbolSearchResult {
  const core::ast::Node* node;
  const core::semantic::Scope* scope;
  core::checker::InstantiatedType type;
};
std::optional<SymbolSearchResult> FindSymbol(const core::SourceFile*, const core::ast::Node*);
std::optional<SymbolSearchResult> FindSymbol(const core::SourceFile*, lsp::Position pos);

const core::ast::Node* GetReadableDefinition(const core::ast::Node* n);

}  // namespace detail
}  // namespace vanadium::ls
