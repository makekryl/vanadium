#pragma once

#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>

namespace vanadium::compiler {

struct TypeSymbol {
  const core::semantic::Symbol* sym;
  bool is_template;

  TypeSymbol(std::nullptr_t = nullptr) : sym(nullptr), is_template(false) {}

  TypeSymbol(const core::semantic::Symbol* sym, bool is_template = false) : sym(sym), is_template(is_template) {
    assert(sym->Flags() & core::semantic::SymbolFlags::kType);
  }

  TypeSymbol(const core::checker::InstantiatedType& itype)
      : TypeSymbol{itype.sym, (itype.restriction != core::checker::TemplateRestrictionKind::kNone)} {}

  operator const core::semantic::Symbol*() const {
    return sym;
  }

  const core::semantic::Symbol* operator->() const {
    return sym;
  }

  [[nodiscard]] bool operator==(const core::semantic::Symbol* csym) const {
    return sym == csym;
  }

  [[nodiscard]] TypeSymbol Derive(const core::semantic::Symbol* dsym) const {
    return {dsym, is_template};
  }
};

}  // namespace vanadium::compiler
