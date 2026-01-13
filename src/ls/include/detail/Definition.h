#pragma once

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>

#include "LSProtocol.h"

namespace vanadium::ls {
namespace detail {

const ast::Node* FindNode(const core::SourceFile*, lsp::Position pos);

struct SymbolSearchResult {
  const ast::Node* node;
  const core::semantic::Scope* scope;
  core::checker::InstantiatedType type;
};
std::optional<SymbolSearchResult> FindSymbol(const core::SourceFile*, const ast::Node*);
std::optional<SymbolSearchResult> FindSymbol(const core::SourceFile*, lsp::Position pos);

const ast::Node* GetReadableDefinition(const ast::Node* n);

}  // namespace detail
}  // namespace vanadium::ls
