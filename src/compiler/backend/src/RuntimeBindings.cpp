#include "vanadium/compiler/RuntimeBindings.h"

#include <string_view>
#include <utility>
#include <variant>

#include <llvm-19/llvm/IR/Attributes.h>
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

llvm::Value* CreateBoundInt(llvm::IRBuilder<>& builder, llvm::Type* ty, llvm::Value* v) {
  llvm::Value* result = llvm::UndefValue::get(ty);
  result = builder.CreateInsertValue(result, v, 0);
  result = builder.CreateInsertValue(result, builder.getTrue(), 1);  // is_bound = true
  // result = builder.CreateInsertValue(result, builder.getFalse(), 2);  // is_big = false
  return result;
}

using SpecificOperationGenerator = llvm::Value* (*)(llvm::IRBuilder<>&, llvm::Type*, llvm::Value*, llvm::Value*);
#define BUILDER_BIN_OP_TO_BOOL(op)                                                                 \
  ([](llvm::IRBuilder<>& builder, llvm::Type*, llvm::Value* va, llvm::Value* vb) -> llvm::Value* { \
    return builder.op(va, vb);                                                                     \
  })
#define BUILDER_BIN_OP_TO_VALUE(create, op)                                                           \
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

void GenerateGenericBinaryIntegerOperation(llvm::LLVMContext& ctx, llvm::Function* fn, SpecificOperationGenerator gen,
                                           llvm::Function* assert_is_bound_fn) {
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

void GenerateBoolGet(llvm::LLVMContext& ctx, llvm::Function* fn, llvm::Function* assert_is_bound_fn) {
  llvm::IRBuilder<> builder(ctx);

  auto* bb_entry = llvm::BasicBlock::Create(ctx, "entry", fn);
  llvm::Value* arg = fn->arg_begin();

  builder.SetInsertPoint(bb_entry);
  builder.CreateCall(assert_is_bound_fn, {arg});
  builder.CreateRet(builder.CreateExtractValue(arg, 0));
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

  using ArithmeticOpIntl = std::tuple<llvm::Function*&, llvm::Type*, std::string_view, SpecificOperationGenerator>;
  auto* const nbool_ty = builder.getInt1Ty();

  panic = llvm::Function::Create(llvm::FunctionType::get(builder.getVoidTy(), {builder.getPtrTy()}, false),
                                 llvm::GlobalValue::ExternalLinkage, "vrt_panic", mod);

  generic_getter_fn_ty = llvm::FunctionType::get(builder.getPtrTy(), {builder.getPtrTy()}, false);

  typeinfo_ty = llvm::StructType::create(ctx_, "vrt_typeinfo_t");
  typeinfo_ty->setBody({
      builder.getPtrTy(),    // const char* name
      builder.getInt8Ty(),   // vrt_typekind_e
      builder.getInt64Ty(),  // size_t bytes
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

  int_ty = llvm::StructType::create(ctx, "vrt_int_t");
  int_ty->setBody({
      builder.getInt64Ty(),  // int64 val
      builder.getInt1Ty(),   // bool is_bound
                             // builder.getInt1Ty(),   // bool is_big
  });
  //
  int_undef = llvm::ConstantStruct::get(int_ty, {
                                                    llvm::UndefValue::get(builder.getInt64Ty()),
                                                    builder.getFalse(),  // is_bound = false
                                                                         // builder.getFalse(),  // is_big = false
                                                });
  //
  auto* int_assert_is_bound_fn = declare_embedded_fn("__vrt_int_assert_is_bound", builder.getVoidTy(), {int_ty});
  GenerateAssertIsBound(*this, ctx, int_assert_is_bound_fn, "integer", 1);
  //
  for (const auto& [fptr, result_ty, fname, fnativeb] : {
           ArithmeticOpIntl(int_eq_f, nbool_ty, "__vrt_int_eq", BUILDER_BIN_OP_TO_BOOL(CreateICmpEQ)),
           ArithmeticOpIntl(int_ne_f, nbool_ty, "__vrt_int_ne", BUILDER_BIN_OP_TO_BOOL(CreateICmpNE)),
           //
           ArithmeticOpIntl(int_lt_f, nbool_ty, "__vrt_int_lt", BUILDER_BIN_OP_TO_BOOL(CreateICmpSLT)),
           ArithmeticOpIntl(int_le_f, nbool_ty, "__vrt_int_le", BUILDER_BIN_OP_TO_BOOL(CreateICmpSLE)),
           ArithmeticOpIntl(int_gt_f, nbool_ty, "__vrt_int_gt", BUILDER_BIN_OP_TO_BOOL(CreateICmpSGT)),
           ArithmeticOpIntl(int_ge_f, nbool_ty, "__vrt_int_ge", BUILDER_BIN_OP_TO_BOOL(CreateICmpSGE)),
           //
           ArithmeticOpIntl(int_add_f, int_ty, "__vrt_int_add", BUILDER_BIN_OP_TO_VALUE(CreateBoundInt, CreateAdd)),
           ArithmeticOpIntl(int_sub_f, int_ty, "__vrt_int_sub", BUILDER_BIN_OP_TO_VALUE(CreateBoundInt, CreateSub)),
           ArithmeticOpIntl(int_mul_f, int_ty, "__vrt_int_mul", BUILDER_BIN_OP_TO_VALUE(CreateBoundInt, CreateMul)),
           ArithmeticOpIntl(int_div_f, int_ty, "__vrt_int_div", BUILDER_BIN_OP_TO_VALUE(CreateBoundInt, CreateSDiv)),
       }) {
    fptr = declare_embedded_fn(fname, result_ty, {int_ty, int_ty});
    GenerateGenericBinaryIntegerOperation(ctx, fptr, fnativeb, int_assert_is_bound_fn);
  }

  bool_ty = llvm::StructType::create(ctx, "vrt_bool_t");
  bool_ty->setBody({
      builder.getInt1Ty(),  // bool value
      builder.getInt1Ty(),  // bool is_bound
  });
  //
  bool_undef = llvm::ConstantStruct::get(int_ty, {
                                                     llvm::UndefValue::get(builder.getInt1Ty()),
                                                     builder.getFalse(),  // is_bound = false
                                                 });
  //
  auto* bool_assert_is_bound_fn = declare_embedded_fn("__vrt_bool_assert_is_bound", builder.getVoidTy(), {bool_ty});
  GenerateAssertIsBound(*this, ctx, bool_assert_is_bound_fn, "boolean", 1);
  //
  bool_get_f = llvm::Function::Create(llvm::FunctionType::get(builder.getInt1Ty(), {bool_ty}, false),
                                      llvm::GlobalValue::InternalLinkage, "_vrt_bool_get", mod);
  GenerateBoolGet(ctx, bool_get_f, bool_assert_is_bound_fn);

  charstring_ty = llvm::StructType::create(ctx, "vrt_charstring_t");
  charstring_ty->setBody({
      builder.getPtrTy(),    // char* data
      builder.getInt32Ty(),  // u32 capacity
      builder.getInt32Ty(),  // u32 length
      builder.getInt1Ty(),   // bool is_bound
      builder.getInt1Ty(),   // bool is_ext
  });
  //
  charstring_dtor_f = declare_external_fn("vrt_charstring_dtor", builder.getVoidTy(), {builder.getPtrTy()});
  charstring_init_f = declare_external_fn("vrt_charstring_init", builder.getVoidTy(),
                                          {
                                              builder.getPtrTy(),    // vrt_charstring_t*
                                              builder.getPtrTy(),    // const char*
                                              builder.getInt32Ty(),  // u32
                                          });
  charstring_copy_f = declare_external_fn("copy_charstring", builder.getVoidTy(),
                                          {
                                              builder.getPtrTy(),  // vrt_charstring_t* dst
                                              builder.getPtrTy(),  // vrt_charstring_t  src
                                          });
  charstring_concat_f = declare_external_fn("vrt_charstring_concat", builder.getVoidTy(),
                                            {
                                                builder.getPtrTy(),  // vrt_charstring_t* dst
                                                builder.getPtrTy(),  // lhs
                                                builder.getPtrTy(),  // rhs
                                            });                      // TODO: add nocapture flags, ...
  charstring_singular_f = declare_external_fn("vrt_charstring_singular", builder.getVoidTy(),
                                              {
                                                  builder.getPtrTy(),  // vrt_charstring_t* dst
                                                  builder.getPtrTy(),
                                                  builder.getInt32Ty(),
                                              });
  //
  charstring_undef = llvm::ConstantStruct::get(charstring_ty, {
                                                                  llvm::UndefValue::get(builder.getPtrTy()),    //
                                                                  llvm::UndefValue::get(builder.getInt32Ty()),  //
                                                                  llvm::UndefValue::get(builder.getInt32Ty()),  //
                                                                  builder.getTrue(),   // is_bound = true
                                                                  builder.getFalse(),  // is_ext = true
                                                              });
}

llvm::Value* RuntimeBindings::GetInt(std::variant<NativeIntType, std::string_view> value) const {
  if (const auto* v = std::get_if<NativeIntType>(&value)) {
    return llvm::ConstantStruct::get(int_ty, {
                                                 llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), *v, true),
                                                 llvm::ConstantInt::getTrue(ctx_),  // is_bound = true
                                                 //  llvm::ConstantInt::getFalse(ctx_),  // is_big = false
                                             });
  }
  // TODO: big ints -- call something like vrt_int_from_str()
  std::unreachable();
}

llvm::Value* RuntimeBindings::GetBool(bool v) const {
  return llvm::ConstantStruct::get(bool_ty, {
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
