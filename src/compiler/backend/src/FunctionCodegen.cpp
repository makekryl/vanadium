#include <optional>

#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/core/utils/SemanticUtils.h>
#include <vanadium/lib/Assert.h>

#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler {

namespace {

struct AllocatedVar {
  llvm::AllocaInst* alloca;
  const core::semantic::Symbol* sym;
};

class ScopeManager {
 public:
  ScopeManager(llvm::Function* fn) : fn_(fn) {}

  void Push() {
    scope_stack_.emplace_back();
  }
  void Pop(CodegenContext& ctx) {
    // TODO: preserve ctor-dtor order?
    for (const auto& var : scope_stack_.back() | std::views::values) {
      if (!var.sym || (var.sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) continue;
      ctx.builder.CreateCall(ctx.rt.type_free_f, {
                                                     ctx.mod.getGlobalVariable(names::TInfo(var.sym)),
                                                     ctx.builder.CreateLoad(var.alloca->getAllocatedType(), var.alloca),
                                                 });
    }

    // TODO: end all frame variables lifetimes
    scope_stack_.pop_back();
  }
  llvm::AllocaInst* Alloc(CodegenContext& ctx, std::string_view name, const core::semantic::Symbol* sym) {
    auto* alloca = createEntryBlockAlloca(ctx.GetSymbolType(sym), name);
    ctx.builder.CreateLifetimeStart(alloca);
    scope_stack_.back()[name] = {.alloca = alloca, .sym = sym};
    return alloca;
  }
  const AllocatedVar* Lookup(std::string_view name) {
    for (const auto& scope : scope_stack_ | std::views::reverse) {
      if (auto it = scope.find(name); it != scope.end()) {
        return &it->second;
      }
    }
    return nullptr;
  }

  //

  // Ideally should not be public, but it is used to allocate service things
  llvm::AllocaInst* createEntryBlockAlloca(llvm::Type* type, std::string_view name) {
    llvm::IRBuilder<> tmp(&fn_->getEntryBlock(), fn_->getEntryBlock().begin());
    return tmp.CreateAlloca(type, nullptr, name);
  }

 private:
  std::vector<std::unordered_map<std::string_view, AllocatedVar>> scope_stack_;
  llvm::Function* fn_;
};
class FunctionCodegen {
 public:
  FunctionCodegen(CodegenContext& ctx) : ctx_(ctx) {}

  void Generate(const ast::nodes::FuncDecl* m) {
    std::vector<llvm::Type*> params;
    std::vector<core::checker::InstantiatedType> params_syms;
    params.reserve(m->params->list.size());
    params_syms.reserve(m->params->list.size());
    for (const auto* param : m->params->list) {
      const auto& type =
          params_syms.emplace_back(core::checker::ResolveExprSymbol(&ctx_.sf, ctx_.sf.module->scope, param->type));
      params.emplace_back(ctx_.GetSymbolType(type.sym));
    }

    const auto& ret_sym = core::checker::ResolveCallableReturnType(&ctx_.sf, m);
    llvm::Type* ret_ty = ctx_.GetSymbolType(ret_sym.sym);

    auto* fn = llvm::Function::Create(llvm::FunctionType::get(ret_ty, params, false), llvm::Function::ExternalLinkage,
                                      ctx_.sf.Text(*m->name), ctx_.mod);
    if (m->external) {
      return;
    }

    scope_.emplace(fn);
    auto* entry = llvm::BasicBlock::Create(ctx_.llvm_ctx, "body", fn);
    ctx_.builder.SetInsertPoint(entry);

    scope_->Push();
    //
    for (auto [arg, param, type] : std::views::zip(fn->args(), m->params->list, params_syms)) {
      const auto& name = ctx_.sf.Text(*param->name);
      arg.setName(name);
      auto* alloca = scope_->Alloc(ctx_, name, type.sym);
      ctx_.builder.CreateStore(&arg, alloca);
    }
    CodegenBody(m->body);  // TODO: do not create a new scope there
    if (ret_sym.sym == &core::checker::symbols::kVoidType) {
      ctx_.builder.CreateRetVoid();
    }
    //
    scope_->Pop(ctx_);
  }

  void Generate(const ast::nodes::ControlPart* m) {}

