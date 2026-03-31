#pragma once

#include <cstdint>
#include <string_view>
#include <variant>

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

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
  llvm::FunctionType* obj_ctor_fn_ty;
  llvm::FunctionType* obj_dtor_fn_ty;

  llvm::Function* type_new_f;
  llvm::Function* type_del_f;
  //
  llvm::Function* stackalloc_mark_f;
  llvm::Function* stackalloc_sweep_f;
  llvm::Function* obj_stackalloc_new_f;

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
  llvm::Function* int_get_f;
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

  llvm::StructType* bool_ty;
  //
  llvm::Value* bool_undef;
  [[nodiscard]] llvm::Value* GetBool(bool) const;
  //
  llvm::Function* bool_get_f;

  llvm::StructType* charstring_ty;
  //
  llvm::Function* charstring_dtor_f;
  llvm::Function* charstring_init_f;
  llvm::Function* charstring_copy_f;
  llvm::Function* charstring_concat_f;
  llvm::Function* charstring_singular_f;
  llvm::Function* charstring_rotate_left_f;
  llvm::Function* charstring_rotate_right_f;
  llvm::Function* charstring_eq_f;
  //
  llvm::Value* charstring_undef;

  llvm::StructType* octetstring_ty;
  //
  llvm::Function* octetstring_dtor_f;
  llvm::Function* octetstring_init_f;
  llvm::Function* octetstring_copy_f;
  llvm::Function* octetstring_concat_f;
  llvm::Function* octetstring_singular_f;
  llvm::Function* octetstring_rotate_left_f;
  llvm::Function* octetstring_rotate_right_f;
  llvm::Function* octetstring_eq_f;
  //
  llvm::Value* octetstring_undef;

  [[nodiscard]] llvm::Type* MakeUnion(std::span<llvm::Type*> members) const;

 private:
  llvm::LLVMContext& ctx_;
  llvm::Module& mod_;
};
}  // namespace vanadium::compiler
