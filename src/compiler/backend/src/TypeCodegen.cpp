#include <print>

#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/lib/Assert.h>

#include "vanadium/compiler/Codegen.h"
#include "vanadium/core/Semantic.h"

namespace vanadium::compiler {

namespace {

llvm::Function* CodegenStructGetter(CodegenContext& ctx, llvm::StructType* sty, std::size_t idx,
                                    const core::semantic::Symbol* struct_sym, const core::semantic::Symbol* member_sym,
                                    std::string_view member_name) {
  auto* m_getter_fn = ctx.declareExternalFunc(names::Getter(struct_sym, member_name), ctx.rt.generic_getter_fn_ty);
  auto* p_arg = m_getter_fn->arg_begin();

  auto* bb_entry = llvm::BasicBlock::Create(ctx.llvm_ctx, "", m_getter_fn);
  ctx.builder.SetInsertPoint(bb_entry);

  if (member_sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    ctx.builder.CreateRet(ctx.builder.CreateStructGEP(sty, p_arg, idx));
    return m_getter_fn;
  }

  llvm::Value* m_ptr = ctx.builder.CreateStructGEP(sty, p_arg, 0, "m_ptr");
  llvm::Value* m_val = ctx.builder.CreateLoad(ctx.builder.getPtrTy(), m_ptr, "m_val");

  auto* bb_init = llvm::BasicBlock::Create(ctx.llvm_ctx, "init", m_getter_fn);
  auto* bb_merge = llvm::BasicBlock::Create(ctx.llvm_ctx, "merge", m_getter_fn);

  ctx.builder.CreateCondBr(ctx.builder.CreateIsNull(m_val), bb_init, bb_merge);

  //=== INIT ===//
  ctx.builder.SetInsertPoint(bb_init);
  llvm::Value* new_m = ctx.builder.CreateCall(
      ctx.rt.type_alloc_f, {ctx.getOrDeclareExternalConst(names::TInfo(member_sym), ctx.rt.typeinfo_ty)}, "new_m");
  ctx.builder.CreateStore(new_m, m_ptr);
  ctx.builder.CreateBr(bb_merge);

  //=== MERGE ===//
  ctx.builder.SetInsertPoint(bb_merge);
  //
  llvm::PHINode* ret = ctx.builder.CreatePHI(ctx.builder.getPtrTy(), 2, "ret");
  ret->addIncoming(m_val, bb_entry);
  ret->addIncoming(new_m, bb_init);
  //
  ctx.builder.CreateRet(ret);

  return m_getter_fn;
}

void CodegenStruct(CodegenContext& ctx, const core::semantic::Symbol* sym, const ast::nodes::StructTypeDecl* m) {
  auto* ty = llvm::StructType::create(ctx.llvm_ctx, ctx.sf.Text(*m->name));

  std::vector<llvm::Type*> body;
  body.reserve(m->fields.size());
  for (const auto& [idx, f] : m->fields | std::views::enumerate) {
    const auto& fsym = ctx.sf.module->scope->Resolve(ctx.sf.Text(f->type));
    body.push_back(ctx.GetSymbolType(fsym));
  }
  ty->setBody(body);
  const auto& ty_bytes = ctx.mod.getDataLayout().getTypeAllocSize(ty);

  llvm::IRBuilder<> fn_ctor_builder(ctx.llvm_ctx);
  auto* fn_ctor = ctx.getOrDeclareExternalFunc(names::Ctor(sym), ctx.rt.type_ctor_fn_ty);
  fn_ctor_builder.SetInsertPoint(llvm::BasicBlock::Create(ctx.llvm_ctx, "", fn_ctor));
  //
  llvm::IRBuilder<> fn_dtor_builder(ctx.llvm_ctx);
  auto* fn_dtor = ctx.getOrDeclareExternalFunc(names::Dtor(sym), ctx.rt.type_dtor_fn_ty);
  fn_dtor_builder.SetInsertPoint(llvm::BasicBlock::Create(ctx.llvm_ctx, "", fn_dtor));
  auto* fn_dtor_arg = fn_dtor->arg_begin();
  for (const auto& [idx, f] : m->fields | std::views::enumerate) {
    const auto& fsym = ctx.sf.module->scope->Resolve(ctx.sf.Text(f->type));

    CodegenStructGetter(ctx, ty, idx, sym, fsym, ctx.sf.Text(*f->name));

    if (f->optional) {
      fn_dtor_builder.CreateCall(ctx.rt.optional_dtor_f, {
                                                             fn_dtor_builder.CreateStructGEP(ty, fn_dtor_arg, idx),
                                                         });
    } else if (!(fsym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
      fn_dtor_builder.CreateCall(ctx.mod.getOrInsertFunction(names::Dtor(fsym), ctx.rt.type_dtor_fn_ty),
                                 {
                                     fn_dtor_builder.CreateStructGEP(ty, fn_dtor_arg, idx),
                                 });
    }
  }
  fn_ctor_builder.CreateMemSet(fn_ctor->arg_begin(), ctx.builder.getInt8(0),
                               ctx.builder.getInt64(ty_bytes.getFixedValue()),
                               llvm::MaybeAlign(ctx.mod.getDataLayout().getPrefTypeAlign(ty)));
  fn_ctor_builder.CreateRetVoid();
  //
  fn_dtor_builder.CreateRetVoid();

  ctx.getOrDeclareExternalConst(names::TInfo(sym), ctx.rt.typeinfo_ty)
      ->setInitializer(llvm::ConstantStruct::get(ctx.rt.typeinfo_ty,
                                                 {
                                                     ctx.builder.CreateGlobalStringPtr(sym->GetName(), "", 0, &ctx.mod),
                                                     ctx.builder.getInt8(0),
                                                     ctx.builder.getInt64(ty_bytes.getFixedValue()),
                                                     llvm::ConstantPointerNull::get(ctx.builder.getPtrTy()),
                                                     llvm::ConstantExpr::getBitCast(fn_ctor, ctx.builder.getPtrTy()),
                                                     llvm::ConstantExpr::getBitCast(fn_dtor, ctx.builder.getPtrTy()),
                                                 }));
}

}  // namespace

void CodegenType(CodegenContext& ctx, const core::semantic::Symbol* sym) {
  assert(sym->Flags() & core::semantic::SymbolFlags::kType);
  const auto* n = sym->Declaration();
  switch (n->nkind) {
    case ast::NodeKind::StructTypeDecl:
      CodegenStruct(ctx, sym, n->As<ast::nodes::StructTypeDecl>());
      break;
    default:
      VANADIUM_DEBUG_ERROR("Unhandled node: {}", magic_enum::enum_name(n->nkind));
      break;
  }
}

}  // namespace vanadium::compiler
