#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/Argument.h>
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
  std::size_t size;
  const std::vector<llvm::Constant*>& member_descriptors;
  llvm::Constant* ctor_fn;
  llvm::Constant* dtor_fn;
  llvm::Constant* copy_fn;
  llvm::Constant* tplval_ctor_fn;
};

void EmitTypeDescriptor(CodegenUnit& u, TypeSymbol sym, const IntermediateStructDescriptor& d) {
  auto* member_descriptors_array = llvm::ConstantArray::get(
      llvm::ArrayType::get(u.rt.smember_ty, d.member_descriptors.size()), d.member_descriptors);
  auto* members_gv =
      new llvm::GlobalVariable(u.mod, member_descriptors_array->getType(), true, llvm::GlobalValue::PrivateLinkage,
                               member_descriptors_array, std::format("{}_members", names::Type(sym)));

  u.getOrDeclareExternalConst(names::TInfo(sym), u.rt.typeinfo_ty)
      ->setInitializer(llvm::ConstantStruct::get(  //
          u.rt.typeinfo_ty,
          {
              u.builder.CreateGlobalStringPtr(sym->GetName()),             // name
              u.builder.getInt8(std::to_underlying(RtTypeKind::kRecord)),  // kind
              u.rt.GetSizeI(d.size),                                       // size
              members_gv,                                                  // *members
              u.rt.GetSizeI(d.member_descriptors.size()),                  // members_count
              d.ctor_fn,                                                   // ctor
              d.dtor_fn,                                                   // dtor
              d.copy_fn,                                                   // copy
              u.GetTypeInfo({sym, !sym.is_template}),                      // counterpart
              d.tplval_ctor_fn,                                            // tpl_construct_value
          }));
}

llvm::Function* GenerateSingleArgumentFunction(CodegenUnit& u, std::string_view name, llvm::FunctionType* fty,
                                               mp::Consumer<llvm::IRBuilder<>&, llvm::Argument*> auto gen) {
  llvm::Function* cfn = u.getOrDeclareExternalFunc(name, fty);
  u.builder.SetInsertPoint(llvm::BasicBlock::Create(u.ctx, "", cfn));
  auto* p_arg = cfn->arg_begin();
  gen(u.builder, p_arg);
  return cfn;
}

llvm::Function* CodegenStructGetterFn(CodegenUnit& u, std::string_view name,
                                      mp::Invocable<llvm::Value*, llvm::IRBuilder<>&, llvm::Argument*> auto gen) {
  return GenerateSingleArgumentFunction(u, name, u.rt.generic_getter_fn_ty,
                                        [&](llvm::IRBuilder<>& builder, llvm::Argument* pa) {
                                          builder.CreateRet(gen(builder, pa));
                                        });
}

