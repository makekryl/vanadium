#include "vanadium/compiler/Compiler.h"

#include <cassert>
#include <format>
#include <print>
#include <ranges>
#include <string_view>
#include <utility>

#include <magic_enum/magic_enum.hpp>

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Builtins.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>

#include "vanadium/compiler/ModuleRegistrar.h"
#include "vanadium/compiler/RuntimeBindings.h"
#include "vanadium/core/utils/SemanticUtils.h"

namespace vanadium::compiler {

namespace {

std::string SymName(const core::semantic::Symbol* sym) {
  if (sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    if (sym == &core::builtins::kInteger) {
      return "integer";
    }
    if (sym == &core::builtins::kCharstring) {
      return "charstring";
    }
    assert(false);
  }
  const auto* modnode = ast::utils::GetPredecessor<ast::nodes::Module>(sym->Declaration());
  const auto* sf = ast::utils::SourceFileOf(modnode);
  return std::format("{}_{}", sf->Text(*modnode->name), sym->GetName());
}

std::string VConstruct(const core::semantic::Symbol* sym) {
  return std::format("{}_ctor", SymName(sym));
}
std::string VDestruct(const core::semantic::Symbol* sym) {
  return std::format("{}_dtor", SymName(sym));
}
std::string VTypeinfo(const core::semantic::Symbol* sym) {
  return std::format("{}_typeinfo", SymName(sym));
}
std::string VGet(const core::semantic::Symbol* structsym, std::string_view member_name) {
  return std::format("{}_get_{}", SymName(structsym), member_name);
}

std::int64_t ParseInt(std::string_view s) {
  std::int64_t result;
  std::from_chars(s.data(), s.data() + s.size(), result);
  return result;
}
std::string_view ParseCharstring(std::string_view s) {
  s.remove_prefix(1);
  s.remove_suffix(1);
  return s;
}

struct RuntimeVarInfo {
  llvm::AllocaInst* alloca;
  const core::semantic::Symbol* sym;
};

class Codegen {
 public:
  Codegen(const core::SourceFile& sf) : sf_(sf), mod_(sf.module->name, ctx_), rt_(ctx_, mod_) {}

  void Gen() {
    CodegenTypes();
    CodegenFunctions();

    GenerateModuleRegistrationCode(sf_, ctx_, builder_, mod_);

    std::error_code ec;
    llvm::raw_fd_ostream dest(std::format("{}.ll", sf_.path), ec);
    mod_.print(dest, nullptr);
  }

 private:
  // TODO: move everything that uses this to a FunctionCodegen class isolated from the rest of Codegen
  std::vector<std::unordered_map<std::string_view, RuntimeVarInfo>> scope_stack_;
  void PushScope() {
    scope_stack_.emplace_back();
  }
  void PopScope() {
    // TODO: end all frame variables lifetimes
    scope_stack_.pop_back();
  }
  llvm::AllocaInst* AllocVar(std::string_view name, const core::semantic::Symbol* sym) {
    std::println("AllocVar({})", name);
    auto* alloca = createEntryBlockAlloca(LGetSymbolType(sym), name);
    builder_.CreateLifetimeStart(alloca);
    scope_stack_.back()[name] = {.alloca = alloca, .sym = sym};
    return alloca;
  }
  const RuntimeVarInfo* LookupVar(std::string_view name) {
    for (const auto& scope : scope_stack_ | std::views::reverse) {
      if (auto it = scope.find(name); it != scope.end()) {
        return &it->second;
      }
    }
    return nullptr;
  }

  //

  llvm::AllocaInst* createEntryBlockAlloca(llvm::Type* type, std::string_view name) {
    llvm::IRBuilder<> tmp(&curfn_->getEntryBlock(), curfn_->getEntryBlock().begin());
    return tmp.CreateAlloca(type, nullptr, name);
  }

  llvm::Function* declareExternalFunc(std::string_view name, llvm::FunctionType* ty) {
    return llvm::Function::Create(ty, llvm::Function::ExternalLinkage, name, mod_);
  }
  llvm::Function* getOrDeclareExternalFunc(std::string_view name, llvm::FunctionType* ty) {
    if (auto* fn = mod_.getFunction(name); fn) {
      return fn;
    }
    return declareExternalFunc(name, ty);
  }

