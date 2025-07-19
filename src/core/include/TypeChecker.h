#pragma once

#include "ASTNodes.h"
#include "ASTTypes.h"

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
extern const semantic::Symbol kErrorTypeSym;

extern const semantic::Symbol kVoidSym;

extern const semantic::Symbol kWildcardTypeSym;
extern const semantic::Symbol kQuestionTypeSym;
}  // namespace symbols

namespace utils {
[[nodiscard]] std::string_view GetReadableTypeName(const SourceFile&, const semantic::Symbol*);
[[nodiscard]] std::string_view GetReadableTypeName(const semantic::Symbol*);

[[nodiscard]] const semantic::Symbol* ResolvePotentiallyAliasedType(const semantic::Symbol*);

[[nodiscard]] const semantic::Symbol* GetIdentType(const SourceFile&, const semantic::Scope*, const ast::nodes::Ident*);
[[nodiscard]] const semantic::Symbol* GetCallableReturnType(const SourceFile&, const ast::nodes::Decl*);
[[nodiscard]] const semantic::Symbol* GetSelectorExprType(const SourceFile&, const semantic::Scope*,
                                                          const ast::nodes::SelectorExpr*);
[[nodiscard]] std::optional<std::pair<std::size_t, std::size_t>> GetLengthExprBounds(const SourceFile*,
                                                                                     const ast::nodes::LengthExpr*);
}  // namespace utils

struct TypeError {
  ast::Range range;
  std::string message;  // TODO: get rid of string
};

const semantic::Symbol* GetEffectiveType(const SourceFile&, const semantic::Scope*, const ast::Node*);
const semantic::Symbol* DeduceExpressionType(const SourceFile&, const ast::nodes::Expr*);

void PerformTypeCheck(const ModuleDescriptor& module, std::vector<TypeError>& errors);

}  // namespace checker

}  // namespace vanadium::core