 private:
  void CodegenBody(const ast::nodes::BlockStmt* b) {
    scope_->Push();

    b->Accept([&](const ast::Node* n) -> bool {
      switch (n->nkind) {
        case ast::NodeKind::DeclStmt:
          return true;

        case ast::NodeKind::ValueDecl: {
          const auto* m = n->As<ast::nodes::ValueDecl>();

          const auto itype = core::checker::ResolveExprType(&ctx_.sf, ctx_.sf.module->scope, m->type);
          for (auto* decl : m->decls) {
            auto* alloca = scope_->Alloc(ctx_, ctx_.sf.Text(*decl->name), itype.sym);

            llvm::Value* init_val;
            if (decl->value) {
              init_val = CodegenExpr(decl->value);
            } else {
              init_val = ctx_.GetUndef(itype.sym);
            }

            ctx_.builder.CreateStore(init_val, alloca);
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
            ctx_.builder.CreateRetVoid();
            break;
          }
          ctx_.builder.CreateRet(CodegenExpr(m->result));
          break;
        }

        default: {
          std::println("Codegen(Body) fail at NodeKind::{}", magic_enum::enum_name(n->nkind));
        }
      }
      return false;
    });

    scope_->Pop(ctx_);
  }

  llvm::Value* CodegenExpr(const ast::nodes::Expr* expr) {
    switch (expr->nkind) {
      case ast::NodeKind::Ident: {
        const auto* m = expr->As<ast::nodes::Ident>();

        const auto& name = ctx_.sf.Text(m);
        auto* alloca = scope_->Lookup(name)->alloca;
        return ctx_.builder.CreateLoad(alloca->getAllocatedType(), alloca, name);
      }

      case ast::NodeKind::SelectorExpr: {
        const auto* m = expr->As<ast::nodes::SelectorExpr>();

        auto* vx = CodegenExpr(m->x);
        const auto* xsym =
            core::checker::ResolveExprType(&ctx_.sf, core::semantic::utils::FindScope(ctx_.sf.module->scope, m), m->x)
                .sym;
        return ctx_.builder.CreateCall(
            ctx_.getOrDeclareExternalFunc(names::Getter(xsym, ctx_.sf.Text(m->sel)), ctx_.rt.generic_getter_fn_ty),
            {vx});
      }

      case ast::NodeKind::ValueLiteral: {
        const auto* m = expr->As<ast::nodes::ValueLiteral>();
        switch (m->tok.kind) {
          case ast::TokenKind::INT:
            return ctx_.rt.GetInt(ctx_.ParseInt(m));
          case ast::TokenKind::STRING:
            return ctx_.rt.GetCharstring(ctx_.ParseCharstring(m));
          default:
            VANADIUM_DEBUG_ERROR("ValueLiteral unhandled token kind: {}", magic_enum::enum_name(m->tok.kind));
            break;
        }
      }

      case ast::NodeKind::BinaryExpr: {
        const auto* m = expr->As<ast::nodes::BinaryExpr>();

        auto* vx = CodegenExpr(m->x);
        auto* vy = CodegenExpr(m->y);

        switch (m->op.kind) {
          case ast::TokenKind::EQ:
            return ctx_.builder.CreateCall(ctx_.rt.int_eq_f, {vx, vy});
          case ast::TokenKind::NE:
            return ctx_.builder.CreateCall(ctx_.rt.int_ne_f, {vx, vy});
          //
          case ast::TokenKind::LT:
            return ctx_.builder.CreateCall(ctx_.rt.int_lt_f, {vx, vy});
          case ast::TokenKind::LE:
            return ctx_.builder.CreateCall(ctx_.rt.int_le_f, {vx, vy});
          case ast::TokenKind::GT:
            return ctx_.builder.CreateCall(ctx_.rt.int_gt_f, {vx, vy});
          case ast::TokenKind::GE:
            return ctx_.builder.CreateCall(ctx_.rt.int_ge_f, {vx, vy});
          //
          case ast::TokenKind::ADD:
            return ctx_.builder.CreateCall(ctx_.rt.int_add_f, {vx, vy});
          case ast::TokenKind::SUB:
            return ctx_.builder.CreateCall(ctx_.rt.int_sub_f, {vx, vy});
          case ast::TokenKind::MUL:
            return ctx_.builder.CreateCall(ctx_.rt.int_mul_f, {vx, vy});
          case ast::TokenKind::DIV:
            return ctx_.builder.CreateCall(ctx_.rt.int_div_f, {vx, vy});
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
            ctx_.builder.CreateStore(vv, scope_->Lookup(ctx_.sf.Text(m->property))->alloca);
            break;
          }
          case ast::NodeKind::SelectorExpr: {
            const auto* propsym = core::checker::ResolveExprType(
                                      &ctx_.sf, core::semantic::utils::FindScope(ctx_.sf.module->scope, m), m->property)
                                      .sym;
            auto* vtgt = CodegenExpr(m->property);
            ctx_.builder.CreateCall(
                ctx_.getOrDeclareExternalFunc(
                    std::format("copy_{}", propsym->GetName()),
                    llvm::FunctionType::get(ctx_.builder.getVoidTy(), {ctx_.builder.getPtrTy(), vv->getType()}, false)),
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
        const auto& itype = core::checker::ext::DeduceCompositeLiteralType(&ctx_.sf, ctx_.sf.module->scope, m);
        return ctx_.builder.CreateCall(ctx_.rt.type_alloc_f,
                                       {ctx_.mod.getOrInsertGlobal(names::TInfo(itype.sym), ctx_.rt.typeinfo_ty)});
      }

      case ast::NodeKind::CallExpr: {
        const auto* m = expr->As<ast::nodes::CallExpr>();

        // poc, todo: resolve func type, check if arg == kVarargsType
        if ("log" == ctx_.sf.Text(m->fun)) {
          std::size_t n = m->args->list.size();

          auto* array_ty = llvm::ArrayType::get(ctx_.rt.generic_val_ty, n);

          auto* arr = scope_->createEntryBlockAlloca(array_ty, "generic_args_arr");
          ctx_.builder.CreateLifetimeStart(arr);

          for (const auto& [idx, argnode] : m->args->list | std::views::enumerate) {
            auto* slot = ctx_.builder.CreateGEP(array_ty, arr, {ctx_.builder.getInt32(0), ctx_.builder.getInt32(idx)});

            auto* val = CodegenExpr(argnode);

            llvm::Value* val_addr;
            if (val->getType()->isPointerTy()) {
              val_addr = val;
            } else {
              auto* tmp = scope_->createEntryBlockAlloca(val->getType(), "");
              ctx_.builder.CreateStore(val, tmp);
              val_addr = tmp;
            }

            const auto& isym = core::checker::ResolveExprType(
                &ctx_.sf, core::semantic::utils::FindScope(ctx_.sf.module->scope, argnode), argnode);
            EmitGenericVal(slot, isym.sym, val_addr);
          }

          auto* res = ctx_.builder.CreateCall(ctx_.rt.log_f, {arr, ctx_.builder.getInt32(n)});
          ctx_.builder.CreateLifetimeEnd(arr);
          return res;
        }

        auto* callee = ctx_.mod.getFunction(ctx_.sf.Text(m->fun));
        std::vector<llvm::Value*> args;
        for (const auto* argnode : m->args->list) {
          args.push_back(CodegenExpr(argnode));
          if (!args.back()) {
            std::println("argnode unk '{}'", ctx_.sf.Text(argnode));
          }
        }

        return ctx_.builder.CreateCall(callee, args);
      }

      default: {
        VANADIUM_DEBUG_ERROR("Codegen(Expr) fail at NodeKind::{}", magic_enum::enum_name(expr->nkind));
        break;
      }
    }
    return nullptr;
  }

  llvm::Value* CodegenSelectorExpr(const ast::nodes::SelectorExpr* se) {
    llvm::Value* xval;
    if (se->x->nkind == ast::NodeKind::SelectorExpr) {
      xval = CodegenSelectorExpr(se->x->As<ast::nodes::SelectorExpr>());
    } else {
      const auto& name = ctx_.sf.Text(se->x);
      auto* alloca = scope_->Lookup(name)->alloca;
      xval = ctx_.builder.CreateLoad(alloca->getAllocatedType(), alloca, name);
    }

    // TODO: this is VERY inefficient TypeChecker usage
    const auto* xsym =
        core::checker::ResolveExprType(&ctx_.sf, core::semantic::utils::FindScope(ctx_.sf.module->scope, se), se->x)
            .sym;
    return ctx_.builder.CreateCall(
        ctx_.getOrDeclareExternalFunc(names::Getter(xsym, ctx_.sf.Text(se->sel)), ctx_.rt.generic_getter_fn_ty),
        {xval});
  }

  void EmitGenericVal(llvm::Value* slot, const core::semantic::Symbol* sym, llvm::Value* v) {
    // p field
    auto* p_gep = ctx_.builder.CreateStructGEP(ctx_.rt.generic_val_ty, slot, 0);
    ctx_.builder.CreateStore(v, p_gep);

    // ty field
    auto* ty_gep = ctx_.builder.CreateStructGEP(ctx_.rt.generic_val_ty, slot, 1);
    ctx_.builder.CreateStore(ctx_.getOrDeclareExternalConst(names::TInfo(sym), ctx_.rt.typeinfo_ty), ty_gep);
  }

  CodegenContext& ctx_;
  std::optional<ScopeManager> scope_;
};
}  // namespace

void CodegenFunction(CodegenContext& ctx, const ast::nodes::FuncDecl* m) {
  FunctionCodegen(ctx).Generate(m);
}
void CodegenFunction(CodegenContext& ctx, const ast::nodes::ControlPart* m) {
  FunctionCodegen(ctx).Generate(m);
}

}  // namespace vanadium::compiler
