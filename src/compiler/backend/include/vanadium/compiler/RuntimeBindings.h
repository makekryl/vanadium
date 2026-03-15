#pragma once

#include <cstdint>
#include <string_view>
#include <variant>

#include <llvm-19/llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace vanadium::compiler {
class RuntimeBindings {
 public:
  using NativeIntType = std::int64_t;

  RuntimeBindings(llvm::LLVMContext&, llvm::Module&);
  //

  llvm::Function* panic;

  llvm::FunctionType* generic_getter_fn_ty;

  llvm::StructType* typeinfo_ty;
  //
  llvm::FunctionType* type_ctor_fn_ty;
  llvm::FunctionType* type_dtor_fn_ty;

  llvm::Function* type_alloc_f;
  llvm::Function* type_free_f;

  llvm::StructType* generic_val_ty;

  llvm::StructType* optional_ty;
  //
  llvm::Function* optional_get_f;
  llvm::Function* optional_set_f;
  llvm::Function* optional_ispresent_f;
  llvm::Function* optional_dtor_f;

  llvm::StructType* int_ty;
  //
  llvm::Value* int_undef;
  [[nodiscard]] llvm::Value* GetInt(std::variant<NativeIntType, std::string_view>) const;
  //
  llvm::Function* int_eq_f;
  llvm::Function* int_ne_f;
  //
  llvm::Function* int_lt_f;
  llvm::Function* int_le_f;
  llvm::Function* int_gt_f;
  llvm::Function* int_ge_f;
  //
  llvm::Function* int_add_f;
  llvm::Function* int_sub_f;
  llvm::Function* int_mul_f;
  llvm::Function* int_div_f;

  llvm::StructType* charstring_ty;
  //
  llvm::Value* charstring_undef;
  [[nodiscard]] llvm::Value* GetCharstring(std::string_view) const;

 private:
  llvm::LLVMContext& ctx_;
  llvm::Module& mod_;
};
}  // namespace vanadium::compiler
