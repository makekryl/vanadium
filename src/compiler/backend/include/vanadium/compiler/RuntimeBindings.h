#pragma once

#include <cstdint>
#include <string_view>
#include <variant>

#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace vanadium::compiler {

// !!!!!!!!!!! SHOULD BE KEPT IN SYNC w/ rt_reflect.h !!!!!!!!!!!
// TODO: deduplicate
enum class RtTplSelection : std::uint8_t {
  kUninitialized = 0,

  kSpecificValue,

  kOmitValue,
  kAnyValue,
  kAnyOrOmit,

  kValueList,
  kComplementedList,
  kConjunctionList,

  kValueRange,
  kStringPattern,

  kSuperset,
  kSubset,

  kImplication,

  kDecode,
  kDynamic,
};
enum class RtTypeKind : std::uint8_t {
  kInteger,
  kFloat,
  kBoolean,

  kCharstring,
  kOctetstring,
  kBitstring,
  kHexstring,

  kEnum,
  kUnion,
  kRecord,
  kSet,

  kRecordOf,
  kSetOf,

  kOptionalMember,
};

struct NumericTypeBindings {
  llvm::StructType* ty;
  //
  llvm::Value* undef;
  //
  llvm::Function* get_f;
  llvm::Function* wrap_f;
  //
  llvm::Function* eq_f;
  llvm::Function* ne_f;
  //
  llvm::Function* lt_f;
  llvm::Function* le_f;
  llvm::Function* gt_f;
  llvm::Function* ge_f;
  //
  llvm::Function* neg_f;
  //
  llvm::Function* add_f;
  llvm::Function* sub_f;
  llvm::Function* mul_f;
  llvm::Function* div_f;
};

struct StringTypeBindings {
  llvm::StructType* ty;
  //
  llvm::Function* dtor_f;
  llvm::Function* init_f;
  llvm::Function* copy_f;
  llvm::Function* concat_f;
  llvm::Function* singular_f;
  llvm::Function* shift_left_f;
  llvm::Function* shift_right_f;
  llvm::Function* rotate_left_f;
  llvm::Function* rotate_right_f;
  llvm::Function* eq_f;
  llvm::Function* ne_f;
  //
  llvm::Function* not4b_f;
  llvm::Function* and4b_f;
  llvm::Function* or4b_f;
  llvm::Function* xor4b_f;
  //
  llvm::Value* undef;
};

class RuntimeBindings {
 public:
  using NativeIntType = std::int64_t;

  RuntimeBindings(llvm::LLVMContext&, llvm::Module&);
  //

  llvm::IntegerType* sizet_ty;

  llvm::Function* panic;

  llvm::FunctionType* generic_getter_fn_ty;

  llvm::StructType* typeinfo_ty;
  llvm::StructType* smember_ty;
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

  NumericTypeBindings integer;
  [[nodiscard]] llvm::ConstantInt* GetRawInt(NativeIntType) const;
  [[nodiscard]] llvm::Value* GetInt(std::variant<NativeIntType, std::string_view>) const;

  NumericTypeBindings floatt;
  [[nodiscard]] llvm::Constant* GetRawFloat(double) const;
  [[nodiscard]] llvm::Value* GetFloat(double) const;

  struct {
    llvm::StructType* ty;
    //
    llvm::Value* undef;
    //
    llvm::Function* get_f;
    llvm::Function* wrap_f;
    //
    llvm::Function* not_f;
    //
    llvm::Function* eq_f;
    llvm::Function* ne_f;
  } boolt;
  [[nodiscard]] llvm::Value* GetBool(bool) const;

  StringTypeBindings charstring;
  StringTypeBindings octetstring;
  StringTypeBindings bitstring;
  StringTypeBindings hexstring;

  struct {
    llvm::Type* tsel_ty;
    llvm::Type* listsize_ty;
    //
    llvm::Function* get_value;
    llvm::Function* value;
    llvm::Function* list;
    llvm::Function* omit;
    llvm::Function* any;
    llvm::Function* any_or_omit;
    //
    llvm::Function* range_integer;
    //
    llvm::Function* generic_opaque_struct_ctor_fn;
    llvm::Function* generic_opaque_struct_dtor_fn;
  } tpl;

  [[nodiscard]] llvm::Type* MakeUnion(llvm::ArrayRef<llvm::Type*> members) const;

  [[nodiscard]] std::vector<llvm::Type*> WrapTemplateStruct(llvm::StructType* specific_sty) const;

  [[nodiscard]] llvm::ConstantInt* GetSizeI(std::size_t i) const {
    return llvm::ConstantInt::get(sizet_ty, i);
  }

 private:
  llvm::LLVMContext& ctx_;
  llvm::Module& mod_;
};
}  // namespace vanadium::compiler