  llvm::GlobalVariable* declareExternalConst(std::string_view name, llvm::Type* ty) {
    return new llvm::GlobalVariable(  //
        mod_, ty, true, llvm::GlobalValue::ExternalLinkage, nullptr, name);
  }
  llvm::GlobalVariable* getOrDeclareExternalConst(std::string_view name, llvm::Type* ty) {
    if (auto* gv = mod_.getNamedGlobal(name); gv) {
      return gv;
    }
    return declareExternalConst(name, ty);
  }

  //

  llvm::Type* LGetSymbolType(const core::semantic::Symbol*);
  llvm::Value* LGetSymbolUndef(const core::semantic::Symbol*);

  void CodegenTypes();
  //
  void CodegenStruct(const core::semantic::Symbol* sym);
  llvm::Function* CodegenStructGetter(llvm::StructType*, std::size_t idx, const core::semantic::Symbol* struct_sym,
                                      std::string_view member_name, const core::semantic::Symbol* member_sym);

  llvm::Value* CodegenExpr(const ast::nodes::Expr*);
  llvm::Value* CodegenSelectorExpr(const ast::nodes::SelectorExpr*);

  void EmitGenericVal(llvm::Value* slot, const core::semantic::Symbol*, llvm::Value* v);

  void CodegenFunctions();
  void CodegenBody(const ast::nodes::BlockStmt*);

  //

  const core::SourceFile& sf_;

  llvm::LLVMContext ctx_;
  llvm::IRBuilder<> builder_{ctx_};
  llvm::Module mod_;

  const RuntimeBindings rt_;

