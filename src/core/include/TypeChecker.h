#pragma once

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "Semantic.h"

namespace vanadium::core {

//////////////////////////
struct SourceFile;
struct ModuleDescriptor;
namespace semantic {
class Symbol;
class Scope;
}  // namespace semantic
//////////////////////////

namespace checker {

namespace symbols {
extern const semantic::Symbol kTypeError;
extern const semantic::Symbol kVoidType;
}  // namespace symbols

const semantic::Symbol* ResolveExprSymbol(const SourceFile*, const semantic::Scope*, const ast::nodes::Expr*);
const semantic::Symbol* ResolveDeclarationType(const SourceFile*, const semantic::Scope*, const ast::nodes::Decl*);
const semantic::Symbol* ResolveCallableReturnType(const SourceFile*, const semantic::Scope*, const ast::nodes::Decl*);
const semantic::Symbol* ResolveExprType(const SourceFile*, const semantic::Scope*, const ast::nodes::Expr*);
const semantic::Symbol* ResolvePotentiallyAliasedType(const semantic::Symbol*);
const semantic::Symbol* DeduceCompositeLiteralType(const SourceFile*, const semantic::Scope*,
                                                   const ast::nodes::CompositeLiteral*);

namespace utils {
[[nodiscard]] const ast::nodes::FormalPars* ResolveCallableParams(const SourceFile*, const semantic::Scope*,
                                                                  const ast::nodes::ParenExpr*);

[[nodiscard]] std::string_view GetReadableTypeName(const SourceFile*, const semantic::Symbol*);
[[nodiscard]] std::string_view GetReadableTypeName(const semantic::Symbol*);
}  // namespace utils

struct TypeError {
  ast::Range range;
  std::string message;
};

void PerformTypeCheck(const ModuleDescriptor& module, std::vector<TypeError>& errors);

}  // namespace checker

}  // namespace vanadium::core
