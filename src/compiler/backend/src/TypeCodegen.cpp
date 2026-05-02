#include <algorithm>
#include <ranges>
#include <utility>
#include <vector>

#include <magic_enum/magic_enum.hpp>

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

// !!!!!!!!!!! SHOULD BE KEPT IN SYNC w/ rt_reflect.h !!!!!!!!!!!
// TODO: deduplicate
enum class RtTypeKind : std::uint8_t {
  kInteger,
  kFloat,
  kBoolean,

  kCharstring,
  kOctetstring,
  kBitstring,
  kHexstring,

  kRecord,
  kSet,

  kRecordOf,
  kSetOf,

  kOptionalMember,
};

llvm::Function* CodegenStructGetter(CodegenUnit& u, llvm::StructType* sty, std::size_t idx, TypeSymbol struct_sym,
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

llvm::Function* CodegenStructMuttor(CodegenUnit& u, llvm::StructType* sty, std::size_t idx, TypeSymbol struct_sym,
                                    const core::semantic::Symbol* member_sym, std::string_view member_name) {
  auto* fn = u.getOrDeclareExternalFunc(names::Muttor(struct_sym, member_name), u.rt.generic_getter_fn_ty);
  auto* bb = llvm::BasicBlock::Create(u.ctx, "", fn);
  u.builder.SetInsertPoint(bb);

  auto* p_arg = fn->arg_begin();

  auto* v = u.builder.CreateStructGEP(sty, p_arg, idx);
  if (u.IsOpaque(member_sym)) {
    // V**
    v = u.builder.CreateLoad(u.builder.getPtrTy(), v);
  }
  // TODO: recycle fn
  u.builder.CreateCall(u.getOrDeclareExternalFunc(names::Dtor(member_sym), u.rt.obj_dtor_fn_ty), {v});
  u.builder.CreateCall(u.getOrDeclareExternalFunc(names::Ctor(member_sym), u.rt.obj_ctor_fn_ty), {v});

  u.builder.CreateRet(v);

  return fn;
}

void CodegenStructType(CodegenUnit& u, TypeSymbol ts, const ast::nodes::StructTypeDecl* m,
                       std::span<TypeSymbol> members) {
  auto* sty = llvm::StructType::create(u.ctx, names::Type(ts));
  // https://github.com/llvm/llvm-project/issues/101614
  std::vector<llvm::Type*> body =
      std::ranges::to<std::vector>(members | std::views::transform([&](const auto& mts) -> llvm::Type* {
                                     return u.GetSymbolType(mts);
                                   }));
  sty->setBody(body);
  //
  const auto& sty_sz = u.mod.getDataLayout().getTypeAllocSize(sty);
  const auto* sty_layout = u.mod.getDataLayout().getStructLayout(sty);

  auto* fn_ctor = [&] -> llvm::Function* {
    llvm::Function* cfn = u.getOrDeclareExternalFunc(names::Ctor(ts), u.rt.obj_ctor_fn_ty);
    u.builder.SetInsertPoint(llvm::BasicBlock::Create(u.ctx, "", cfn));
    //
    auto* p_arg = cfn->arg_begin();
    for (const auto& [idx, f] : m->fields | std::views::enumerate) {
      const auto& fts = members[idx];
      auto* fv = u.builder.CreateStructGEP(sty, p_arg, idx);
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

  auto* fn_dtor = [&] -> llvm::Function* {
    auto* dfn = u.getOrDeclareExternalFunc(names::Dtor(ts), u.rt.obj_dtor_fn_ty);
    u.builder.SetInsertPoint(llvm::BasicBlock::Create(u.ctx, "", dfn));
    //
    auto* p_arg = dfn->arg_begin();
    for (const auto& [idx, f] : m->fields | std::views::enumerate) {
      if (u.IsTrivial(body[idx])) {
        continue;
      }
      auto* fv = u.builder.CreateStructGEP(sty, p_arg, idx);

      // TODO: below is copypaste from FunctionCodegen, should be unified + support for templates
      const auto& fts = members[idx];
      if (u.IsOpaque(fts)) {
        u.builder.CreateCall(u.rt.type_del_f, {
                                                  u.mod.getGlobalVariable(names::TInfo(fts)),
                                                  u.builder.CreateLoad(u.builder.getPtrTy(), fv),
                                              });
      } else {
        auto* fdtor_f = [&] -> llvm::Function* {
          if (const auto* strb = u.GetStringTypeBindings(fts)) {
            return strb->dtor_f;
          }
          return nullptr;
        }();
        if (fdtor_f) {
          u.builder.CreateCall(fdtor_f, {fv});
        }
      }
    }
    //
    u.builder.CreateRetVoid();
    return dfn;
  }();

  std::vector<llvm::Constant*> member_descriptors;
  member_descriptors.reserve(members.size());
  for (const auto& [idx, f] : m->fields | std::views::enumerate) {
    const auto& fts = members[idx];
    CodegenStructGetter(u, sty, idx, ts, fts, u.sf.Text(*f->name));
    CodegenStructMuttor(u, sty, idx, ts, fts, u.sf.Text(*f->name));

    member_descriptors.emplace_back(
        llvm::ConstantStruct::get(u.rt.smember_ty, {
                                                       u.builder.CreateGlobalStringPtr(u.sf.Text(*f->name)),
                                                       u.GetTypeInfo(fts),
                                                       u.builder.getInt64(sty_layout->getElementOffset(idx)),
                                                   }));
  }
  auto* member_descriptors_array =
      llvm::ConstantArray::get(llvm::ArrayType::get(u.rt.smember_ty, member_descriptors.size()), member_descriptors);

  u.getOrDeclareExternalConst(names::TInfo(ts), u.rt.typeinfo_ty)
      ->setInitializer(llvm::ConstantStruct::get(
          u.rt.typeinfo_ty,
          {
              u.builder.CreateGlobalStringPtr(ts->GetName()),               // name
              u.builder.getInt8(std::to_underlying(RtTypeKind::kRecord)),   // kind
              ts.is_template ? u.builder.getTrue() : u.builder.getFalse(),  // is_template
              u.rt.GetSizeI(sty_sz.getFixedValue()),                        // size
              new llvm::GlobalVariable(u.mod, member_descriptors_array->getType(), true,
                                       llvm::GlobalValue::PrivateLinkage, member_descriptors_array,
                                       std::format("{}_members", names::Type(ts))),  // *members
              u.rt.GetSizeI(member_descriptors.size()),                              // members_count
              llvm::ConstantExpr::getBitCast(fn_ctor, u.builder.getPtrTy()),         // ctor
              llvm::ConstantExpr::getBitCast(fn_dtor, u.builder.getPtrTy()),         // dtor
              llvm::ConstantPointerNull::get(u.builder.getPtrTy()),                  // copy
              llvm::ConstantPointerNull::get(u.builder.getPtrTy()),                  // counterpart
          }));
}

// TODO: THIS NEEDS CLEANUP BADLY
void CodegenStruct(CodegenUnit& u, const core::semantic::Symbol* sym, const ast::nodes::StructTypeDecl* m) {
  std::vector<TypeSymbol> members;
  members.reserve(m->fields.size());
  for (const auto& [idx, f] : m->fields | std::views::enumerate) {
    // TODO: use checker::ResolveTypeSymbol, support for anonymous TypeSpec/ types
    const auto* fsym = u.sf.module->scope->Resolve(u.sf.Text(f->type));
    assert(fsym);
    members.emplace_back(fsym, false);
  }

  TypeSymbol ts(sym, false);
  CodegenStructType(u, ts, m, members);
  //
  ts.is_template = true;
  std::ranges::for_each(members, [&](auto& mts) {
    mts.is_template = true;
  });
  CodegenStructType(u, ts, m, members);
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
