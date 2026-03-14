#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace vanadium {

namespace core {
struct SourceFile;
}

namespace compiler {
void GenerateModuleRegistrationCode(const core::SourceFile&, llvm::LLVMContext&, llvm::IRBuilder<>&, llvm::Module&);
}

}  // namespace vanadium
