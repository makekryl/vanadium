#include "vanadium/compiler/IRHelpers.h"

#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

namespace vanadium::compiler::helpers {

llvm::Value* CreateGlobalBytePtr(llvm::Module& mod, llvm::ArrayRef<uint8_t> data, const llvm::Twine& name) {
  auto* arr = llvm::ConstantDataArray::get(mod.getContext(), data);
  auto* gv = new llvm::GlobalVariable(mod, arr->getType(), true, llvm::GlobalValue::PrivateLinkage, arr, name);

  gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
  gv->setAlignment(llvm::Align(1));

  llvm::Value* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(mod.getContext()), 0);
  return llvm::ConstantExpr::getInBoundsGetElementPtr(arr->getType(), gv, {zero, zero});
}

}  // namespace vanadium::compiler::helpers
