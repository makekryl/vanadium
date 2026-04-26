#include "vanadium/compiler/RuntimeBindings.h"

#include <format>
#include <string_view>
#include <utility>
#include <variant>

#include <llvm/IR/Attributes.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace vanadium::compiler {
namespace {

llvm::Value* MakeBoundValW(llvm::IRBuilder<>& builder, llvm::Type* ty, llvm::Value* v) {
  llvm::Value* result = llvm::UndefValue::get(ty);
  result = builder.CreateInsertValue(result, v, 0);
  result = builder.CreateInsertValue(result, builder.getTrue(), 1);  // is_bound = true
  // result = builder.CreateInsertValue(result, builder.getFalse(), 2);  // is_big = false
  return result;
}

using SpecificBinaryOperationGenerator = llvm::Value* (*)(llvm::IRBuilder<>&, llvm::Type*, llvm::Value*, llvm::Value*);
using SpecificUnaryOperationGenerator = llvm::Value* (*)(llvm::IRBuilder<>&, llvm::Type*, llvm::Value*);
#define BLD_BINOP_TO_BOOL(op)                                                                      \
  ([](llvm::IRBuilder<>& builder, llvm::Type*, llvm::Value* va, llvm::Value* vb) -> llvm::Value* { \
    return builder.op(va, vb);                                                                     \
  })
#define BLD_BINOP_TO_VAL(create, op)                                                                  \
  ([](llvm::IRBuilder<>& builder, llvm::Type* ty, llvm::Value* va, llvm::Value* vb) -> llvm::Value* { \
    return create(builder, ty, builder.op(va, vb));                                                   \
  })

void GenerateAssertIsBound(RuntimeBindings& rt, llvm::LLVMContext& ctx, llvm::Function* fn, std::string_view tpname,
                           std::uint32_t flag_idx) {
  llvm::IRBuilder<> builder(ctx);

  auto* bb_entry = llvm::BasicBlock::Create(ctx, "", fn);
  auto* bb_unbound = llvm::BasicBlock::Create(ctx, "trap", fn);
  auto* bb_proceed = llvm::BasicBlock::Create(ctx, "ok", fn);

  llvm::Value* pv = fn->arg_begin();

  //=== ENTRY ===//
  builder.SetInsertPoint(bb_entry);
  //
  llvm::Value* pv_bound = builder.CreateExtractValue(pv, flag_idx);
  //
  builder.CreateCondBr(pv_bound, bb_proceed, bb_unbound);

  //=== TRAP ===//
  builder.SetInsertPoint(bb_unbound);
  builder.CreateCall(rt.panic, builder.CreateGlobalStringPtr(std::format("accessing an unbound {} value", tpname)));
  builder.CreateUnreachable();

  //=== PROCEED ===//
  builder.SetInsertPoint(bb_proceed);
  builder.CreateRetVoid();
}

void GenerateGenericBinaryNumericOperation(llvm::LLVMContext& ctx, llvm::Function* fn,
                                           llvm::Function* assert_is_bound_fn, SpecificBinaryOperationGenerator gen) {
  llvm::IRBuilder<> builder(ctx);

  auto* bb_entry = llvm::BasicBlock::Create(ctx, "", fn);

  //
  auto* arg_it = fn->arg_begin();
  llvm::Value* a = arg_it++;
  llvm::Value* b = arg_it++;
  //

  builder.SetInsertPoint(bb_entry);

  builder.CreateCall(assert_is_bound_fn, {a});
  builder.CreateCall(assert_is_bound_fn, {b});

  //
  llvm::Value* a_val = builder.CreateExtractValue(a, 0);
  llvm::Value* b_val = builder.CreateExtractValue(b, 0);
  //
  builder.CreateRet(gen(builder, a->getType(), a_val, b_val));
}

void GenerateGenericUnaryNumericOperation(llvm::LLVMContext& ctx, llvm::Function* fn,
                                          llvm::Function* assert_is_bound_fn, SpecificUnaryOperationGenerator gen) {
  llvm::IRBuilder<> builder(ctx);

  auto* bb_entry = llvm::BasicBlock::Create(ctx, "", fn);

  //
  auto* arg_it = fn->arg_begin();
  llvm::Value* a = arg_it++;
  //

  builder.SetInsertPoint(bb_entry);

  builder.CreateCall(assert_is_bound_fn, {a});

  //
  llvm::Value* a_val = builder.CreateExtractValue(a, 0);
  //
  builder.CreateRet(gen(builder, a->getType(), a_val));
}

void GenerateBoxedUnwrapFn(llvm::LLVMContext& ctx, llvm::Function* fn, llvm::Function* assert_is_bound_fn,
                           std::uint32_t idx) {
  llvm::IRBuilder<> builder(ctx);

  auto* bb_entry = llvm::BasicBlock::Create(ctx, "", fn);
  llvm::Value* arg = fn->arg_begin();

  builder.SetInsertPoint(bb_entry);
  builder.CreateCall(assert_is_bound_fn, {arg});
  builder.CreateRet(builder.CreateExtractValue(arg, idx));

  fn->addFnAttr(llvm::Attribute::AlwaysInline);
  fn->addFnAttr(llvm::Attribute::Hot);
}

void GenerateWrapFn(llvm::LLVMContext& ctx, llvm::Function* fn, llvm::Value* undef) {
  llvm::IRBuilder<> builder(ctx);

  auto* bb_entry = llvm::BasicBlock::Create(ctx, "", fn);
  llvm::Value* arg = fn->arg_begin();

  builder.SetInsertPoint(bb_entry);

  llvm::Value* s = undef;
  s = builder.CreateInsertValue(s, arg, 0);
  s = builder.CreateInsertValue(s, builder.getTrue(), 1);

  builder.CreateRet(s);

  fn->addFnAttr(llvm::Attribute::AlwaysInline);
  fn->addFnAttr(llvm::Attribute::Hot);
}

}  // namespace

