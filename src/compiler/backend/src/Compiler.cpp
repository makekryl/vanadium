#include "vanadium/compiler/Compiler.h"

#include <cassert>
#include <format>
#include <ranges>

#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Builtins.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>

#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler {

namespace {

class Codegen {
 public:
  Codegen(const core::SourceFile& sf) : u_(sf) {}

  void Gen() {
    CodegenTypes();
    CodegenFunctions();

    GenerateModuleRegistrationCode(u_);

    std::error_code ec;
    llvm::raw_fd_ostream dest(std::format("{}.ll", u_.sf.path), ec);
    u_.mod.print(dest, nullptr);
  }

 private:
  void CodegenTypes() {
    for (const auto& sym : u_.sf.module->scope->symbols.Enumerate() | std::views::values) {
      if (!(sym.Flags() & core::semantic::SymbolFlags::kType)) {
        continue;
      }
      CodegenType(u_, &sym);
    }
  }

  void CodegenFunctions() {
    for (const auto& sym : u_.sf.module->scope->symbols.Enumerate() | std::views::values) {
      if (!(sym.Flags() & core::semantic::SymbolFlags::kFunction)) {
        continue;
      }
      CodegenFunction(u_, &sym);
    }
  }

  //

  CodegenUnit u_;
};

}  // namespace

//

void CompileIR(const core::SourceFile& sf) {
  Codegen(sf).Gen();
}

}  // namespace vanadium::compiler
