#include "vanadium/compiler/Compiler.h"

#include <cassert>
#include <filesystem>
#include <ranges>

#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/lib/Assert.h>
#include <vanadium/lib/FunctionRef.h>
#include <vanadium/lib/concurrency/Algorithm.h>
#include <vanadium/lib/concurrency/ThreadSpecific.h>

#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler {

namespace {
void CompileUnit(CodegenUnit& u) {
  u.mod.setSourceFileName(std::filesystem::path(u.sf.path).filename().string());

  for (const auto& sym : u.sf.module->scope->symbols.Enumerate() | std::views::values) {
    if (sym.Flags() & core::semantic::SymbolFlags::kType) {
      const auto* n = sym.Declaration();
      switch (n->nkind) {
        case ast::NodeKind::StructTypeDecl:
          CodegenStruct(u, &sym, n->As<ast::nodes::StructTypeDecl>());
          break;
        default:
          VANADIUM_DEBUG_ERROR("Unhandled type node: {}", magic_enum::enum_name(n->nkind));
          break;
      }
    } else if (sym.Flags() & core::semantic::SymbolFlags::kFunction) {
      CodegenFunction(u, &sym);
    }
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

void Compile(const core::SourceFile& sf, const CompileOptions& opts, llvm::Module& mod) {
  CodegenUnit u(mod, sf, opts.debug);
  CompileUnit(u);
}

void Compile(const core::Program& program, const CompileOptions& opts,
             lib::Consumer<const core::SourceFile&, llvm::Module&> accept) {
  lib::concurrency::ThreadSpecific<llvm::LLVMContext> llvm_ctx;
  lib::concurrency::ParallelFor(program.Files() | std::views::values, [&](const core::SourceFile& sf) {
    llvm::Module mod(sf.module->name, llvm_ctx.Local());
    Compile(sf, opts, mod);
    accept(sf, mod);
  });
}

}  // namespace vanadium::compiler
