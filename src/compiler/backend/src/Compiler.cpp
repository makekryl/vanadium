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
void CompileIR(const core::SourceFile& sf, const CompileOptions& opts) {
  const auto symbols = [&](core::semantic::SymbolFlags::Value mask) {
    return sf.module->scope->symbols.Enumerate() | std::views::values | std::views::filter([mask](const auto& sym) {
             return bool(sym.Flags() & mask);
           });
  };

  CodegenUnit u(sf, opts.debug);
  for (const auto& sym : symbols(core::semantic::SymbolFlags::kType)) {
    CodegenType(u, &sym);
  }
  for (const auto& sym : symbols(core::semantic::SymbolFlags::kFunction)) {
    CodegenFunction(u, &sym);
  }

  GenerateModuleRegistrationCode(u);

  u.EmitDebugInfo([&](DebugInfo& dinfo) {
    u.mod.addModuleFlag(llvm::Module::Warning, "Dwarf Version",
                        llvm::ConstantInt::get(llvm::Type::getInt32Ty(u.ctx), 5));
    u.mod.addModuleFlag(llvm::Module::Warning, "Debug Info Version",
                        llvm::ConstantInt::get(llvm::Type::getInt32Ty(u.ctx), 3));
    dinfo.builder.finalize();
  });

  std::error_code ec;
  llvm::raw_fd_ostream dest(std::format("{}.ll", sf.path), ec);
  u.mod.print(dest, nullptr);
}
}  // namespace vanadium::compiler
