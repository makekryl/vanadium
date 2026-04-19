#include "vanadium/compiler/Compiler.h"

#include <cassert>
#include <format>
#include <ranges>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/lib/FunctionRef.h>
#include <vanadium/lib/concurrency/Algorithm.h>
#include <vanadium/lib/concurrency/ThreadSpecific.h>

#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler {

namespace {
void CompileUnit(CodegenUnit& u) {
  const auto symbols = [&](core::semantic::SymbolFlags::Value mask) {
    return u.sf.module->scope->symbols.Enumerate() | std::views::values | std::views::filter([mask](const auto& sym) {
             return bool(sym.Flags() & mask);
           });
  };

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
}
}  // namespace

void Compile(const core::Program& program, const CompileOptions& opts,
             lib::Consumer<const core::SourceFile&, llvm::Module&> accept) {
  lib::concurrency::ThreadSpecific<llvm::LLVMContext> llvm_ctx;
  lib::concurrency::ParallelFor(program.Files() | std::views::values, [&](const core::SourceFile& sf) {
    CodegenUnit u(llvm_ctx.Local(), sf, opts.debug);
    CompileUnit(u);
    accept(u.sf, u.mod);
  });
}

}  // namespace vanadium::compiler