void CodegenStructValueType(CodegenUnit& u, const core::semantic::Symbol* sym,
                            std::span<const ast::nodes::Field* const> fields,
                            std::span<const core::semantic::Symbol* const> members) {
  auto* sty = llvm::StructType::create(u.ctx, names::Type(sym));
  // https://github.com/llvm/llvm-project/issues/101614
  const std::vector<llvm::Type*> body =
      std::ranges::to<std::vector>(members | std::views::transform([&](const auto* msym) -> llvm::Type* {
                                     return u.GetSymbolType(msym);
                                   }));
  sty->setBody(body);
  const auto* sty_layout = u.mod.getDataLayout().getStructLayout(sty);

  std::vector<llvm::Constant*> member_descriptors;
  member_descriptors.reserve(members.size());
  for (const auto& [idx, f] : fields | std::views::enumerate) {
    const auto& fts = members[idx];
    const auto codegen_ptr_get = [&](llvm::IRBuilder<>& builder, llvm::Argument* pa) {
      auto* v = builder.CreateStructGEP(sty, pa, idx);
      if (u.IsOpaque(fts)) {
        // V**
        v = builder.CreateLoad(builder.getPtrTy(), v);
      }  // else V*
      return v;
    };
    CodegenStructGetterFn(u, names::Getter(sym, u.sf.Text(*f->name)),
                          [&](llvm::IRBuilder<>& builder, llvm::Argument* pa) -> llvm::Value* {
                            return codegen_ptr_get(builder, pa);
                          });
    CodegenStructGetterFn(
        u, names::Muttor(sym, u.sf.Text(*f->name)),
        [&](llvm::IRBuilder<>& builder, llvm::Argument* pa) -> llvm::Value* {
          auto* v = codegen_ptr_get(builder, pa);
          if (!u.IsTrivial(fts)) {
            builder.CreateCall(u.getOrDeclareExternalFunc(names::Dtor(fts), u.rt.obj_dtor_fn_ty), {v});
            builder.CreateCall(u.getOrDeclareExternalFunc(names::Ctor(fts), u.rt.obj_ctor_fn_ty), {v});
          }
          return v;
        });

    member_descriptors.emplace_back(
        llvm::ConstantStruct::get(u.rt.smember_ty, {
                                                       u.builder.CreateGlobalStringPtr(u.sf.Text(*f->name)),
                                                       u.GetTypeInfo(fts),
                                                       u.builder.getInt64(sty_layout->getElementOffset(idx)),
                                                   }));
  }

  EmitTypeDescriptor(  //
      u, sym,
      IntermediateStructDescriptor{
          .size = u.mod.getDataLayout().getTypeAllocSize(sty).getFixedValue(),
          .member_descriptors = member_descriptors,

          .ctor_fn = GenerateSingleArgumentFunction(
              u, names::Ctor(sym), u.rt.obj_ctor_fn_ty,
              [&](llvm::IRBuilder<>& builder, llvm::Argument* pa) {
                for (std::size_t idx = 0; idx < members.size(); ++idx) {
                  const auto& fts = members[idx];
                  auto* fv = builder.CreateStructGEP(sty, pa, idx);
                  if (u.IsOpaque(fts)) {
                    auto* aptrv = builder.CreateCall(u.rt.type_new_f, {u.GetTypeInfo(fts), fv});
                    builder.CreateStore(aptrv, fv);
                  } else if (!u.IsTrivial(fts)) {
                    builder.CreateCall(u.getOrDeclareExternalFunc(names::Ctor(fts), u.rt.obj_ctor_fn_ty), {fv});
                  }
                }
                builder.CreateRetVoid();
              }),

          .dtor_fn = GenerateSingleArgumentFunction(  //
              u, names::Dtor(sym), u.rt.obj_ctor_fn_ty,
              [&](llvm::IRBuilder<>& builder, llvm::Argument* pa) {
                for (std::size_t idx = 0; idx < members.size(); ++idx) {
                  if (!u.IsTrivial(body[idx])) {
                    u.EmitDestructorInvocation(members[idx], builder.CreateStructGEP(sty, pa, idx));
                  }
                }
                u.builder.CreateRetVoid();
              }),

          .copy_fn = llvm::ConstantPointerNull::get(u.builder.getPtrTy()),

          .tplval_ctor_fn = llvm::ConstantPointerNull::get(u.builder.getPtrTy()),
      });
}

//

