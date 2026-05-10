#include <algorithm>
#include <ranges>
#include <utility>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <llvm-19/llvm/IR/Constants.h>
#include <llvm-19/llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/lib/Assert.h>

#include "vanadium/compiler/Codegen.h"
#include "vanadium/compiler/TypeSymbol.h"

namespace vanadium::compiler {

namespace {

llvm::Function* CodegenStructValueGetter(CodegenUnit& u, llvm::StructType* sty, std::size_t idx, TypeSymbol struct_sym,
                                         const core::semantic::Symbol* member_sym, std::string_view member_name) {
  auto* fn = u.getOrDeclareExternalFunc(names::Getter(struct_sym, member_name), u.rt.generic_getter_fn_ty);
  auto* bb = llvm::BasicBlock::Create(u.ctx, "", fn);
  u.builder.SetInsertPoint(bb);

  auto* p_arg = fn->arg_begin();

  auto* v = u.builder.CreateStructGEP(sty, p_arg, idx);
  if (u.IsOpaque(member_sym)) {
    // V**
    v = u.builder.CreateLoad(u.builder.getPtrTy(), v);
  }  // else V*
  u.builder.CreateRet(v);

  return fn;
}
llvm::Function* CodegenStructValueMuttor(CodegenUnit& u, llvm::StructType* sty, std::size_t idx, TypeSymbol struct_sym,
                                         const core::semantic::Symbol* member_sym, std::string_view member_name) {
  auto* fn = u.getOrDeclareExternalFunc(names::Muttor(struct_sym, member_name), u.rt.generic_getter_fn_ty);
  auto* bb = llvm::BasicBlock::Create(u.ctx, "", fn);
  u.builder.SetInsertPoint(bb);

  auto* p_arg = fn->arg_begin();

  auto* v = u.builder.CreateStructGEP(sty, p_arg, idx);
  if (u.IsOpaque(member_sym)) {
    // V**
    v = u.builder.CreateLoad(u.builder.getPtrTy(), v);
  }  // else V*
  // TODO: recycle fn
  if (!u.IsTrivial(member_sym)) {
    u.builder.CreateCall(u.getOrDeclareExternalFunc(names::Dtor(member_sym), u.rt.obj_dtor_fn_ty), {v});
    u.builder.CreateCall(u.getOrDeclareExternalFunc(names::Ctor(member_sym), u.rt.obj_ctor_fn_ty), {v});
  }

  u.builder.CreateRet(v);

  return fn;
}
// TODO: cleanup/unify(?) CodegenStructValueType & CodegenStructTemplateType
void CodegenStructValueType(CodegenUnit& u, const core::semantic::Symbol* sym, const ast::nodes::StructTypeDecl* m,
                            std::span<const core::semantic::Symbol*> members) {
  auto* sty = llvm::StructType::create(u.ctx, names::Type(sym));
  // https://github.com/llvm/llvm-project/issues/101614
  std::vector<llvm::Type*> body =
      std::ranges::to<std::vector>(members | std::views::transform([&](const auto* msym) -> llvm::Type* {
                                     return u.GetSymbolType(msym);
                                   }));
  sty->setBody(body);

  auto* fn_ctor = [&] -> llvm::Function* {
    llvm::Function* cfn = u.getOrDeclareExternalFunc(names::Ctor(sym), u.rt.obj_ctor_fn_ty);
    u.builder.SetInsertPoint(llvm::BasicBlock::Create(u.ctx, "", cfn));
    //
    auto* p_arg = cfn->arg_begin();
    for (std::size_t idx = 0; idx < members.size(); ++idx) {
      const auto& fts = members[idx];
      auto* fv = u.builder.CreateStructGEP(sty, p_arg, idx);
      if (u.IsOpaque(fts)) {
        auto* aptrv = u.builder.CreateCall(u.rt.type_new_f, {u.GetTypeInfo(fts), fv});
        u.builder.CreateStore(aptrv, fv);
      } else if (!u.IsTrivial(fts)) {
        u.builder.CreateCall(u.getOrDeclareExternalFunc(names::Ctor(fts), u.rt.obj_ctor_fn_ty), {fv});
      }
    }
    //
    u.builder.CreateRetVoid();
    return cfn;
  }();

  auto* fn_dtor = [&] -> llvm::Function* {
    auto* dfn = u.getOrDeclareExternalFunc(names::Dtor(sym), u.rt.obj_dtor_fn_ty);
    u.builder.SetInsertPoint(llvm::BasicBlock::Create(u.ctx, "", dfn));
    //
    auto* p_arg = dfn->arg_begin();
    for (std::size_t idx = 0; idx < members.size(); ++idx) {
      if (!u.IsTrivial(body[idx])) {
        u.EmitDestructorInvocation(members[idx], u.builder.CreateStructGEP(sty, p_arg, idx));
      }
    }
    //
    u.builder.CreateRetVoid();
    return dfn;
  }();

  const auto* sty_layout = u.mod.getDataLayout().getStructLayout(sty);
  std::vector<llvm::Constant*> member_descriptors;
  member_descriptors.reserve(members.size());
  for (const auto& [idx, f] : m->fields | std::views::enumerate) {
    const auto& fts = members[idx];
    CodegenStructValueGetter(u, sty, idx, sym, fts, u.sf.Text(*f->name));
    CodegenStructValueMuttor(u, sty, idx, sym, fts, u.sf.Text(*f->name));

    member_descriptors.emplace_back(
        llvm::ConstantStruct::get(u.rt.smember_ty, {
                                                       u.builder.CreateGlobalStringPtr(u.sf.Text(*f->name)),
                                                       u.GetTypeInfo(fts),
                                                       u.builder.getInt64(sty_layout->getElementOffset(idx)),
                                                   }));
  }
  auto* member_descriptors_array =
      llvm::ConstantArray::get(llvm::ArrayType::get(u.rt.smember_ty, member_descriptors.size()), member_descriptors);

  u.getOrDeclareExternalConst(names::TInfo(sym), u.rt.typeinfo_ty)
      ->setInitializer(llvm::ConstantStruct::get(
          u.rt.typeinfo_ty,
          {
              u.builder.CreateGlobalStringPtr(sym->GetName()),             // name
              u.builder.getInt8(std::to_underlying(RtTypeKind::kRecord)),  // kind
              u.rt.GetSizeI(u.mod.getDataLayout().getTypeAllocSize(sty)),  // size
              new llvm::GlobalVariable(u.mod, member_descriptors_array->getType(), true,
                                       llvm::GlobalValue::PrivateLinkage, member_descriptors_array,
                                       std::format("{}_members", names::Type(sym))),  // *members
              u.rt.GetSizeI(member_descriptors.size()),                               // members_count
              fn_ctor,                                                                // ctor
              fn_dtor,                                                                // dtor
              llvm::ConstantPointerNull::get(u.builder.getPtrTy()),                   // copy
              u.GetTypeInfo({sym, true}),                                             // counterpart
              llvm::ConstantPointerNull::get(u.builder.getPtrTy()),                   // tpl_construct_value
          }));
}

//

llvm::Function* CodegenStructTemplateGetter(CodegenUnit& u, llvm::StructType* specific_sty, std::size_t idx,
                                            TypeSymbol struct_sym, TypeSymbol member_sym,
                                            std::string_view member_name) {
  auto* fn = u.getOrDeclareExternalFunc(names::Getter(struct_sym, member_name), u.rt.generic_getter_fn_ty);
  auto* bb = llvm::BasicBlock::Create(u.ctx, "", fn);
  u.builder.SetInsertPoint(bb);

  auto* p_arg = fn->arg_begin();

  auto* v = u.builder.CreateStructGEP(specific_sty, p_arg, idx);
  if (u.IsOpaque(member_sym)) {
    // V**
    v = u.builder.CreateLoad(u.builder.getPtrTy(), v);
  }  // else V*
  u.builder.CreateRet(v);

  return fn;
}
llvm::Function* CodegenStructTemplateMuttor(CodegenUnit& u, llvm::StructType* specific_sty, std::size_t idx,
                                            TypeSymbol struct_sym, TypeSymbol member_sym,
                                            std::string_view member_name) {
  auto* fn = u.getOrDeclareExternalFunc(names::Muttor(struct_sym, member_name), u.rt.generic_getter_fn_ty);
  auto* bb = llvm::BasicBlock::Create(u.ctx, "", fn);
  u.builder.SetInsertPoint(bb);

  auto* p_arg = fn->arg_begin();

  auto* v = u.builder.CreateStructGEP(specific_sty, p_arg, idx);
  if (u.IsOpaque(member_sym)) {
    // V**
    v = u.builder.CreateLoad(u.builder.getPtrTy(), v);
  }  // else V*
  u.builder.CreateRet(v);

  return fn;
}
void CodegenStructTemplateType(CodegenUnit& u, const core::semantic::Symbol* semsym,
                               const ast::nodes::StructTypeDecl* m,
                               std::span<const core::semantic::Symbol*> semmembers) {
  const TypeSymbol sym{semsym, true};
  const auto members = semmembers | std::views::transform([](const auto* msym) {
                         return TypeSymbol{msym, true};
                       });

  auto* specific_sty = llvm::StructType::create(u.ctx, std::format("{}_specific", names::Type(sym)));
  std::vector<llvm::Type*> body =
      std::ranges::to<std::vector>(members | std::views::transform([&](const auto& msym) -> llvm::Type* {
                                     return u.GetSymbolType(msym);
                                   }));
  specific_sty->setBody(body);
  auto* sty = llvm::StructType::create(u.rt.WrapTemplateStruct(specific_sty), names::Type(sym));

  auto* fn_ctor = [&] -> llvm::Function* {
    llvm::Function* cfn = u.getOrDeclareExternalFunc(names::Ctor(sym), u.rt.obj_ctor_fn_ty);
    u.builder.SetInsertPoint(llvm::BasicBlock::Create(u.ctx, "", cfn));
    auto* p_arg = cfn->arg_begin();
    u.builder.CreateCall(u.rt.tpl.generic_opaque_struct_ctor_fn, {p_arg});
    u.builder.CreateRetVoid();
    return cfn;
  }();

  auto* fn_tpl_val_ctor = [&] -> llvm::Function* {
    llvm::Function* cfn = u.getOrDeclareExternalFunc(names::TplValCtor(sym), u.rt.obj_ctor_fn_ty);
    u.builder.SetInsertPoint(llvm::BasicBlock::Create(u.ctx, "", cfn));
    //
    auto* p_arg = cfn->arg_begin();
    for (std::size_t idx = 0; idx < members.size(); ++idx) {
      const auto& fts = members[idx];
      auto* fv = u.builder.CreateStructGEP(specific_sty, p_arg, idx);
      if (u.IsOpaque(fts)) {
        auto* aptrv = u.builder.CreateCall(u.rt.type_new_f, {u.GetTypeInfo(fts), fv});
        u.builder.CreateStore(aptrv, fv);
      } else {
        u.builder.CreateCall(u.getOrDeclareExternalFunc(names::Ctor(fts), u.rt.obj_ctor_fn_ty), {fv});
      }
    }
    //
    u.builder.CreateRetVoid();
    return cfn;
  }();

  constexpr std::uint8_t kTplSpecificValue = 1;  // TODO: should be kept in sync with rt/rt_template.h
  auto* fn_dtor = [&] -> llvm::Function* {
    auto* dfn = u.getOrDeclareExternalFunc(names::Dtor(sym), u.rt.obj_dtor_fn_ty);
    //
    auto* p_arg = dfn->arg_begin();

    auto* bb_entry = llvm::BasicBlock::Create(u.ctx, "", dfn);
    auto* bb_specific_cleanup = llvm::BasicBlock::Create(u.ctx, "specific", dfn);
    auto* bb_generic_cleanup = llvm::BasicBlock::Create(u.ctx, "generic", dfn);

    //=== ENTRY ===//
    u.builder.SetInsertPoint(bb_entry);
    auto* p_tsel = u.builder.CreateStructGEP(sty, p_arg, 0);
    auto* tsel = u.builder.CreateLoad(u.builder.getInt8Ty(), p_tsel);
    llvm::Value* cond = u.builder.CreateICmpEQ(tsel, u.builder.getInt8(kTplSpecificValue));
    u.builder.CreateCondBr(cond, bb_specific_cleanup, bb_generic_cleanup);

    //=== SPECIFIC ===//
    u.builder.SetInsertPoint(bb_specific_cleanup);
    auto* p_specific = u.builder.CreateStructGEP(sty, p_arg, 1);
    for (std::size_t idx = 0; idx < members.size(); ++idx) {
      if (!u.IsTrivial(body[idx])) {
        u.EmitDestructorInvocation(members[idx], u.builder.CreateStructGEP(specific_sty, p_specific, idx));
      }
    }
    u.builder.CreateRetVoid();

    //=== GENERIC ===//
    u.builder.SetInsertPoint(bb_generic_cleanup);
    u.builder.CreateCall(u.rt.tpl.generic_opaque_struct_dtor_fn, {dfn, p_arg});
    u.builder.CreateRetVoid();
    return dfn;
  }();

  const auto* sty_layout = u.mod.getDataLayout().getStructLayout(sty);
  const auto* specific_sty_layout = u.mod.getDataLayout().getStructLayout(specific_sty);
  std::vector<llvm::Constant*> member_descriptors;
  member_descriptors.reserve(members.size());
  const auto specifics_offset = sty_layout->getElementOffset(1);
  for (const auto& [idx, f] : m->fields | std::views::enumerate) {
    const auto& fts = members[idx];
    CodegenStructTemplateGetter(u, specific_sty, idx, sym, fts, u.sf.Text(*f->name));
    CodegenStructTemplateMuttor(u, specific_sty, idx, sym, fts, u.sf.Text(*f->name));
    member_descriptors.emplace_back(llvm::ConstantStruct::get(
        u.rt.smember_ty, {
                             u.builder.CreateGlobalStringPtr(u.sf.Text(*f->name)),
                             u.GetTypeInfo(fts),
                             u.builder.getInt64(specifics_offset + specific_sty_layout->getElementOffset(idx)),
                         }));
  }
  auto* member_descriptors_array =
      llvm::ConstantArray::get(llvm::ArrayType::get(u.rt.smember_ty, member_descriptors.size()), member_descriptors);

  u.getOrDeclareExternalConst(names::TInfo(sym), u.rt.typeinfo_ty)
      ->setInitializer(llvm::ConstantStruct::get(
          u.rt.typeinfo_ty,
          {
              u.builder.CreateGlobalStringPtr(sym->GetName()),                             // name
              u.builder.getInt8(std::to_underlying(RtTypeKind::kRecord)),                  // kind
              u.rt.GetSizeI(u.mod.getDataLayout().getTypeAllocSize(sty).getFixedValue()),  // size
              new llvm::GlobalVariable(u.mod, member_descriptors_array->getType(), true,
                                       llvm::GlobalValue::PrivateLinkage, member_descriptors_array,
                                       std::format("{}_members", names::Type(sym))),  // *members
              u.rt.GetSizeI(member_descriptors.size()),                               // members_count
              fn_ctor,                                                                // ctor
              fn_dtor,                                                                // dtor
              llvm::ConstantPointerNull::get(u.builder.getPtrTy()),                   // copy
              u.GetTypeInfo({sym, false}),                                            // counterpart
              fn_tpl_val_ctor,                                                        // tpl_construct_value
          }));
}

void CodegenStruct(CodegenUnit& u, const core::semantic::Symbol* sym, const ast::nodes::StructTypeDecl* m) {
  std::vector<const core::semantic::Symbol*> members;
  members.reserve(m->fields.size());
  for (const auto& [idx, f] : m->fields | std::views::enumerate) {
    // TODO: use checker::ResolveTypeSymbol, support for anonymous TypeSpec/ types
    const auto* fsym = u.sf.module->scope->Resolve(u.sf.Text(f->type));
    assert(fsym);
    members.emplace_back(fsym);
  }

  CodegenStructValueType(u, sym, m, members);
  CodegenStructTemplateType(u, sym, m, members);
}

}  // namespace

void CodegenType(CodegenUnit& u, const core::semantic::Symbol* sym) {
  assert(sym->Flags() & core::semantic::SymbolFlags::kType);
  const auto* n = sym->Declaration();
  switch (n->nkind) {
    case ast::NodeKind::StructTypeDecl:
      CodegenStruct(u, sym, n->As<ast::nodes::StructTypeDecl>());
      break;
    default:
      VANADIUM_DEBUG_ERROR("Unhandled node: {}", magic_enum::enum_name(n->nkind));
      break;
  }
}

}  // namespace vanadium::compiler