RuntimeBindings::RuntimeBindings(llvm::LLVMContext& ctx, llvm::Module& mod) : ctx_(ctx), mod_(mod) {
  llvm::IRBuilder<> builder(ctx);

  const auto declare_external_fn = [&mod](std::string_view name, llvm::Type* result,
                                          llvm::ArrayRef<llvm::Type*> params) {
    return llvm::Function::Create(llvm::FunctionType::get(result, params, false),  //
                                  llvm::GlobalValue::ExternalLinkage, name, mod);
  };
  const auto declare_embedded_fn = [&mod](std::string_view name, llvm::Type* result,
                                          llvm::ArrayRef<llvm::Type*> params) {
    auto* f = llvm::Function::Create(llvm::FunctionType::get(result, params, false),  //
                                     llvm::GlobalValue::InternalLinkage, name, mod);
    f->addFnAttr(llvm::Attribute::AlwaysInline);
    f->addFnAttr(llvm::Attribute::Hot);
    return f;
  };

  using ArithmeticOpIntl =
      std::tuple<llvm::Function*&, llvm::Type*, std::string_view, SpecificBinaryOperationGenerator>;
  auto* const nbool_ty = builder.getInt1Ty();

  panic = llvm::Function::Create(llvm::FunctionType::get(builder.getVoidTy(), {builder.getPtrTy()}, false),
                                 llvm::GlobalValue::ExternalLinkage, "vrt_panic", mod);

  generic_getter_fn_ty = llvm::FunctionType::get(builder.getPtrTy(), {builder.getPtrTy()}, false);

  typeinfo_ty = llvm::StructType::create(ctx_, "vrt_typeinfo_t");
  typeinfo_ty->setBody({
      builder.getPtrTy(),    // const char* name
      builder.getInt8Ty(),   // vrt_typekind_e
      builder.getInt64Ty(),  // size_t size
      builder.getPtrTy(),    // members**
      builder.getPtrTy(),    // construct(*)(void*)
      builder.getPtrTy()     // destruct(*)(void*)
  });
  //
  obj_ctor_fn_ty = llvm::FunctionType::get(builder.getVoidTy(), {builder.getPtrTy()}, false);
  obj_dtor_fn_ty = llvm::FunctionType::get(builder.getVoidTy(), {builder.getPtrTy()}, false);

  type_new_f = declare_external_fn("vrt_new",
                                   builder.getPtrTy(),     // return void*
                                   {builder.getPtrTy()});  // const vrt_typeinfo_t*;
  type_del_f = declare_external_fn("vrt_del", builder.getVoidTy(),
                                   {
                                       builder.getPtrTy(),  // const vrt_typeinfo_t*
                                       builder.getPtrTy(),  // void* p
                                   });

  stackalloc_mark_f = declare_external_fn("vrt_stackalloc_mark", builder.getVoidTy(), {});
  stackalloc_sweep_f = declare_external_fn("vrt_stackalloc_sweep", builder.getVoidTy(), {});
  for (auto* f : {stackalloc_mark_f, stackalloc_sweep_f}) {
    f->addFnAttr(llvm::Attribute::NoUnwind);
    f->addFnAttr(llvm::Attribute::WillReturn);
  }
  obj_stackalloc_new_f = declare_external_fn("vrt_stackalloc_new",
                                             builder.getPtrTy(),     // return void*
                                             {builder.getPtrTy()});  // const vrt_typeinfo_t*

  generic_val_ty = llvm::StructType::create(ctx_, "vrt_val_t");
  generic_val_ty->setBody({
      builder.getPtrTy(),  // void* p
      builder.getPtrTy()   // const vrt_typeinfo_t* ty
  });

  optional_ty = llvm::StructType::create(ctx, "vrt_optional_t");
  optional_ty->setBody({
      builder.getPtrTy(),   // void* value
      builder.getPtrTy(),   // const vrt_typeinfo_t* member_type
      builder.getInt1Ty(),  // bool is_present
  });
  //
  // TODO: embed inlined code for functions below
  optional_get_f = declare_external_fn("vrt_optional_get", builder.getPtrTy(), {builder.getPtrTy()});
  optional_set_f = declare_external_fn("vrt_optional_set", builder.getVoidTy(),
                                       {
                                           builder.getPtrTy(),  // vrt_optional_t*
                                           builder.getPtrTy(),  // void*
                                       });
  optional_ispresent_f = declare_external_fn("vrt_optional_is_present", builder.getInt1Ty(), {builder.getPtrTy()});
  optional_dtor_f = declare_external_fn("vrt_optional_dtor", builder.getVoidTy(), {builder.getPtrTy()});

  integer.ty = llvm::StructType::create(ctx, "vrt_integer_t");
  integer.ty->setBody({
      builder.getInt64Ty(),  // int64 val
      builder.getInt1Ty(),   // bool is_bound
                             // builder.getInt1Ty(),   // bool is_big
  });
  //
  integer.undef =
      llvm::ConstantStruct::get(integer.ty, {
                                                llvm::UndefValue::get(builder.getInt64Ty()),
                                                builder.getFalse(),  // is_bound = false
                                                                     // builder.getFalse(),  // is_big = false
                                            });
  //
  auto* int_assert_is_bound_fn =
      declare_embedded_fn("__vrt_integer_assert_is_bound", builder.getVoidTy(), {integer.ty});
  GenerateAssertIsBound(*this, ctx, int_assert_is_bound_fn, "integer", 1);
  //
  integer.get_f = llvm::Function::Create(llvm::FunctionType::get(builder.getInt64Ty(), {integer.ty}, false),
                                         llvm::GlobalValue::InternalLinkage, "__vrt_integer_get", mod);
  GenerateBoxedUnwrapFn(ctx, integer.get_f, int_assert_is_bound_fn, 0);
  //
  integer.wrap_f = llvm::Function::Create(llvm::FunctionType::get(integer.ty, {builder.getInt64Ty()}, false),
                                          llvm::GlobalValue::InternalLinkage, "__vrt_integer_wrap", mod);
  GenerateWrapFn(ctx, integer.wrap_f, integer.undef);
  //
  for (const auto& [fptr, result_ty, fname, fnativeb] : {
           ArithmeticOpIntl(integer.eq_f, nbool_ty, "__vrt_integer_eq", BLD_BINOP_TO_BOOL(CreateICmpEQ)),
           ArithmeticOpIntl(integer.ne_f, nbool_ty, "__vrt_integer_ne", BLD_BINOP_TO_BOOL(CreateICmpNE)),
           //
           ArithmeticOpIntl(integer.lt_f, nbool_ty, "__vrt_integer_lt", BLD_BINOP_TO_BOOL(CreateICmpSLT)),
           ArithmeticOpIntl(integer.le_f, nbool_ty, "__vrt_integer_le", BLD_BINOP_TO_BOOL(CreateICmpSLE)),
           ArithmeticOpIntl(integer.gt_f, nbool_ty, "__vrt_integer_gt", BLD_BINOP_TO_BOOL(CreateICmpSGT)),
           ArithmeticOpIntl(integer.ge_f, nbool_ty, "__vrt_integer_ge", BLD_BINOP_TO_BOOL(CreateICmpSGE)),
           //
           ArithmeticOpIntl(integer.add_f, integer.ty, "__vrt_integer_add", BLD_BINOP_TO_VAL(MakeBoundValW, CreateAdd)),
           ArithmeticOpIntl(integer.sub_f, integer.ty, "__vrt_integer_sub", BLD_BINOP_TO_VAL(MakeBoundValW, CreateSub)),
           ArithmeticOpIntl(integer.mul_f, integer.ty, "__vrt_integer_mul", BLD_BINOP_TO_VAL(MakeBoundValW, CreateMul)),
           ArithmeticOpIntl(integer.div_f, integer.ty, "__vrt_integer_div",
                            BLD_BINOP_TO_VAL(MakeBoundValW, CreateSDiv)),
       }) {
    fptr = declare_embedded_fn(fname, result_ty, {integer.ty, integer.ty});
    GenerateGenericBinaryNumericOperation(ctx, fptr, int_assert_is_bound_fn, fnativeb);
  }
  integer.neg_f = declare_embedded_fn("__vrt_integer_neg", integer.ty, {integer.ty});
  GenerateGenericUnaryNumericOperation(ctx, integer.neg_f, int_assert_is_bound_fn,
                                       [](llvm::IRBuilder<>& builder, llvm::Type* ty, llvm::Value* va) -> llvm::Value* {
                                         return MakeBoundValW(builder, ty, builder.CreateNeg(va));
                                       });

  floatt.ty = llvm::StructType::create(ctx, "vrt_float_t");
  floatt.ty->setBody({
      builder.getDoubleTy(),  // double val
      builder.getInt1Ty(),    // bool is_bound
  });
  //
  floatt.undef = llvm::ConstantStruct::get(floatt.ty, {
                                                          llvm::UndefValue::get(builder.getDoubleTy()),
                                                          builder.getFalse(),  // is_bound = false
                                                      });
  //
  auto* float_assert_is_bound_fn = declare_embedded_fn("__vrt_float_assert_is_bound", builder.getVoidTy(), {floatt.ty});
  GenerateAssertIsBound(*this, ctx, float_assert_is_bound_fn, "float", 1);
  //
  floatt.get_f = llvm::Function::Create(llvm::FunctionType::get(builder.getDoubleTy(), {floatt.ty}, false),
                                        llvm::GlobalValue::InternalLinkage, "__vrt_float_get", mod);
  GenerateBoxedUnwrapFn(ctx, floatt.get_f, float_assert_is_bound_fn, 0);
  //
  floatt.wrap_f = llvm::Function::Create(llvm::FunctionType::get(floatt.ty, {builder.getDoubleTy()}, false),
                                         llvm::GlobalValue::InternalLinkage, "__vrt_float_wrap", mod);
  GenerateWrapFn(ctx, floatt.wrap_f, floatt.undef);
  //
  for (const auto& [fptr, result_ty, fname, fnativeb] : {
           ArithmeticOpIntl(floatt.eq_f, nbool_ty, "__vrt_float_eq", BLD_BINOP_TO_BOOL(CreateFCmpOEQ)),
           ArithmeticOpIntl(floatt.ne_f, nbool_ty, "__vrt_float_ne", BLD_BINOP_TO_BOOL(CreateFCmpONE)),
           //
           ArithmeticOpIntl(floatt.lt_f, nbool_ty, "__vrt_float_lt", BLD_BINOP_TO_BOOL(CreateFCmpOLT)),
           ArithmeticOpIntl(floatt.le_f, nbool_ty, "__vrt_float_le", BLD_BINOP_TO_BOOL(CreateFCmpOLE)),
           ArithmeticOpIntl(floatt.gt_f, nbool_ty, "__vrt_float_gt", BLD_BINOP_TO_BOOL(CreateFCmpOGT)),
           ArithmeticOpIntl(floatt.ge_f, nbool_ty, "__vrt_float_ge", BLD_BINOP_TO_BOOL(CreateFCmpOGE)),
           //
           ArithmeticOpIntl(floatt.add_f, floatt.ty, "__vrt_float_add", BLD_BINOP_TO_VAL(MakeBoundValW, CreateFAdd)),
           ArithmeticOpIntl(floatt.sub_f, floatt.ty, "__vrt_float_sub", BLD_BINOP_TO_VAL(MakeBoundValW, CreateFSub)),
           ArithmeticOpIntl(floatt.mul_f, floatt.ty, "__vrt_float_mul", BLD_BINOP_TO_VAL(MakeBoundValW, CreateFMul)),
           ArithmeticOpIntl(floatt.div_f, floatt.ty, "__vrt_float_div", BLD_BINOP_TO_VAL(MakeBoundValW, CreateFDiv)),
       }) {
    fptr = declare_embedded_fn(fname, result_ty, {floatt.ty, floatt.ty});
    GenerateGenericBinaryNumericOperation(ctx, fptr, float_assert_is_bound_fn, fnativeb);
  }
  floatt.neg_f = declare_embedded_fn("__vrt_float_neg", floatt.ty, {floatt.ty});
  GenerateGenericUnaryNumericOperation(ctx, floatt.neg_f, float_assert_is_bound_fn,
                                       [](llvm::IRBuilder<>& builder, llvm::Type* ty, llvm::Value* va) -> llvm::Value* {
                                         return MakeBoundValW(builder, ty, builder.CreateFNeg(va));
                                       });

  boolt.ty = llvm::StructType::create(ctx, "vrt_boolean_t");
  boolt.ty->setBody({
      builder.getInt1Ty(),  // bool value
      builder.getInt1Ty(),  // bool is_bound
  });
  //
  boolt.undef = llvm::ConstantStruct::get(boolt.ty, {
                                                        llvm::UndefValue::get(builder.getInt1Ty()),
                                                        builder.getFalse(),  // is_bound = false
                                                    });
  //
  auto* bool_assert_is_bound_fn = declare_embedded_fn("__vrt_boolean_assert_is_bound", builder.getVoidTy(), {boolt.ty});
  GenerateAssertIsBound(*this, ctx, bool_assert_is_bound_fn, "boolean", 1);
  //
  boolt.get_f = llvm::Function::Create(llvm::FunctionType::get(builder.getInt1Ty(), {boolt.ty}, false),
                                       llvm::GlobalValue::InternalLinkage, "__vrt_boolean_get", mod);
  GenerateBoxedUnwrapFn(ctx, boolt.get_f, bool_assert_is_bound_fn, 0);
  //
  boolt.wrap_f = llvm::Function::Create(llvm::FunctionType::get(boolt.ty, {builder.getInt1Ty()}, false),
                                        llvm::GlobalValue::InternalLinkage, "__vrt_boolean_wrap", mod);
  GenerateWrapFn(ctx, boolt.wrap_f, boolt.undef);

  const auto fill_string_bindings = [&](StringTypeBindings& b, std::string_view ty_name) {
    const auto sty_name = [&](std::format_string<std::string_view&> sname) {
      return std::format(sname, ty_name);
    };

    b.ty = llvm::StructType::create(ctx, sty_name("vrt_{}_t"));
    b.ty->setBody({
        builder.getPtrTy(),    // u32* data
        builder.getInt32Ty(),  // u32 capacity
        builder.getInt32Ty(),  // u32 length
        builder.getInt1Ty(),   // bool is_bound
        builder.getInt1Ty(),   // bool is_ext
    });
    //
    b.dtor_f = declare_external_fn(sty_name("vrt_{}_dtor"), builder.getVoidTy(), {builder.getPtrTy()});
    b.init_f = declare_external_fn(sty_name("vrt_{}_init"), builder.getVoidTy(),
                                   {
                                       builder.getPtrTy(),    // StringType*
                                       builder.getPtrTy(),    // const char*
                                       builder.getInt32Ty(),  // u32
                                   });
    b.copy_f = declare_external_fn(sty_name("copy_{}"), builder.getVoidTy(),
                                   {
                                       builder.getPtrTy(),  // StringType* dst
                                       builder.getPtrTy(),  // StringType* src
                                   });
    b.concat_f = declare_external_fn(sty_name("vrt_{}_concat"), builder.getVoidTy(),
                                     {
                                         builder.getPtrTy(),  // StringType* dst
                                         builder.getPtrTy(),  // lhs
                                         builder.getPtrTy(),  // rhs
                                     });                      // TODO: add nocapture flags, ...
    b.eq_f = declare_external_fn(sty_name("vrt_{}_eq"), builder.getInt1Ty(),
                                 {
                                     builder.getPtrTy(),  // lhs
                                     builder.getPtrTy(),  // rhs
                                 });
    b.ne_f = declare_external_fn(sty_name("vrt_{}_ne"), builder.getInt1Ty(),
                                 {
                                     builder.getPtrTy(),  // lhs
                                     builder.getPtrTy(),  // rhs
                                 });
    b.singular_f = declare_external_fn(sty_name("vrt_{}_singular"), builder.getVoidTy(),
                                       {
                                           builder.getPtrTy(),  // StringType* dst
                                           builder.getPtrTy(),
                                           builder.getInt32Ty(),
                                       });

    b.rotate_left_f = declare_external_fn(sty_name("vrt_{}_rotate_left"), builder.getVoidTy(),
                                          {
                                              builder.getPtrTy(),    // StringType* dst
                                              builder.getPtrTy(),    // const StringType* src
                                              builder.getInt64Ty(),  // int64 n
                                          });
    b.rotate_right_f = declare_external_fn(sty_name("vrt_{}_rotate_right"), builder.getVoidTy(),
                                           {
                                               builder.getPtrTy(),    // StringType* dst
                                               builder.getPtrTy(),    // const StringType* src
                                               builder.getInt64Ty(),  // int64 n
                                           });

    if (ty_name != "charstring") {
      b.shift_left_f = declare_external_fn(sty_name("vrt_{}_shift_left"), builder.getVoidTy(),
                                           {
                                               builder.getPtrTy(),    // StringType* dst
                                               builder.getPtrTy(),    // const StringType* src
                                               builder.getInt64Ty(),  // int64 n
                                           });
      b.shift_right_f = declare_external_fn(sty_name("vrt_{}_shift_right"), builder.getVoidTy(),
                                            {
                                                builder.getPtrTy(),    // StringType* dst
                                                builder.getPtrTy(),    // const StringType* src
                                                builder.getInt64Ty(),  // int64 n
                                            });
      //
      b.not4b_f = declare_external_fn(sty_name("vrt_{}_not4b"), builder.getVoidTy(),
                                      {
                                          builder.getPtrTy(),  // StringType* dst
                                          builder.getPtrTy(),  // const StringType* s
                                      });
      b.and4b_f = declare_external_fn(sty_name("vrt_{}_and4b"), builder.getVoidTy(),
                                      {
                                          builder.getPtrTy(),  // StringType* dst
                                          builder.getPtrTy(),  // lhs
                                          builder.getPtrTy(),  // rhs
                                      });
      b.or4b_f = declare_external_fn(sty_name("vrt_{}_or4b"), builder.getVoidTy(),
                                     {
                                         builder.getPtrTy(),  // StringType* dst
                                         builder.getPtrTy(),  // lhs
                                         builder.getPtrTy(),  // rhs
                                     });
      b.xor4b_f = declare_external_fn(sty_name("vrt_{}_xor4b"), builder.getVoidTy(),
                                      {
                                          builder.getPtrTy(),  // StringType* dst
                                          builder.getPtrTy(),  // lhs
                                          builder.getPtrTy(),  // rhs
                                      });
    }
    //
    b.undef = llvm::ConstantStruct::get(b.ty, {
                                                  llvm::UndefValue::get(builder.getPtrTy()),    //
                                                  llvm::UndefValue::get(builder.getInt32Ty()),  //
                                                  llvm::UndefValue::get(builder.getInt32Ty()),  //
                                                  builder.getTrue(),                            // is_bound = true
                                                  builder.getFalse(),                           // is_ext = true
                                              });
  };

  fill_string_bindings(charstring, "charstring");
  fill_string_bindings(octetstring, "octetstring");
  fill_string_bindings(bitstring, "bitstring");
  fill_string_bindings(hexstring, "hexstring");
}