  llvm::Function* curfn_;
};

llvm::Type* Codegen::LGetSymbolType(const core::semantic::Symbol* sym) {
  assert(sym->Flags() & core::semantic::SymbolFlags::kType);
  if (sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    if (sym == &core::builtins::kInteger) {
      return rt_.int_ty;
    }
    if (sym == &core::builtins::kCharstring) {
      return rt_.charstring_ty;
    }
    if (sym == &core::checker::symbols::kVoidType) {
      return builder_.getVoidTy();
    }
    assert(false);
  }
  return builder_.getPtrTy();
}
llvm::Value* Codegen::LGetSymbolUndef(const core::semantic::Symbol* sym) {
  assert(sym->Flags() & core::semantic::SymbolFlags::kType);
  if (!(sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
    return llvm::ConstantPointerNull::get(builder_.getPtrTy());
  }

  if (sym == &core::builtins::kInteger) {
    return rt_.int_undef;
  }
  if (sym == &core::builtins::kCharstring) {
    return rt_.charstring_undef;
  }

  return nullptr;
}

void Codegen::CodegenTypes() {
  for (const auto& sym : sf_.module->scope->symbols.Enumerate() | std::views::values) {
    if (!(sym.Flags() & core::semantic::SymbolFlags::kType)) {
      continue;
    }

    const auto* n = sym.Declaration();
    switch (n->nkind) {
      case ast::NodeKind::StructTypeDecl: {
        CodegenStruct(&sym);
        break;
      }
      default: {
        break;
      }
    }
  }
}

void Codegen::CodegenStruct(const core::semantic::Symbol* sym) {
  const auto* m = sym->Declaration()->As<ast::nodes::StructTypeDecl>();

  auto* ty = llvm::StructType::create(ctx_, sf_.Text(*m->name));

  std::vector<llvm::Type*> body;
  body.reserve(m->fields.size());
  for (const auto& [idx, f] : m->fields | std::views::enumerate) {
    const auto& fsym = sf_.module->scope->Resolve(sf_.Text(f->type));
    body.push_back(LGetSymbolType(fsym));
  }
  ty->setBody(body);
  const auto& ty_bytes = mod_.getDataLayout().getTypeAllocSize(ty);

  llvm::IRBuilder<> fn_ctor_builder(ctx_);
  auto* fn_ctor = getOrDeclareExternalFunc(VConstruct(sym), rt_.type_ctor_fn_ty);
  fn_ctor_builder.SetInsertPoint(llvm::BasicBlock::Create(ctx_, "", fn_ctor));
  //
  llvm::IRBuilder<> fn_dtor_builder(ctx_);
  auto* fn_dtor = getOrDeclareExternalFunc(VDestruct(sym), rt_.type_dtor_fn_ty);
  fn_dtor_builder.SetInsertPoint(llvm::BasicBlock::Create(ctx_, "", fn_dtor));
  auto* fn_dtor_arg = fn_dtor->arg_begin();
  for (const auto& [idx, f] : m->fields | std::views::enumerate) {
    const auto& fsym = sf_.module->scope->Resolve(sf_.Text(f->type));

    CodegenStructGetter(ty, idx, sym, sf_.Text(*f->name), fsym);

    if (f->optional) {
      fn_dtor_builder.CreateCall(rt_.optional_dtor_f, {
                                                          fn_dtor_builder.CreateStructGEP(ty, fn_dtor_arg, idx),
                                                      });
    } else if (!(fsym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
      fn_dtor_builder.CreateCall(mod_.getOrInsertFunction(VDestruct(fsym), rt_.type_dtor_fn_ty),
                                 {
                                     fn_dtor_builder.CreateStructGEP(ty, fn_dtor_arg, idx),
                                 });
    }
  }
  fn_ctor_builder.CreateMemSet(fn_ctor->arg_begin(), builder_.getInt8(0), builder_.getInt64(ty_bytes.getFixedValue()),
                               llvm::MaybeAlign(mod_.getDataLayout().getPrefTypeAlign(ty)));
  fn_ctor_builder.CreateRetVoid();
  //
  fn_dtor_builder.CreateRetVoid();

  getOrDeclareExternalConst(VTypeinfo(sym), rt_.typeinfo_ty)
      ->setInitializer(
          llvm::ConstantStruct::get(rt_.typeinfo_ty, {
                                                         builder_.CreateGlobalStringPtr(sym->GetName(), "", 0, &mod_),
                                                         builder_.getInt8(0),
                                                         builder_.getInt64(ty_bytes.getFixedValue()),
                                                         llvm::ConstantPointerNull::get(builder_.getPtrTy()),
                                                         llvm::ConstantExpr::getBitCast(fn_ctor, builder_.getPtrTy()),
                                                         llvm::ConstantExpr::getBitCast(fn_dtor, builder_.getPtrTy()),
                                                     }));
}

llvm::Function* Codegen::CodegenStructGetter(llvm::StructType* sty, std::size_t idx,
                                             const core::semantic::Symbol* struct_sym, std::string_view member_name,
                                             const core::semantic::Symbol* member_sym) {
  auto* m_getter_fn = declareExternalFunc(VGet(struct_sym, member_name), rt_.generic_getter_fn_ty);
  auto* p_arg = m_getter_fn->arg_begin();

  auto* bb_entry = llvm::BasicBlock::Create(ctx_, "", m_getter_fn);
  builder_.SetInsertPoint(bb_entry);

  if (member_sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    builder_.CreateRet(builder_.CreateStructGEP(sty, p_arg, idx));
    return m_getter_fn;
  }

  llvm::Value* m_ptr = builder_.CreateStructGEP(sty, p_arg, 0, "m_ptr");
  llvm::Value* m_val = builder_.CreateLoad(builder_.getPtrTy(), m_ptr, "m_val");

  auto* bb_init = llvm::BasicBlock::Create(ctx_, "init", m_getter_fn);
  auto* bb_merge = llvm::BasicBlock::Create(ctx_, "merge", m_getter_fn);

  builder_.CreateCondBr(builder_.CreateIsNull(m_val), bb_init, bb_merge);

  //=== INIT ===//
  builder_.SetInsertPoint(bb_init);
  llvm::Value* new_m = builder_.CreateCall(
      rt_.type_alloc_f, {getOrDeclareExternalConst(VTypeinfo(member_sym), rt_.typeinfo_ty)}, "new_m");
  builder_.CreateStore(new_m, m_ptr);
  builder_.CreateBr(bb_merge);

  //=== MERGE ===//
  builder_.SetInsertPoint(bb_merge);
  //
  llvm::PHINode* ret = builder_.CreatePHI(builder_.getPtrTy(), 2, "ret");
  ret->addIncoming(m_val, bb_entry);
  ret->addIncoming(new_m, bb_init);
  //
  builder_.CreateRet(ret);

  return m_getter_fn;
}

void Codegen::CodegenFunctions() {
  for (const auto* def : sf_.ast.root->nodes.front()->As<ast::nodes::Module>()->defs) {
    const auto* n = def->def;

    switch (n->nkind) {
      case ast::NodeKind::FuncDecl: {
        const auto* m = n->As<ast::nodes::FuncDecl>();

        std::vector<llvm::Type*> params;
        std::vector<core::checker::InstantiatedType> params_syms;
        params.reserve(m->params->list.size());
        params_syms.reserve(m->params->list.size());
        for (const auto* param : m->params->list) {
          const auto& type =
              params_syms.emplace_back(core::checker::ResolveExprSymbol(&sf_, sf_.module->scope, param->type));
          params.emplace_back(LGetSymbolType(type.sym));
        }

        const auto& ret_sym = core::checker::ResolveCallableReturnType(&sf_, m);
        llvm::Type* ret_ty = LGetSymbolType(ret_sym.sym);

        auto* fntype = llvm::FunctionType::get(ret_ty, params, false);
        curfn_ = llvm::Function::Create(fntype, llvm::Function::ExternalLinkage, sf_.Text(*m->name), mod_);
        if (m->external) {
          break;
        }

        auto* entry = llvm::BasicBlock::Create(ctx_, "body", curfn_);
        builder_.SetInsertPoint(entry);

        PushScope();
        //
        for (auto [arg, param, type] : std::views::zip(curfn_->args(), m->params->list, params_syms)) {
          const auto& name = sf_.Text(*param->name);
          arg.setName(name);
          auto* alloca = AllocVar(name, type.sym);
          builder_.CreateStore(&arg, alloca);
        }
        CodegenBody(m->body);  // TODO: do not create a new scope there
        if (ret_sym.sym == &core::checker::symbols::kVoidType) {
          builder_.CreateRetVoid();
        }
        //
        PopScope();

        break;
      }
      case ast::NodeKind::ControlPart: {
        const auto* m = n->As<ast::nodes::ControlPart>();

        curfn_ = llvm::Function::Create(llvm::FunctionType::get(builder_.getVoidTy(), {}, false),
                                        llvm::Function::ExternalLinkage, "control", mod_);
        auto* entry = llvm::BasicBlock::Create(ctx_, "body", curfn_);
        builder_.SetInsertPoint(entry);
        CodegenBody(m->body);
        builder_.CreateRetVoid();

        break;
      }
    }
  }
}

void Codegen::CodegenBody(const ast::nodes::BlockStmt* b) {
  scope_stack_.emplace_back();

  b->Accept([&](const ast::Node* n) -> bool {
    switch (n->nkind) {
      case ast::NodeKind::DeclStmt:
        return true;

      case ast::NodeKind::ValueDecl: {
        const auto* m = n->As<ast::nodes::ValueDecl>();

        const auto itype = core::checker::ResolveExprType(&sf_, sf_.module->scope, m->type);
        for (auto* decl : m->decls) {
          auto* alloca = AllocVar(sf_.Text(*decl->name), itype.sym);

          llvm::Value* init_val;
          if (decl->value) {
            init_val = CodegenExpr(decl->value);
          } else {
            init_val = LGetSymbolUndef(itype.sym);
          }

          builder_.CreateStore(init_val, alloca);
        }

        break;
      }

      case ast::NodeKind::ExprStmt: {
        const auto* m = n->As<ast::nodes::ExprStmt>();
        return CodegenExpr(m->expr);
      }

      case ast::NodeKind::ReturnStmt: {
        const auto* m = n->As<ast::nodes::ReturnStmt>();
        if (!m->result) {
          builder_.CreateRetVoid();
          break;
        }
        builder_.CreateRet(CodegenExpr(m->result));
        break;
      }

      default: {
        std::println("Codegen(Body) fail at NodeKind::{}", magic_enum::enum_name(n->nkind));
      }
    }
    return false;
  });

  // TODO: preserve ctor-dtor order?
  for (const auto& var : scope_stack_.back() | std::views::values) {
    if (!var.sym || (var.sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) continue;
    builder_.CreateCall(rt_.type_free_f, {
                                             mod_.getGlobalVariable(VTypeinfo(var.sym)),
                                             builder_.CreateLoad(var.alloca->getAllocatedType(), var.alloca),
                                         });
  }

  scope_stack_.pop_back();
}

llvm::Value* Codegen::CodegenExpr(const ast::nodes::Expr* expr) {
  switch (expr->nkind) {
    case ast::NodeKind::Ident: {
      const auto* m = expr->As<ast::nodes::Ident>();

      const auto& name = sf_.Text(m);
      auto* alloca = LookupVar(name)->alloca;
      return builder_.CreateLoad(alloca->getAllocatedType(), alloca, name);
    }

    case ast::NodeKind::SelectorExpr: {
      const auto* m = expr->As<ast::nodes::SelectorExpr>();

      auto* vx = CodegenExpr(m->x);
      const auto* xsym =
          core::checker::ResolveExprType(&sf_, core::semantic::utils::FindScope(sf_.module->scope, m), m->x).sym;
      return builder_.CreateCall(getOrDeclareExternalFunc(VGet(xsym, sf_.Text(m->sel)), rt_.generic_getter_fn_ty),
                                 {vx});
    }

    case ast::NodeKind::ValueLiteral: {
      const auto* m = expr->As<ast::nodes::ValueLiteral>();
      switch (m->tok.kind) {
        case ast::TokenKind::INT:
          return rt_.GetInt(ParseInt(sf_.Text(m)));
        case ast::TokenKind::STRING:
          return rt_.GetCharstring(ParseCharstring(sf_.Text(m)));
      }
    }

    case ast::NodeKind::BinaryExpr: {
      const auto* m = expr->As<ast::nodes::BinaryExpr>();

      auto* vx = CodegenExpr(m->x);
      auto* vy = CodegenExpr(m->y);

      switch (m->op.kind) {
        case ast::TokenKind::EQ:
          return builder_.CreateCall(rt_.int_eq_f, {vx, vy});
        case ast::TokenKind::NE:
          return builder_.CreateCall(rt_.int_ne_f, {vx, vy});
        //
        case ast::TokenKind::LT:
          return builder_.CreateCall(rt_.int_lt_f, {vx, vy});
        case ast::TokenKind::LE:
          return builder_.CreateCall(rt_.int_le_f, {vx, vy});
        case ast::TokenKind::GT:
          return builder_.CreateCall(rt_.int_gt_f, {vx, vy});
        case ast::TokenKind::GE:
          return builder_.CreateCall(rt_.int_ge_f, {vx, vy});
        //
        case ast::TokenKind::ADD:
          return builder_.CreateCall(rt_.int_add_f, {vx, vy});
        case ast::TokenKind::SUB:
          return builder_.CreateCall(rt_.int_sub_f, {vx, vy});
        case ast::TokenKind::MUL:
          return builder_.CreateCall(rt_.int_mul_f, {vx, vy});
        case ast::TokenKind::DIV:
          return builder_.CreateCall(rt_.int_div_f, {vx, vy});
        default:
          break;
      }
      break;
    }

    case ast::NodeKind::AssignmentExpr: {
      const auto* m = expr->As<ast::nodes::AssignmentExpr>();

      auto* vv = CodegenExpr(m->value);

      switch (m->property->nkind) {
        case ast::NodeKind::Ident: {
          builder_.CreateStore(vv, LookupVar(sf_.Text(m->property))->alloca);
          break;
        }
        case ast::NodeKind::SelectorExpr: {
          const auto* propsym =
              core::checker::ResolveExprType(&sf_, core::semantic::utils::FindScope(sf_.module->scope, m), m->property)
                  .sym;
          auto* vtgt = CodegenExpr(m->property);
          builder_.CreateCall(
              getOrDeclareExternalFunc(
                  std::format("copy_{}", propsym->GetName()),
                  llvm::FunctionType::get(builder_.getVoidTy(), {builder_.getPtrTy(), vv->getType()}, false)),
              {vtgt, vv});
          break;
        }
        default: {
          std::unreachable();
          break;
        }
      }

      break;
    }

    case ast::NodeKind::CompositeLiteral: {
      const auto* m = expr->As<ast::nodes::CompositeLiteral>();
      const auto& itype = core::checker::ext::DeduceCompositeLiteralType(&sf_, sf_.module->scope, m);
      return builder_.CreateCall(rt_.type_alloc_f, {mod_.getOrInsertGlobal(VTypeinfo(itype.sym), rt_.typeinfo_ty)});
    }

    case ast::NodeKind::CallExpr: {
      const auto* m = expr->As<ast::nodes::CallExpr>();

      // poc, todo: resolve func type, check if arg == kVarargsType
      if ("log" == sf_.Text(m->fun)) {
        std::size_t n = m->args->list.size();

        auto* array_ty = llvm::ArrayType::get(rt_.generic_val_ty, n);

        auto* arr = createEntryBlockAlloca(array_ty, "generic_args_arr");
        builder_.CreateLifetimeStart(arr);

        for (const auto& [idx, argnode] : m->args->list | std::views::enumerate) {
          auto* slot = builder_.CreateGEP(array_ty, arr, {builder_.getInt32(0), builder_.getInt32(idx)});

          auto* val = CodegenExpr(argnode);

          llvm::Value* val_addr;
          if (val->getType()->isPointerTy()) {
            val_addr = val;
          } else {
            auto* tmp = createEntryBlockAlloca(val->getType(), "");
            builder_.CreateStore(val, tmp);
            val_addr = tmp;
          }

          const auto& isym = core::checker::ResolveExprType(
              &sf_, core::semantic::utils::FindScope(sf_.module->scope, argnode), argnode);
          std::println("isym->Name()={} / '{}'", isym->GetName(), sf_.Text(argnode));
          EmitGenericVal(slot, isym.sym, val_addr);
        }

        auto* res = builder_.CreateCall(rt_.log_f, {arr, builder_.getInt32(n)});
        builder_.CreateLifetimeEnd(arr);
        return res;
      }

      auto* callee = mod_.getFunction(sf_.Text(m->fun));
      std::vector<llvm::Value*> args;
      for (const auto* argnode : m->args->list) {
        args.push_back(CodegenExpr(argnode));
        if (!args.back()) {
          std::println("argnode unk '{}'", sf_.Text(argnode));
        }
      }

      return builder_.CreateCall(callee, args);
    }

    default: {
      std::println("Codegen(Expr) fail at NodeKind::{}", magic_enum::enum_name(expr->nkind));
      break;
    }
  }
  return nullptr;
}

llvm::Value* Codegen::CodegenSelectorExpr(const ast::nodes::SelectorExpr* se) {
  llvm::Value* xval;
  if (se->x->nkind == ast::NodeKind::SelectorExpr) {
    xval = CodegenSelectorExpr(se->x->As<ast::nodes::SelectorExpr>());
  } else {
    const auto& name = sf_.Text(se->x);
    auto* alloca = LookupVar(name)->alloca;
    xval = builder_.CreateLoad(alloca->getAllocatedType(), alloca, name);
  }

  // TODO: this is VERY inefficient TypeChecker usage
  const auto* xsym =
      core::checker::ResolveExprType(&sf_, core::semantic::utils::FindScope(sf_.module->scope, se), se->x).sym;
  return builder_.CreateCall(getOrDeclareExternalFunc(VGet(xsym, sf_.Text(se->sel)), rt_.generic_getter_fn_ty), {xval});
}

void Codegen::EmitGenericVal(llvm::Value* slot, const core::semantic::Symbol* sym, llvm::Value* v) {
  // p field
  auto* p_gep = builder_.CreateStructGEP(rt_.generic_val_ty, slot, 0);
  builder_.CreateStore(v, p_gep);

  // ty field
  auto* ty_gep = builder_.CreateStructGEP(rt_.generic_val_ty, slot, 1);
  builder_.CreateStore(getOrDeclareExternalConst(VTypeinfo(sym), rt_.typeinfo_ty), ty_gep);
}

}  // namespace

//

void CompileIR(const core::SourceFile& sf) {
  Codegen(sf).Gen();
}

}  // namespace vanadium::compiler
