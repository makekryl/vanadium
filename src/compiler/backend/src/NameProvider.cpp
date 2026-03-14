#include <format>

#include <vanadium/core/TypeChecker.h>

#include "vanadium/ast/utils/ASTUtils.h"
#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler::names {

namespace {
std::string SymName(const core::semantic::Symbol* sym) {
  if (sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    return std::string(sym->GetName());
  }
  // TODO: this can be more efficient (?)
  const auto* modnode = ast::utils::GetPredecessor<ast::nodes::Module>(sym->Declaration());
  const auto* sf = ast::utils::SourceFileOf(modnode);
  return std::format("{}_{}", sf->Text(*modnode->name), sym->GetName());
}
}  // namespace

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