llvm::ConstantInt* RuntimeBindings::GetRawInt(NativeIntType value) const {
  return llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), value, true);
}
llvm::Value* RuntimeBindings::GetInt(std::variant<NativeIntType, std::string_view> value) const {
  if (const auto* v = std::get_if<NativeIntType>(&value)) {
    return llvm::ConstantStruct::get(integer.ty, {
                                                     GetRawInt(*v),
                                                     llvm::ConstantInt::getTrue(ctx_),  // is_bound = true
                                                     //  llvm::ConstantInt::getFalse(ctx_),  // is_big = false
                                                 });
  }
  // TODO: big ints -- call something like vrt_integer_from_str()
  std::unreachable();
}

llvm::Constant* RuntimeBindings::GetRawFloat(double value) const {
  return llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx_), value);
}
llvm::Value* RuntimeBindings::GetFloat(double value) const {
  return llvm::ConstantStruct::get(floatt.ty, {
                                                  GetRawFloat(value),
                                                  llvm::ConstantInt::getTrue(ctx_),  // is_bound = true
                                              });
}

llvm::Value* RuntimeBindings::GetBool(bool v) const {
  return llvm::ConstantStruct::get(boolt.ty, {
                                                 llvm::ConstantInt::getBool(llvm::Type::getInt1Ty(ctx_), v),
                                                 llvm::ConstantInt::getTrue(ctx_),  // is_bound = true
                                             });
}

llvm::Type* RuntimeBindings::MakeUnion(std::span<llvm::Type*> members) const {
  const auto& dl = mod_.getDataLayout();

  std::uint64_t max_size = 0;
  llvm::Type* largest_ty = nullptr;

  for (auto* ty : members) {
    const std::uint64_t size = dl.getTypeAllocSize(ty);
    if (size > max_size) {
      max_size = size;
      largest_ty = ty;
    }
  }

  return largest_ty;
}

}  // namespace vanadium::compiler