void CodegenStructTemplateType(CodegenUnit& u, const core::semantic::Symbol* semsym,
                               std::span<const ast::nodes::Field* const> fields,
                               std::span<const core::semantic::Symbol*> semmembers) {
  const TypeSymbol sym{semsym, true};
  const auto members = semmembers | std::views::transform([](const auto* msym) {
                         return TypeSymbol{msym, true};
                       });

  auto* specific_sty = llvm::StructType::create(u.ctx, std::format("{}_specific", names::Type(sym)));
  const auto specific_body =
      std::ranges::to<std::vector>(members | std::views::transform([&](const auto& msym) -> llvm::Type* {
                                     return u.GetSymbolType(msym);
                                   }));
  specific_sty->setBody(specific_body);
  auto* sty = llvm::StructType::create(u.rt.WrapTemplateStruct(specific_sty), names::Type(sym));

  const auto specifics_offset = u.mod.getDataLayout().getStructLayout(sty)->getElementOffset(1);
  const auto* specific_sty_layout = u.mod.getDataLayout().getStructLayout(specific_sty);

  std::vector<llvm::Constant*> member_descriptors;
  member_descriptors.reserve(members.size());
  for (const auto& [idx, f] : fields | std::views::enumerate) {
    const auto& fts = members[idx];
    const auto codegen_ptr_get = [&](llvm::IRBuilder<>& builder, llvm::Argument* pa) {
      auto* v = u.builder.CreateStructGEP(specific_sty, pa, idx);
      if (u.IsOpaque(fts)) {
        // V**
        v = builder.CreateLoad(builder.getPtrTy(), v);
      }  // else V*
      return v;
    };
    CodegenStructGetterFn(u, names::Getter(sym, u.sf.Text(*f->name)),
                          [&](llvm::IRBuilder<>& builder, llvm::Argument* pa) {
                            return codegen_ptr_get(builder, pa);
                          });
    CodegenStructGetterFn(u, names::Muttor(sym, u.sf.Text(*f->name)),
                          [&](llvm::IRBuilder<>& builder, llvm::Argument* pa) {
                            // TODO: reset data like in value types
                            return codegen_ptr_get(builder, pa);
                          });

    member_descriptors.emplace_back(llvm::ConstantStruct::get(
        u.rt.smember_ty, {
                             u.builder.CreateGlobalStringPtr(u.sf.Text(*f->name)),
                             u.GetTypeInfo(fts),
                             u.builder.getInt64(specifics_offset + specific_sty_layout->getElementOffset(idx)),
                         }));
  }

  EmitTypeDescriptor(
      u, sym,
      IntermediateStructDescriptor{
          .size = u.mod.getDataLayout().getTypeAllocSize(sty).getFixedValue(),
          .member_descriptors = member_descriptors,

          .ctor_fn = GenerateSingleArgumentFunction(  //
              u, names::Ctor(sym), u.rt.obj_ctor_fn_ty,
              [&](llvm::IRBuilder<>& builder, llvm::Argument* pa) {
                builder.CreateCall(u.rt.tpl.generic_opaque_struct_ctor_fn, {pa});
                builder.CreateRetVoid();
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
            auto* p_tsel = u.builder.CreateStructGEP(sty, pa, 0);
            auto* tsel = u.builder.CreateLoad(u.builder.getInt8Ty(), p_tsel);
            llvm::Value* cond =
                u.builder.CreateICmpEQ(tsel, u.builder.getInt8(std::to_underlying(RtTplSelection::kSpecificValue)));
            u.builder.CreateCondBr(cond, bb_specific_cleanup, bb_generic_cleanup);

            //=== SPECIFIC ===//
            u.builder.SetInsertPoint(bb_specific_cleanup);
            auto* p_specific = u.builder.CreateStructGEP(sty, pa, 1);
            for (std::size_t idx = 0; idx < members.size(); ++idx) {
              if (!u.IsTrivial(specific_body[idx])) {
                u.EmitDestructorInvocation(members[idx], u.builder.CreateStructGEP(specific_sty, p_specific, idx));
              }
            }
            u.builder.CreateRetVoid();

            //=== GENERIC ===//
            u.builder.SetInsertPoint(bb_generic_cleanup);
            u.builder.CreateCall(u.rt.tpl.generic_opaque_struct_dtor_fn, {dfn, pa});
            u.builder.CreateRetVoid();
            return dfn;
          }(),

          .copy_fn = llvm::ConstantPointerNull::get(u.builder.getPtrTy()),

          .tplval_ctor_fn = GenerateSingleArgumentFunction(
              u, names::TplValCtor(sym), u.rt.obj_ctor_fn_ty,
              [&](llvm::IRBuilder<>& builder, llvm::Argument* pa) {
                for (std::size_t idx = 0; idx < members.size(); ++idx) {
                  const auto& fts = members[idx];
                  auto* fv = builder.CreateStructGEP(specific_sty, pa, idx);
                  if (u.IsOpaque(fts)) {
                    auto* aptrv = builder.CreateCall(u.rt.type_new_f, {u.GetTypeInfo(fts), fv});
                    builder.CreateStore(aptrv, fv);
                  } else {
                    builder.CreateCall(u.getOrDeclareExternalFunc(names::Ctor(fts), u.rt.obj_ctor_fn_ty), {fv});
                  }
                }
                builder.CreateRetVoid();
              }),
      });
}

void CodegenStruct(CodegenUnit& u, const core::semantic::Symbol* sym,
                   std::span<const ast::nodes::Field* const> fields) {
  std::vector<const core::semantic::Symbol*> members;
  members.reserve(fields.size());
  for (const auto& [idx, f] : fields | std::views::enumerate) {
    // TODO: use checker::ResolveTypeSymbol, support for anonymous TypeSpec/ types
    const auto* fsym = u.sf.module->scope->Resolve(u.sf.Text(f->type));
    assert(fsym);
    members.emplace_back(fsym);
  }

  CodegenStructValueType(u, sym, fields, members);
  CodegenStructTemplateType(u, sym, fields, members);
}

}  // namespace

void CodegenStruct(CodegenUnit& u, const core::semantic::Symbol* sym, const ast::nodes::StructTypeDecl* m) {
  CodegenStruct(u, sym, m->fields);
}
void CodegenStruct(CodegenUnit& u, const core::semantic::Symbol* sym, const ast::nodes::StructSpec* m) {
  CodegenStruct(u, sym, m->fields);
}

}  // namespace vanadium::compiler
