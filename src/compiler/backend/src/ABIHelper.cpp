#include "vanadium/compiler/ABIHelper.h"

#include <memory>
#include <mutex>

#include <llvm/IR/Module.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>

#include <vanadium/lib/Assert.h>

namespace vanadium::compiler {

namespace {
std::once_flag llvm_machine_init_flag;
struct TargetMachineHolder {
  std::string triple;
  std::unique_ptr<llvm::TargetMachine> machine;
} holder;

void InitTargetMachine() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  holder.triple = llvm::sys::getDefaultTargetTriple();

  std::string error;
  const auto* target = llvm::TargetRegistry::lookupTarget(holder.triple, error);
  if (!target) {
    VANADIUM_DEBUG_ERROR("Failed to lookup target triple: '{}'", error);
    return;
  }

  llvm::TargetOptions opt;
  holder.machine.reset(target->createTargetMachine(holder.triple, "generic", "", opt, std::nullopt));
}
}  // namespace

void ConfigureTargetTriple(llvm::Module& mod) {
  std::call_once(llvm_machine_init_flag, InitTargetMachine);
  mod.setTargetTriple(holder.triple);
  mod.setDataLayout(holder.machine->createDataLayout());
}

}  // namespace vanadium::compiler
