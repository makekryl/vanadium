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
extern const semantic::Symbol kSelfType;
}  // namespace symbols

const semantic::Symbol* ResolveExprSymbol(const SourceFile*, const semantic::Scope*, const ast::nodes::Expr*);
const semantic::Symbol* ResolveDeclarationType(const SourceFile*, const semantic::Scope*, const ast::nodes::Decl*);
const semantic::Symbol* ResolveCallableReturnType(const SourceFile*, const semantic::Scope*, const ast::nodes::Decl*);
const semantic::Symbol* ResolveExprType(const SourceFile*, const semantic::Scope*, const ast::nodes::Expr*);
const semantic::Symbol* ResolvePotentiallyAliasedType(const semantic::Symbol*);

namespace ext {
const semantic::Symbol* ResolveAssignmentTarget(const SourceFile*, const semantic::Scope*,
                                                const ast::nodes::AssignmentExpr*);
const semantic::Symbol* DeduceCompositeLiteralType(const SourceFile*, const semantic::Scope*,
                                                   const ast::nodes::CompositeLiteral*);
const semantic::Symbol* DeduceExpectedType(const SourceFile*, const semantic::Scope*, const ast::Node*);
}  // namespace ext

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

void PerformTypeCheck(SourceFile&);

}  // namespace checker

}  // namespace vanadium::core
