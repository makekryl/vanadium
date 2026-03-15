#include "vanadium/compiler/Compiler.h"

#include <cassert>
#include <format>
#include <ranges>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>

#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler {
void CompileIR(const core::SourceFile& sf) {
  const auto symbols = [&](core::semantic::SymbolFlags::Value mask) {
    return sf.module->scope->symbols.Enumerate() | std::views::values | std::views::filter([mask](const auto& sym) {
             return bool(sym.Flags() & mask);
           });
  };

  CodegenUnit u(sf);
  for (const auto& sym : symbols(core::semantic::SymbolFlags::kType)) {
    CodegenType(u, &sym);
  }
  for (const auto& sym : symbols(core::semantic::SymbolFlags::kFunction)) {
    CodegenFunction(u, &sym);
  }

  GenerateModuleRegistrationCode(u);

  std::error_code ec;
  llvm::raw_fd_ostream dest(std::format("{}.ll", sf.path), ec);
  u.mod.print(dest, nullptr);
}
}  // namespace vanadium::compiler
