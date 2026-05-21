#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/Argument.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/lib/Assert.h>
#include <vanadium/lib/Metaprogramming.h>

#include "vanadium/compiler/Codegen.h"
#include "vanadium/compiler/RuntimeBindings.h"
#include "vanadium/compiler/TypeSymbol.h"

namespace vanadium::compiler {

namespace {

struct IntermediateStructDescriptor {
  RtTypeKind kind;
  //
  std::size_t size;
  std::size_t alignment;
  //
  const std::vector<llvm::Constant*>& member_descriptors;
  //
  llvm::Constant* ctor_fn;
  llvm::Constant* dtor_fn;
  llvm::Constant* copy_fn;
  llvm::Constant* tplval_ctor_fn;
};

void EmitStructuralTypeDescriptor(CodegenUnit& u, TypeSymbol sym, const IntermediateStructDescriptor& d) {
  auto* member_descriptors_array = llvm::ConstantArray::get(
      llvm::ArrayType::get(u.rt.smember_ty, d.member_descriptors.size()), d.member_descriptors);
  auto* members_gv =
      new llvm::GlobalVariable(u.mod, member_descriptors_array->getType(), true, llvm::GlobalValue::PrivateLinkage,
                               member_descriptors_array, std::format("{}_members", names::Type(sym)));

  u.getOrDeclareExternalConst(names::TInfo(sym), u.rt.typeinfo_ty)
      ->setInitializer(llvm::ConstantStruct::get(  //
          u.rt.typeinfo_ty,
          {
              u.builder.CreateGlobalStringPtr(sym->GetName()),  // name
              u.builder.getInt8(std::to_underlying(d.kind)),    // kind
              u.rt.GetSizeI(d.size),                            // size
              u.rt.GetSizeI(d.alignment),                       // size
              members_gv,                                       // *members
              u.rt.GetSizeI(d.member_descriptors.size()),       // members_count
              d.ctor_fn,                                        // ctor
              d.dtor_fn,                                        // dtor
              d.copy_fn,                                        // copy
              u.GetTypeInfo({sym, !sym.is_template}),           // counterpart
              d.tplval_ctor_fn,                                 // tpl_construct_value
          }));
}

llvm::Function* GenerateSingleArgumentFunction(CodegenUnit& u, std::string_view name, llvm::FunctionType* fty,
                                               mp::Consumer<llvm::Argument*> auto gen) {
  llvm::Function* cfn = u.getOrDeclareExternalFunc(name, fty);
  u.builder.SetInsertPoint(llvm::BasicBlock::Create(u.ctx, "", cfn));
  auto* p_arg = cfn->arg_begin();
  gen(p_arg);
  return cfn;
}

llvm::Function* CodegenStructGetterFn(CodegenUnit& u, std::string_view name,
                                      mp::Invocable<llvm::Value*, llvm::Argument*> auto gen) {
  return GenerateSingleArgumentFunction(u, name, u.rt.generic_getter_fn_ty, [&](llvm::Argument* pa) {
    u.builder.CreateRet(gen(pa));
  });
}

auto MemberSymbolsToTypesView(CodegenUnit& u, auto members) {
  return members | std::views::transform([&](const auto& msym) -> llvm::Type* {
           return u.GetSymbolType(msym);
         });
}

//

void CodegenStructValueType(RtTypeKind kind, CodegenUnit& u, const core::semantic::Symbol* sym,
                            std::span<const ast::nodes::Field* const> fields,
                            std::span<const core::semantic::Symbol* const> members) {
  auto* sty = llvm::StructType::create(u.ctx, names::Type(sym));
  sty->setBody(std::ranges::to<std::vector>(MemberSymbolsToTypesView(u, members)));
  const auto* sty_layout = u.mod.getDataLayout().getStructLayout(sty);

  std::vector<llvm::Constant*> member_descriptors;
  member_descriptors.reserve(members.size());
  for (const auto& [idx, f] : fields | std::views::enumerate) {
    const auto& fts = members[idx];
    const auto codegen_ptr_get = [&](llvm::Argument* pa) {
      auto* v = u.builder.CreateStructGEP(sty, pa, idx);
      if (u.IsOpaque(fts)) {
        // V**
        v = u.builder.CreateLoad(u.builder.getPtrTy(), v);
      }  // else V*
      return v;
    };
    CodegenStructGetterFn(u, names::Getter(sym, u.sf.Text(*f->name)), [&](llvm::Argument* pa) -> llvm::Value* {
      return codegen_ptr_get(pa);
    });

    member_descriptors.emplace_back(
        llvm::ConstantStruct::get(u.rt.smember_ty, {
                                                       u.builder.CreateGlobalStringPtr(u.sf.Text(*f->name)),
                                                       u.GetTypeInfo(fts),
                                                       u.builder.getInt64(sty_layout->getElementOffset(idx)),
                                                   }));
  }

  EmitStructuralTypeDescriptor(  //
      u, sym,
      IntermediateStructDescriptor{
          .kind = kind,
          .size = u.mod.getDataLayout().getTypeAllocSize(sty).getFixedValue(),
          .alignment = u.mod.getDataLayout().getABITypeAlign(sty).value(),
          .member_descriptors = member_descriptors,

          .ctor_fn = GenerateSingleArgumentFunction(  //
              u, names::Ctor(sym), u.rt.obj_ctor_fn_ty,
              [&](llvm::Argument* pa) {
                for (std::size_t idx = 0; idx < members.size(); ++idx) {
                  u.ConstructAt(members[idx], u.builder.CreateStructGEP(sty, pa, idx));
                }
                u.builder.CreateRetVoid();
              }),

          .dtor_fn = GenerateSingleArgumentFunction(  //
              u, names::Dtor(sym), u.rt.obj_ctor_fn_ty,
              [&](llvm::Argument* pa) {
                for (std::size_t idx = 0; idx < members.size(); ++idx) {
                  u.DestructAt(members[idx], u.builder.CreateStructGEP(sty, pa, idx));
                }
                u.builder.CreateRetVoid();
              }),

          .copy_fn = llvm::ConstantPointerNull::get(u.builder.getPtrTy()),

          .tplval_ctor_fn = llvm::ConstantPointerNull::get(u.builder.getPtrTy()),
      });
}

void CodegenStructTemplateType(RtTypeKind kind, CodegenUnit& u, const core::semantic::Symbol* semsym,
                               std::span<const ast::nodes::Field* const> fields,
                               std::span<const core::semantic::Symbol*> semmembers) {
  const TypeSymbol sym{semsym, true};
  const auto members = semmembers | std::views::transform([](const auto* msym) {
                         return TypeSymbol{msym, true};
                       });

  auto* specific_sty = llvm::StructType::create(u.ctx, std::format("{}_specific", names::Type(sym)));
  specific_sty->setBody(std::ranges::to<std::vector>(MemberSymbolsToTypesView(u, members)));
  //
  auto* sty = llvm::StructType::create(u.rt.WrapTemplateStruct(specific_sty), names::Type(sym));
  constexpr std::size_t kTplSelectionSlotIdx = 0;
  constexpr std::size_t kTplValueStorageSlotIdx = 1;

  const auto specifics_offset = u.mod.getDataLayout().getStructLayout(sty)->getElementOffset(1);
  const auto* specific_sty_layout = u.mod.getDataLayout().getStructLayout(specific_sty);

  std::vector<llvm::Constant*> member_descriptors;
  member_descriptors.reserve(members.size());
  for (const auto& [idx, f] : fields | std::views::enumerate) {
    const auto& fts = members[idx];
    const auto codegen_ptr_get = [&](llvm::Argument* pa) {
      auto* v = u.builder.CreateStructGEP(specific_sty, pa, idx);
      if (u.IsOpaque(fts)) {
        // V**
        v = u.builder.CreateLoad(u.builder.getPtrTy(), v);
      }  // else V*
      return v;
    };
    CodegenStructGetterFn(u, names::Getter(sym, u.sf.Text(*f->name)), [&](llvm::Argument* pa) {
      return codegen_ptr_get(pa);
    });

    member_descriptors.emplace_back(llvm::ConstantStruct::get(
        u.rt.smember_ty, {
                             u.builder.CreateGlobalStringPtr(u.sf.Text(*f->name)),
                             u.GetTypeInfo(fts),
                             u.builder.getInt64(specifics_offset + specific_sty_layout->getElementOffset(idx)),
                         }));
  }

  EmitStructuralTypeDescriptor(  //
      u, sym,
      IntermediateStructDescriptor{
          .kind = kind,
          .size = u.mod.getDataLayout().getTypeAllocSize(sty).getFixedValue(),
          .alignment = u.mod.getDataLayout().getABITypeAlign(sty).value(),
          .member_descriptors = member_descriptors,

          .ctor_fn = GenerateSingleArgumentFunction(  //
              u, names::Ctor(sym), u.rt.obj_ctor_fn_ty,
              [&](llvm::Argument* pa) {
                u.builder.CreateCall(u.rt.tpl.generic_opaque_struct_ctor_fn, {pa});
                u.builder.CreateRetVoid();
              }),

          .dtor_fn = [&] -> llvm::Function* {
            auto* dfn = u.getOrDeclareExternalFunc(names::Dtor(sym), u.rt.obj_dtor_fn_ty);
            //
            auto* pa = dfn->arg_begin();

            auto* bb_entry = llvm::BasicBlock::Create(u.ctx, "", dfn);
            auto* bb_specific_cleanup = llvm::BasicBlock::Create(u.ctx, "specific", dfn);
            auto* bb_generic_cleanup = llvm::BasicBlock::Create(u.ctx, "generic", dfn);

            //=== ENTRY ===//
            u.builder.SetInsertPoint(bb_entry);
            auto* p_tsel = u.builder.CreateStructGEP(sty, pa, kTplSelectionSlotIdx);
            auto* tsel = u.builder.CreateLoad(u.builder.getInt8Ty(), p_tsel);
            llvm::Value* cond =
                u.builder.CreateICmpEQ(tsel, u.builder.getInt8(std::to_underlying(RtTplSelection::kSpecificValue)));
            u.builder.CreateCondBr(cond, bb_specific_cleanup, bb_generic_cleanup);

            //=== SPECIFIC ===//
            u.builder.SetInsertPoint(bb_specific_cleanup);
            auto* p_specific = u.builder.CreateStructGEP(sty, pa, kTplValueStorageSlotIdx);
            for (std::size_t idx = 0; idx < members.size(); ++idx) {
              u.DestructAt(members[idx], u.builder.CreateStructGEP(specific_sty, p_specific, idx));
            }
            u.builder.CreateRetVoid();

            //=== GENERIC ===//
            u.builder.SetInsertPoint(bb_generic_cleanup);
            u.builder.CreateCall(u.rt.tpl.generic_opaque_struct_dtor_fn, {dfn, pa});
            u.builder.CreateRetVoid();
            return dfn;
          }(),

          .copy_fn = llvm::ConstantPointerNull::get(u.builder.getPtrTy()),

          .tplval_ctor_fn = GenerateSingleArgumentFunction(  //
              u, names::TplValCtor(sym), u.rt.obj_ctor_fn_ty,
              [&](llvm::Argument* pa) {
                for (std::size_t idx = 0; idx < members.size(); ++idx) {
                  u.ConstructAt(members[idx], u.builder.CreateStructGEP(specific_sty, pa, idx));
                }
                u.builder.CreateRetVoid();
              }),
      });
}

//

void CodegenUnionValueDtor(CodegenUnit& u, auto members, llvm::StructType* sty, llvm::Argument* pa) {
  // TODO: deduplicate those constants
  constexpr std::size_t kActiveMemberSlotIdx = 0;
  constexpr std::size_t kValueStorageSlotIdx = 1;

  auto* this_fn = u.builder.GetInsertBlock()->getParent();

  auto* uvalptr = u.builder.CreateStructGEP(sty, pa, kValueStorageSlotIdx);
  auto* active_idx =
      u.builder.CreateLoad(u.rt.uniont.active_member_idx_ty, u.builder.CreateStructGEP(sty, pa, kActiveMemberSlotIdx));

  auto* bb_merge = llvm::BasicBlock::Create(u.ctx, "merge");

  auto* bb_default = llvm::BasicBlock::Create(u.ctx, "trap", this_fn);
  //
  auto* sw = u.builder.CreateSwitch(active_idx, bb_default, 1 + members.size());
  //
  u.builder.SetInsertPoint(bb_default);
  u.builder.CreateIntrinsic(llvm::Intrinsic::trap, {}, {});
  u.builder.CreateUnreachable();

  sw->addCase(u.rt.uniont.unbound_am_idx, [&] {
    auto* bb = llvm::BasicBlock::Create(u.ctx, "unbound", this_fn);
    u.builder.SetInsertPoint(bb);
    u.builder.CreateBr(bb_merge);
    return bb;
  }());
  for (std::size_t idx = 0; idx < members.size(); ++idx) {
    sw->addCase(llvm::ConstantInt::get(u.rt.uniont.active_member_idx_ty, idx), [&] {
      auto* bb = llvm::BasicBlock::Create(u.ctx, "sel", this_fn);
      u.builder.SetInsertPoint(bb);
      u.DestructAt(members[idx], uvalptr);
      u.builder.CreateBr(bb_merge);
      return bb;
    }());
  }

  this_fn->insert(this_fn->end(), bb_merge);
  u.builder.SetInsertPoint(bb_merge);
  u.builder.CreateRetVoid();
}

void CodegenUnionValueType(CodegenUnit& u, const core::semantic::Symbol* sym,
                           std::span<const ast::nodes::Field* const> fields,
                           std::span<const core::semantic::Symbol* const> members) {
  auto* sty = llvm::StructType::create(u.ctx, names::Type(sym));
  sty->setBody({
      u.rt.uniont.active_member_idx_ty,
      u.rt.MakeUnion(std::ranges::to<std::vector>(MemberSymbolsToTypesView(u, members))),
  });
  constexpr std::size_t kActiveMemberSlotIdx = 0;
  constexpr std::size_t kValueStorageSlotIdx = 1;
  const auto* sty_layout = u.mod.getDataLayout().getStructLayout(sty);

  auto* dtor_fn = GenerateSingleArgumentFunction(  //
      u, names::Dtor(sym), u.rt.obj_dtor_fn_ty, [&](llvm::Argument* pa) {
        CodegenUnionValueDtor(u, members, sty, pa);
      });

  std::vector<llvm::Constant*> member_descriptors;
  member_descriptors.reserve(members.size());
  for (const auto& [idx, f] : fields | std::views::enumerate) {
    const auto& fts = members[idx];
    const auto codegen_ptr_get = [&](llvm::Argument* pa) {
      auto* v = u.builder.CreateStructGEP(sty, pa, kValueStorageSlotIdx);
      if (u.IsOpaque(fts)) {
        // V**
        v = u.builder.CreateLoad(u.builder.getPtrTy(), v);
      }  // else V*
      return v;
    };
    auto* am_idx_val = llvm::ConstantInt::get(u.rt.uniont.active_member_idx_ty, idx);
    const auto codegen_isselected = [&](llvm::Argument* pa) {
      auto* active_idx = u.builder.CreateLoad(u.rt.uniont.active_member_idx_ty,
                                              u.builder.CreateStructGEP(sty, pa, kActiveMemberSlotIdx));
      return u.builder.CreateICmpEQ(active_idx, am_idx_val);
    };
    CodegenStructGetterFn(u, names::Getter(sym, u.sf.Text(*f->name)), [&](llvm::Argument* pa) -> llvm::Value* {
      // TODO: format names like @Module.SymbolName
      // TODO: to avoid blowing up executable size, we don't emit tons of string literals like Titan, but anyway we
      //       should consider printing field name and union name for traceability purposes, with fmt done in runtime
      // TODO: ^ currently, the string is not interned and is emitted into IR for each instantiation, being reduced to
      //       the single instance only after an optimization pass, generify the assertion for now to avoid that
      u.builder.CreateCall(u.rt.assert_f, {
                                              codegen_isselected(pa),
                                              u.builder.CreateGlobalStringPtr("using non-selected field of a union"),
                                          });
      return codegen_ptr_get(pa);
    });
    CodegenStructGetterFn(u, names::Muttor(sym, u.sf.Text(*f->name)), [&](llvm::Argument* pa) -> llvm::Value* {
      auto* bb_reinit = llvm::BasicBlock::Create(u.ctx, "reinit", u.builder.GetInsertBlock()->getParent());
      auto* bb_ret = llvm::BasicBlock::Create(u.ctx, "ret", u.builder.GetInsertBlock()->getParent());

      //=== ENTRY ===//
      u.builder.CreateCondBr(codegen_isselected(pa), bb_ret, bb_reinit);

      //=== REINIT ===//
      u.builder.SetInsertPoint(bb_reinit);
      u.builder.CreateCall(dtor_fn, pa);
      u.ConstructAt(members[idx], u.builder.CreateStructGEP(sty, pa, kValueStorageSlotIdx));
      u.builder.CreateStore(am_idx_val, u.builder.CreateStructGEP(sty, pa, kActiveMemberSlotIdx));
      u.builder.CreateBr(bb_ret);

      //=== RET ===//
      u.builder.SetInsertPoint(bb_ret);
      return codegen_ptr_get(pa);
    });

    member_descriptors.emplace_back(llvm::ConstantStruct::get(
        u.rt.smember_ty, {
                             u.builder.CreateGlobalStringPtr(u.sf.Text(*f->name)),
                             u.GetTypeInfo(fts),
                             u.builder.getInt64(sty_layout->getElementOffset(kValueStorageSlotIdx)),
                         }));
  }

  EmitStructuralTypeDescriptor(  //
      u, sym,
      IntermediateStructDescriptor{
          .kind = RtTypeKind::kUnion,
          .size = u.mod.getDataLayout().getTypeAllocSize(sty).getFixedValue(),
          .alignment = u.mod.getDataLayout().getABITypeAlign(sty).value(),
          .member_descriptors = member_descriptors,

          .ctor_fn = GenerateSingleArgumentFunction(  //
              u, names::Ctor(sym), u.rt.obj_ctor_fn_ty,
              [&](llvm::Argument* pa) {
                u.builder.CreateStore(u.rt.uniont.unbound_am_idx,
                                      u.builder.CreateStructGEP(sty, pa, kActiveMemberSlotIdx));
                u.builder.CreateRetVoid();
              }),

          .dtor_fn = dtor_fn,

          .copy_fn = llvm::ConstantPointerNull::get(u.builder.getPtrTy()),

          .tplval_ctor_fn = llvm::ConstantPointerNull::get(u.builder.getPtrTy()),
      });
}

void CodegenUnionTemplateType(CodegenUnit& u, const core::semantic::Symbol* semsym,
                              std::span<const ast::nodes::Field* const> fields,
                              std::span<const core::semantic::Symbol*> semmembers) {
  const TypeSymbol sym{semsym, true};
  const auto members = semmembers | std::views::transform([](const auto* msym) {
                         return TypeSymbol{msym, true};
                       });

  auto* specific_sty = llvm::StructType::create(u.ctx, std::format("{}_specific", names::Type(sym)));
  specific_sty->setBody({
      u.rt.uniont.active_member_idx_ty,
      u.rt.MakeUnion(std::ranges::to<std::vector>(MemberSymbolsToTypesView(u, members))),
  });
  constexpr std::size_t kActiveMemberSlotIdx = 0;
  constexpr std::size_t kValueStorageSlotIdx = 1;

  auto* specific_dtor_fn = GenerateSingleArgumentFunction(  //
      u, names::TplValDtor(sym), u.rt.obj_dtor_fn_ty, [&](llvm::Argument* pa) {
        CodegenUnionValueDtor(u, members, specific_sty, pa);
      });

  auto* sty = llvm::StructType::create(u.rt.WrapTemplateStruct(specific_sty), names::Type(sym));
  constexpr std::size_t kTplSelectionSlotIdx = 0;
  constexpr std::size_t kTplValueStorageSlotIdx = 1;

  const auto specifics_offset = u.mod.getDataLayout().getStructLayout(sty)->getElementOffset(1);
  const auto* specific_sty_layout = u.mod.getDataLayout().getStructLayout(specific_sty);

  std::vector<llvm::Constant*> member_descriptors;
  member_descriptors.reserve(members.size());
  for (const auto& [idx, f] : fields | std::views::enumerate) {
    const auto& fts = members[idx];
    const auto codegen_ptr_get = [&](llvm::Argument* pa) {
      auto* v = u.builder.CreateStructGEP(specific_sty, pa, kValueStorageSlotIdx);
      if (u.IsOpaque(fts)) {
        // V**
        v = u.builder.CreateLoad(u.builder.getPtrTy(), v);
      }  // else V*
      return v;
    };
    auto* am_idx_val = llvm::ConstantInt::get(u.rt.uniont.active_member_idx_ty, idx);
    const auto codegen_isselected = [&](llvm::Argument* pa) {
      auto* active_idx = u.builder.CreateLoad(u.rt.uniont.active_member_idx_ty,
                                              u.builder.CreateStructGEP(specific_sty, pa, kActiveMemberSlotIdx));
      return u.builder.CreateICmpEQ(active_idx, am_idx_val);
    };
    CodegenStructGetterFn(u, names::Getter(sym, u.sf.Text(*f->name)), [&](llvm::Argument* pa) -> llvm::Value* {
      // TODO: see the TODOs in the value counterpart
      u.builder.CreateCall(u.rt.assert_f, {
                                              codegen_isselected(pa),
                                              u.builder.CreateGlobalStringPtr("using non-selected field of a union"),
                                          });
      return codegen_ptr_get(pa);
    });
    CodegenStructGetterFn(u, names::Muttor(sym, u.sf.Text(*f->name)), [&](llvm::Argument* pa) -> llvm::Value* {
      auto* bb_reinit = llvm::BasicBlock::Create(u.ctx, "reinit", u.builder.GetInsertBlock()->getParent());
      auto* bb_ret = llvm::BasicBlock::Create(u.ctx, "ret", u.builder.GetInsertBlock()->getParent());

      //=== ENTRY ===//
      u.builder.CreateCondBr(codegen_isselected(pa), bb_ret, bb_reinit);

      //=== REINIT ===//
      u.builder.SetInsertPoint(bb_reinit);
      u.builder.CreateCall(specific_dtor_fn, pa);
      u.ConstructAt(members[idx], u.builder.CreateStructGEP(specific_sty, pa, kValueStorageSlotIdx));
      u.builder.CreateStore(am_idx_val, u.builder.CreateStructGEP(specific_sty, pa, kActiveMemberSlotIdx));
      u.builder.CreateBr(bb_ret);

      //=== RET ===//
      u.builder.SetInsertPoint(bb_ret);
      return codegen_ptr_get(pa);
    });

    member_descriptors.emplace_back(llvm::ConstantStruct::get(
        u.rt.smember_ty,
        {
            u.builder.CreateGlobalStringPtr(u.sf.Text(*f->name)),
            u.GetTypeInfo(fts),
            u.builder.getInt64(specifics_offset + specific_sty_layout->getElementOffset(kValueStorageSlotIdx)),
        }));
  }

  EmitStructuralTypeDescriptor(  //
      u, sym,
      IntermediateStructDescriptor{
          .kind = RtTypeKind::kUnion,
          .size = u.mod.getDataLayout().getTypeAllocSize(sty).getFixedValue(),
          .alignment = u.mod.getDataLayout().getABITypeAlign(sty).value(),
          .member_descriptors = member_descriptors,

          .ctor_fn = GenerateSingleArgumentFunction(  //
              u, names::Ctor(sym), u.rt.obj_ctor_fn_ty,
              [&](llvm::Argument* pa) {
                u.builder.CreateCall(u.rt.tpl.generic_opaque_struct_ctor_fn, {pa});
                u.builder.CreateRetVoid();
              }),

          .dtor_fn = [&] -> llvm::Function* {
            auto* dfn = u.getOrDeclareExternalFunc(names::Dtor(sym), u.rt.obj_dtor_fn_ty);
            //
            auto* pa = dfn->arg_begin();

            auto* bb_entry = llvm::BasicBlock::Create(u.ctx, "", dfn);
            auto* bb_specific_cleanup = llvm::BasicBlock::Create(u.ctx, "specific", dfn);
            auto* bb_generic_cleanup = llvm::BasicBlock::Create(u.ctx, "generic", dfn);

            //=== ENTRY ===//
            u.builder.SetInsertPoint(bb_entry);
            auto* p_tsel = u.builder.CreateStructGEP(sty, pa, kTplSelectionSlotIdx);
            auto* tsel = u.builder.CreateLoad(u.builder.getInt8Ty(), p_tsel);
            llvm::Value* cond =
                u.builder.CreateICmpEQ(tsel, u.builder.getInt8(std::to_underlying(RtTplSelection::kSpecificValue)));
            u.builder.CreateCondBr(cond, bb_specific_cleanup, bb_generic_cleanup);

            //=== SPECIFIC ===//
            u.builder.SetInsertPoint(bb_specific_cleanup);
            auto* p_specific = u.builder.CreateStructGEP(sty, pa, kTplValueStorageSlotIdx);
            u.builder.CreateCall(specific_dtor_fn, {p_specific});
            u.builder.CreateRetVoid();

            //=== GENERIC ===//
            u.builder.SetInsertPoint(bb_generic_cleanup);
            u.builder.CreateCall(u.rt.tpl.generic_opaque_struct_dtor_fn, {dfn, pa});
            u.builder.CreateRetVoid();
            return dfn;
          }(),

          .copy_fn = llvm::ConstantPointerNull::get(u.builder.getPtrTy()),

          .tplval_ctor_fn = GenerateSingleArgumentFunction(  //
              u, names::TplValCtor(sym), u.rt.obj_ctor_fn_ty,
              [&](llvm::Argument* pa) {
                u.builder.CreateStore(u.rt.uniont.unbound_am_idx,
                                      u.builder.CreateStructGEP(specific_sty, pa, kActiveMemberSlotIdx));
                u.builder.CreateRetVoid();
              }),
      });
}

//

void CodegenStructural(RtTypeKind kind, CodegenUnit& u, const core::semantic::Symbol* sym,
                       std::span<const ast::nodes::Field* const> fields) {
  std::vector<const core::semantic::Symbol*> members;
  members.reserve(fields.size());
  for (const auto& [idx, f] : fields | std::views::enumerate) {
    const auto* fsym = core::checker::ResolveTypeSpecSymbol(&u.sf, f->type, sym);
    assert(fsym);
    if (fsym->Flags() & core::semantic::SymbolFlags::kAnonymous) {
      CodegenStructural(u, fsym, fsym->Declaration()->As<ast::nodes::StructSpec>());
    }
    members.emplace_back(fsym);
  }

  if (kind != RtTypeKind::kUnion) {
    CodegenStructValueType(kind, u, sym, fields, members);
    CodegenStructTemplateType(kind, u, sym, fields, members);
  } else {
    CodegenUnionValueType(u, sym, fields, members);
    CodegenUnionTemplateType(u, sym, fields, members);
  }
}

RtTypeKind GetStructRtKind(const ast::Token& tok) {
  switch (tok.kind) {
    case ast::TokenKind::RECORD:
      return RtTypeKind::kRecord;
    case ast::TokenKind::SET:
      return RtTypeKind::kSet;
    case ast::TokenKind::UNION:
      return RtTypeKind::kUnion;
    default:
      assert(false);
      return RtTypeKind::kRecord;
  }
}

}  // namespace

void CodegenStructural(CodegenUnit& u, const core::semantic::Symbol* sym, const ast::nodes::StructTypeDecl* m) {
  CodegenStructural(GetStructRtKind(m->kind), u, sym, m->fields);
}
void CodegenStructural(CodegenUnit& u, const core::semantic::Symbol* sym, const ast::nodes::StructSpec* m) {
  CodegenStructural(GetStructRtKind(m->kind), u, sym, m->fields);
}

}  // namespace vanadium::compiler
