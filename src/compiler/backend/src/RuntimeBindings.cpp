#include "vanadium/compiler/RuntimeBindings.h"

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
struct TypeAndPanic {
  llvm::StructType* ty;
  llvm::Function* panic;
};

llvm::Value* CreateBoundInt(llvm::IRBuilder<>& builder, llvm::Type* ty, llvm::Value* v) {
  llvm::Value* result = llvm::UndefValue::get(ty);
  result = builder.CreateInsertValue(result, v, 0);
  result = builder.CreateInsertValue(result, builder.getTrue(), 1);   // is_bound = true
  result = builder.CreateInsertValue(result, builder.getFalse(), 2);  // is_big = false
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

void GenerateGenericIntegerOperation(llvm::LLVMContext& ctx, llvm::Module&, const TypeAndPanic& tp, llvm::Function* fn,
                                     SpecificOperationGenerator gen) {
  llvm::IRBuilder<> builder(ctx);

  auto* const bb_entry = llvm::BasicBlock::Create(ctx, "entry", fn);
  auto* const bb_unbound = llvm::BasicBlock::Create(ctx, "unbound", fn);
  auto* const bb_proceed = llvm::BasicBlock::Create(ctx, "proceed", fn);

  //
  auto* arg_it = fn->arg_begin();
  llvm::Value* a = arg_it++;
  llvm::Value* b = arg_it++;
  //

  //=== ENTRY ===//
  builder.SetInsertPoint(bb_entry);
  //
  llvm::Value* const a_bound = builder.CreateExtractValue(a, 1);
  llvm::Value* const b_bound = builder.CreateExtractValue(b, 1);
  //
  llvm::Value* const both_bound = builder.CreateAnd(a_bound, b_bound);
  builder.CreateCondBr(both_bound, bb_proceed, bb_unbound);

  //=== TRAP ===//
  builder.SetInsertPoint(bb_unbound);
  builder.CreateCall(tp.panic, builder.CreateGlobalStringPtr("UNBOUND VALUE"));
  builder.CreateUnreachable();

  //=== PROCEED ===//
  builder.SetInsertPoint(bb_proceed);
  //
  llvm::Value* const a_val = builder.CreateExtractValue(a, 0);
  llvm::Value* const b_val = builder.CreateExtractValue(b, 0);
  //
  builder.CreateRet(gen(builder, tp.ty, a_val, b_val));
}

}  // namespace

