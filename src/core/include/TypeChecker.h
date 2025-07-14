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

namespace utils {
const ast::nodes::FormalPars* GetCallableDeclParams(const ast::nodes::Decl*);
const semantic::Symbol* GetCallableReturnType(const SourceFile&, const ast::nodes::Decl*);
}  // namespace utils

struct TypeError {
  ast::Range range;
  std::string message;  // TODO: get rid of string
};

const semantic::Symbol* DeduceExpressionType(const SourceFile&, const ast::nodes::Expr*);

void PerformTypeCheck(const ModuleDescriptor& module, std::vector<TypeError>& errors);

}  // namespace checker

}  // namespace vanadium::core
