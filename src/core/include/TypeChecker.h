#pragma once

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "EnumFlags.h"
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

// ES 201 873-1 [TTCN-3: Core Language], par. 15.8.1
enum class TemplateRestrictionKind : std::uint8_t {
  kNone = 0,

  kOmit = 1 << 0,
  kValue = 1 << 1,
  kPresent = 1 << 2,

  kRegular = kOmit | kValue | kPresent,

  kOptionalField = 1 << 3,
};
ENUM_FLAGS_TRAITS(TemplateRestrictionKind)
[[nodiscard]] TemplateRestrictionKind ParseTemplateRestriction(const ast::nodes::RestrictionSpec*);

struct InstantiatedType {
  const semantic::Symbol* sym{nullptr};
  TemplateRestrictionKind restriction{TemplateRestrictionKind::kNone};
  bool is_instance{false};
  std::uint32_t depth{0};

  operator bool() const {
    return sym != nullptr;
  }
  const semantic::Symbol* operator->() const {
    return sym;
  }

  InstantiatedType& operator=(const semantic::Symbol* nsym) {
    sym = nsym;
    return *this;
  }

  InstantiatedType Derive(const semantic::Symbol* nsym) const {
    return InstantiatedType{
        .sym = nsym,
        .restriction = restriction,
        .is_instance = is_instance,
        .depth = depth,
    };
  }

  static constexpr InstantiatedType None() {
    return {};
  }
};

namespace symbols {
extern const semantic::Symbol kTypeError;
extern const semantic::Symbol kVoidType;
extern const semantic::Symbol kInferType;
extern const semantic::Symbol kVarargsType;
extern const semantic::Symbol kAltstepType;
}  // namespace symbols

InstantiatedType ResolveExprSymbol(const SourceFile*, const semantic::Scope*, const ast::nodes::Expr*);
InstantiatedType ResolveDeclarationType(const SourceFile*, const ast::Node*);
InstantiatedType ResolveCallableReturnType(const SourceFile*, const ast::nodes::Decl*);
InstantiatedType ResolveExprType(const SourceFile*, const semantic::Scope*, const ast::nodes::Expr*);

const semantic::Symbol* ResolveListElementType(const semantic::Symbol*);
const semantic::Symbol* ResolveTypeSpecSymbol(const SourceFile*, const ast::nodes::TypeSpec*);

const semantic::Symbol* ResolvePotentiallyAliasedType(const semantic::Symbol*);

namespace ext {
InstantiatedType ResolveAssignmentTarget(const SourceFile*, const semantic::Scope*, const ast::nodes::AssignmentExpr*);
InstantiatedType DeduceCompositeLiteralType(const SourceFile*, const semantic::Scope*,
                                            const ast::nodes::CompositeLiteral*,
                                            const InstantiatedType& parent_hint = InstantiatedType::None());
InstantiatedType DeduceExpectedType(const SourceFile*, const semantic::Scope*, const ast::Node*);
}  // namespace ext

namespace utils {
[[nodiscard]] const ast::nodes::FormalPars* ResolveCallableParams(const SourceFile*, const semantic::Scope*,
                                                                  const ast::nodes::ParenExpr*);
}  // namespace utils

struct TypeError {
  ast::Range range;
  std::string message;
};

void PerformTypeCheck(SourceFile&);

}  // namespace checker

}  // namespace vanadium::core