RuntimeBindings::RuntimeBindings(llvm::LLVMContext& ctx, llvm::Module& mod) : ctx_(ctx), mod_(mod) {
  llvm::IRBuilder<> builder(ctx);

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
  type_ctor_fn_ty = llvm::FunctionType::get(builder.getVoidTy(), {builder.getPtrTy()}, false);
  type_dtor_fn_ty = llvm::FunctionType::get(builder.getVoidTy(), {builder.getPtrTy()}, false);

  type_alloc_f = llvm::Function::Create(llvm::FunctionType::get(builder.getPtrTy(),    // return void*
                                                                {builder.getPtrTy()},  // const vrt_typeinfo_t*
                                                                false),
                                        llvm::GlobalValue::ExternalLinkage, "vrt_allocate", mod);
  type_free_f = llvm::Function::Create(llvm::FunctionType::get(builder.getVoidTy(),
                                                               {
                                                                   builder.getPtrTy(),  // const vrt_typeinfo_t*
                                                                   builder.getPtrTy(),  // void* p
                                                               },
                                                               false),
                                       llvm::GlobalValue::ExternalLinkage, "vrt_free", mod);

  generic_val_ty = llvm::StructType::create(ctx_, "vrt_val_t");
  generic_val_ty->setBody({
      builder.getPtrTy(),  // void* p
      builder.getPtrTy()   // const vrt_typeinfo_t* ty
  });

  log_f = llvm::Function::Create(llvm::FunctionType::get(builder.getVoidTy(),
                                                         {
                                                             builder.getPtrTy(),    // const vrt_val_t* args
                                                             builder.getInt64Ty(),  // std::size_t n
                                                         },
                                                         false),
                                 llvm::GlobalValue::ExternalLinkage, "vrt_log", mod);
  log_f->addParamAttr(0, llvm::Attribute::NoCapture);
  log_f->addParamAttr(0, llvm::Attribute::ReadOnly);

  optional_ty = llvm::StructType::create(ctx, "vrt_optional_t");
  optional_ty->setBody({
      builder.getPtrTy(),   // void* value
      builder.getPtrTy(),   // const vrt_typeinfo_t* member_type
      builder.getInt1Ty(),  // bool is_present
  });
  //
  // TODO: embed inlined code for functions below
  optional_get_f = llvm::Function::Create(llvm::FunctionType::get(builder.getPtrTy(), {builder.getPtrTy()}, false),
                                          llvm::GlobalValue::ExternalLinkage, "vrt_optional_get", mod);
  optional_set_f = llvm::Function::Create(llvm::FunctionType::get(builder.getVoidTy(),
                                                                  {
                                                                      builder.getPtrTy(),  // vrt_optional_t*
                                                                      builder.getPtrTy(),  // void*
                                                                  },
                                                                  false),
                                          llvm::GlobalValue::ExternalLinkage, "vrt_optional_set", mod);
  optional_ispresent_f =
      llvm::Function::Create(llvm::FunctionType::get(builder.getInt1Ty(), {builder.getPtrTy()}, false),
                             llvm::GlobalValue::ExternalLinkage, "vrt_optional_is_present", mod);
  optional_dtor_f = llvm::Function::Create(llvm::FunctionType::get(builder.getVoidTy(), {builder.getPtrTy()}, false),
                                           llvm::GlobalValue::ExternalLinkage, "vrt_optional_dtor", mod);

  int_ty = llvm::StructType::create(ctx, "vrt_int_t");
  int_ty->setBody({
      llvm::Type::getInt64Ty(ctx),  // int64 val
      llvm::Type::getInt1Ty(ctx),   // bool is_bound
      llvm::Type::getInt1Ty(ctx),   // bool is_big
  });
  //
  int_undef = llvm::ConstantStruct::get(int_ty, {
                                                    llvm::UndefValue::get(llvm::Type::getInt64Ty(ctx)),
                                                    llvm::ConstantInt::getFalse(ctx),  // is_bound = false
                                                    llvm::ConstantInt::getFalse(ctx),  // is_big = false
                                                });
  //
  for (const auto& [fptr, result_ty, fname, fnativeb] : {
           ArithmeticOpIntl(int_eq_f, nbool_ty, "_vrt_int_eq", BUILDER_BIN_OP_TO_BOOL(CreateICmpEQ)),
           ArithmeticOpIntl(int_ne_f, nbool_ty, "_vrt_int_ne", BUILDER_BIN_OP_TO_BOOL(CreateICmpNE)),
           //
           ArithmeticOpIntl(int_lt_f, nbool_ty, "_vrt_int_lt", BUILDER_BIN_OP_TO_BOOL(CreateICmpSLT)),
           ArithmeticOpIntl(int_le_f, nbool_ty, "_vrt_int_le", BUILDER_BIN_OP_TO_BOOL(CreateICmpSLE)),
           ArithmeticOpIntl(int_gt_f, nbool_ty, "_vrt_int_gt", BUILDER_BIN_OP_TO_BOOL(CreateICmpSGT)),
           ArithmeticOpIntl(int_ge_f, nbool_ty, "_vrt_int_ge", BUILDER_BIN_OP_TO_BOOL(CreateICmpSGE)),
           //
           ArithmeticOpIntl(int_add_f, int_ty, "_vrt_int_add", BUILDER_BIN_OP_TO_VALUE(CreateBoundInt, CreateAdd)),
           ArithmeticOpIntl(int_sub_f, int_ty, "_vrt_int_sub", BUILDER_BIN_OP_TO_VALUE(CreateBoundInt, CreateSub)),
           ArithmeticOpIntl(int_mul_f, int_ty, "_vrt_int_mul", BUILDER_BIN_OP_TO_VALUE(CreateBoundInt, CreateMul)),
           ArithmeticOpIntl(int_div_f, int_ty, "_vrt_int_div", BUILDER_BIN_OP_TO_VALUE(CreateBoundInt, CreateSDiv)),
       }) {
    fptr = llvm::Function::Create(llvm::FunctionType::get(result_ty, {int_ty, int_ty}, false),
                                  llvm::GlobalValue::InternalLinkage, fname, mod);
    fptr->addFnAttr(llvm::Attribute::AlwaysInline);
    fptr->addFnAttr(llvm::Attribute::Hot);
    GenerateGenericIntegerOperation(ctx, mod, {.ty = int_ty, .panic = panic}, fptr, fnativeb);
  }

  charstring_ty = llvm::StructType::create(ctx, "vrt_charstring_t");
  charstring_ty->setBody({
      builder.getPtrTy(),    // const char* value
      builder.getInt32Ty(),  // u32 length
      builder.getInt1Ty(),   // bool is_bound
  });
  //
  charstring_undef = llvm::ConstantStruct::get(charstring_ty, {
                                                                  llvm::UndefValue::get(builder.getPtrTy()),
                                                                  llvm::UndefValue::get(builder.getInt32Ty()),
                                                                  builder.getFalse(),  // is_bound = false
                                                              });
}

llvm::Value* RuntimeBindings::GetInt(std::int64_t value) const {
  return llvm::ConstantStruct::get(int_ty, {
                                               llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), value, true),
                                               llvm::ConstantInt::getTrue(ctx_),   // is_bound = true
                                               llvm::ConstantInt::getFalse(ctx_),  // is_big = false
                                           });
}

[[nodiscard]] llvm::Value* RuntimeBindings::GetCharstring(std::string_view value) const {
  llvm::IRBuilder<> builder(ctx_);
  return llvm::ConstantStruct::get(charstring_ty, {
                                                      builder.CreateGlobalStringPtr(value, "", 0, &mod_),
                                                      builder.getInt32(value.size()),  // length
                                                      builder.getTrue(),               // is_bound = true
                                                  });
}

}  // namespace vanadium::compiler
