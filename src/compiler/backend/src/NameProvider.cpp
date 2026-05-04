#include <format>

#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>

#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler::names {

namespace {
std::string SymName(const core::semantic::Symbol* sym, bool is_template = false) {
  if (sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    return std::string(sym->GetName()) + (is_template ? "_template" : "");
  }
  const auto* sf = ast::utils::SourceFileOf(sym->Declaration());
  return std::format("{}_{}{}", sf->module->name, sym->GetName(), is_template ? "_template" : "");
}

auto SymName(TypeSymbol ts) {
  return SymName(ts.sym, ts.is_template);
}
}  // namespace

std::string Func(const core::semantic::Symbol* sym) {
  if (sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    return std::format("vrt_{}", sym->GetName());
  }
  return SymName(sym);
}

std::string Type(TypeSymbol ts) {
  // TODO: reconsider
  if (ts->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    return std::format("vrt_{}", SymName(ts));
  }
  return SymName(ts);
}

std::string Ctor(TypeSymbol ts) {
  return std::format("{}_ctor", SymName(ts));
}

std::string Dtor(TypeSymbol ts) {
  return std::format("{}_dtor", SymName(ts));
}

std::string CopyCtor(TypeSymbol ts) {
  return std::format("copy_{}", SymName(ts));
}

std::string TInfo(TypeSymbol ts) {
  return std::format("{}_typeinfo", SymName(ts));
}

std::string Getter(TypeSymbol holder, std::string_view member) {
  return std::format("{}_get_{}", SymName(holder), member);
}

std::string Muttor(TypeSymbol holder, std::string_view member) {
  return std::format("{}_mut_{}", SymName(holder), member);
}

std::string TplValCtor(TypeSymbol ts) {
  assert(ts.is_template);
  return std::format("{}_tplval_ctor", SymName(ts));
}

}  // namespace vanadium::compiler::names
