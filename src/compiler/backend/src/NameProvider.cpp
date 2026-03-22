#include <format>

#include <vanadium/core/TypeChecker.h>

#include "vanadium/ast/utils/ASTUtils.h"
#include "vanadium/compiler/Codegen.h"
#include "vanadium/core/Semantic.h"

namespace vanadium::compiler::names {

namespace {
std::string SymName(const core::semantic::Symbol* sym) {
  if (sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    return std::string(sym->GetName());
  }
  const auto* sf = ast::utils::SourceFileOf(sym->Declaration());
  return std::format("{}_{}", sf->module->name, sym->GetName());
}
}  // namespace

std::string Func(const core::semantic::Symbol* sym) {
  if (sym->Flags() & core::semantic::SymbolFlags::kBuiltin && sym->GetName() == "log") {
    // TODO: generify
    return "vrt_log";
  }
  return SymName(sym);
}

std::string Ctor(const core::semantic::Symbol* sym) {
  return std::format("{}_ctor", SymName(sym));
}

std::string Dtor(const core::semantic::Symbol* sym) {
  return std::format("{}_dtor", SymName(sym));
}

std::string Getter(const core::semantic::Symbol* holder, std::string_view member) {
  return std::format("{}_get_{}", SymName(holder), member);
}

std::string TInfo(const core::semantic::Symbol* sym) {
  return std::format("{}_typeinfo", SymName(sym));
}

}  // namespace vanadium::compiler::names
