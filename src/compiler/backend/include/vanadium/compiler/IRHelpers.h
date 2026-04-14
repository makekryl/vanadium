#pragma once

#include <cstdint>

#include <llvm-19/llvm/ADT/Twine.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

namespace vanadium::compiler::helpers {

llvm::Value* CreateGlobalBytePtr(llvm::Module&, llvm::ArrayRef<std::uint8_t> data, const llvm::Twine& name = "");

}
