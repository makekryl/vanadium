#include <optional>
#include <print>

#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
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
  void Pop(CodegenUnit& u) {
    // TODO: preserve ctor-dtor order?
    for (const auto& var : scope_stack_.back() | std::views::values) {
      if (!var.sym || (var.sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) continue;
      u.builder.CreateCall(u.rt.type_free_f, {
                                                 u.mod.getGlobalVariable(names::TInfo(var.sym)),
                                                 u.builder.CreateLoad(var.alloca->getAllocatedType(), var.alloca),
                                             });
    }

    // TODO: end all frame variables lifetimes
    scope_stack_.pop_back();
  }
  llvm::AllocaInst* Alloc(CodegenUnit& u, std::string_view name, const core::semantic::Symbol* sym) {
    auto* alloca = createEntryBlockAlloca(u.GetSymbolType(sym), name);
    u.builder.CreateLifetimeStart(alloca);
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
  FunctionCodegen(CodegenUnit& u) : u_(u) {}

  void Generate(const core::semantic::Symbol* sym, const ast::nodes::FuncDecl* m) {
    auto* fn = u_.GetFunction(sym);
    if (m->external) {
      return;
    }

    scope_.emplace(fn);
    auto* entry = llvm::BasicBlock::Create(u_.ctx, "body", fn);
    u_.builder.SetInsertPoint(entry);

    scope_->Push();
    //
    for (auto [arg, param] : std::views::zip(fn->args(), m->params->list)) {
      const auto& isym = core::checker::ResolveExprSymbol(&u_.sf, u_.sf.module->scope, param->type);

      const auto& name = Lit(param->name);
      arg.setName(name);

      auto* alloca = scope_->Alloc(u_, name, isym.sym);
      u_.builder.CreateStore(&arg, alloca);
    }
    CodegenBody(m->body);  // TODO: do not create a new scope there
    if (fn->getReturnType() == u_.builder.getVoidTy()) {
      u_.builder.CreateRetVoid();
    }
    //
    scope_->Pop(u_);
  }

  void Generate(const core::semantic::Symbol* sym, const ast::nodes::ControlPart* m) {}

 private:
  std::string_view Lit(const std::optional<ast::nodes::Ident>& t) {
    return u_.sf.Text(*t);
  }
  std::string_view Lit(const ast::Node* n) {
    return u_.sf.Text(n);
  }

  void CodegenBody(const ast::nodes::BlockStmt* b) {
    scope_->Push();

    b->Accept([&](const ast::Node* n) -> bool {
      switch (n->nkind) {
        case ast::NodeKind::DeclStmt:
          return true;

        case ast::NodeKind::ValueDecl: {
          const auto* m = n->As<ast::nodes::ValueDecl>();

          const auto itype = core::checker::ResolveExprType(&u_.sf, u_.sf.module->scope, m->type);
          for (auto* decl : m->decls) {
            auto* alloca = scope_->Alloc(u_, Lit(decl->name), itype.sym);

            llvm::Value* init_val;
            if (decl->value) {
              init_val = CodegenExpr(decl->value);
            } else {
              init_val = u_.GetUndef(itype.sym);
            }

            u_.builder.CreateStore(init_val, alloca);
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
            u_.builder.CreateRetVoid();
            break;
          }
          u_.builder.CreateRet(CodegenExpr(m->result));
          break;
        }

        default: {
          std::println("Codegen(Body) fail at NodeKind::{}", magic_enum::enum_name(n->nkind));
        }
      }
      return false;
    });

    scope_->Pop(u_);
  }

  llvm::Value* CodegenExpr(const ast::nodes::Expr* expr) {
    switch (expr->nkind) {
      case ast::NodeKind::Ident: {
        const auto* m = expr->As<ast::nodes::Ident>();

        const auto& name = Lit(m);
        auto* alloca = scope_->Lookup(name)->alloca;
        return u_.builder.CreateLoad(alloca->getAllocatedType(), alloca, name);
      }

      case ast::NodeKind::SelectorExpr: {
        const auto* m = expr->As<ast::nodes::SelectorExpr>();

        auto* vx = CodegenExpr(m->x);
        const auto* xsym =
            core::checker::ResolveExprType(&u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, m), m->x).sym;
        return u_.builder.CreateCall(
            u_.getOrDeclareExternalFunc(names::Getter(xsym, Lit(m->sel)), u_.rt.generic_getter_fn_ty), {vx});
      }

      case ast::NodeKind::ValueLiteral: {
        const auto* m = expr->As<ast::nodes::ValueLiteral>();
        switch (m->tok.kind) {
          case ast::TokenKind::INT:
            return u_.rt.GetInt(u_.ParseInt(m));
          case ast::TokenKind::STRING:
            return u_.rt.GetCharstring(u_.ParseCharstring(m));
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
            return u_.builder.CreateCall(u_.rt.int_eq_f, {vx, vy});
          case ast::TokenKind::NE:
            return u_.builder.CreateCall(u_.rt.int_ne_f, {vx, vy});
          //
          case ast::TokenKind::LT:
            return u_.builder.CreateCall(u_.rt.int_lt_f, {vx, vy});
          case ast::TokenKind::LE:
            return u_.builder.CreateCall(u_.rt.int_le_f, {vx, vy});
          case ast::TokenKind::GT:
            return u_.builder.CreateCall(u_.rt.int_gt_f, {vx, vy});
          case ast::TokenKind::GE:
            return u_.builder.CreateCall(u_.rt.int_ge_f, {vx, vy});
          //
          case ast::TokenKind::ADD:
            return u_.builder.CreateCall(u_.rt.int_add_f, {vx, vy});
          case ast::TokenKind::SUB:
            return u_.builder.CreateCall(u_.rt.int_sub_f, {vx, vy});
          case ast::TokenKind::MUL:
            return u_.builder.CreateCall(u_.rt.int_mul_f, {vx, vy});
          case ast::TokenKind::DIV:
            return u_.builder.CreateCall(u_.rt.int_div_f, {vx, vy});
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
            u_.builder.CreateStore(vv, scope_->Lookup(Lit(m->property))->alloca);
            break;
          }
          case ast::NodeKind::SelectorExpr: {
            const auto* propsym = core::checker::ResolveExprType(
                                      &u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, m), m->property)
                                      .sym;
            auto* vtgt = CodegenExpr(m->property);
            u_.builder.CreateCall(
                u_.getOrDeclareExternalFunc(
                    std::format("copy_{}", propsym->GetName()),
                    llvm::FunctionType::get(u_.builder.getVoidTy(), {u_.builder.getPtrTy(), vv->getType()}, false)),
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
        const auto& itype = core::checker::ext::DeduceCompositeLiteralType(&u_.sf, u_.sf.module->scope, m);
        return u_.builder.CreateCall(u_.rt.type_alloc_f,
                                     {u_.mod.getOrInsertGlobal(names::TInfo(itype.sym), u_.rt.typeinfo_ty)});
      }

      case ast::NodeKind::CallExpr: {
        const auto* m = expr->As<ast::nodes::CallExpr>();

        const auto* func_sym = core::checker::ResolveExprSymbol(&u_.sf, u_.sf.module->scope, m->fun).sym;
        assert(func_sym->Flags() & core::semantic::SymbolFlags::kFunction);

        auto* callee = u_.GetFunction(func_sym);

        if (callee->hasFnAttribute(kVarargsAttr)) {
          const std::size_t n = m->args->list.size();

          auto* array_ty = llvm::ArrayType::get(u_.rt.generic_val_ty, n);

          auto* arr = scope_->createEntryBlockAlloca(array_ty, "generic_args_arr");
          u_.builder.CreateLifetimeStart(arr);

          for (const auto& [idx, argnode] : m->args->list | std::views::enumerate) {
            auto* slot = u_.builder.CreateGEP(array_ty, arr, {u_.builder.getInt32(0), u_.builder.getInt32(idx)});

            auto* val = CodegenExpr(argnode);

            llvm::Value* val_addr;
            if (val->getType()->isPointerTy()) {
              val_addr = val;
            } else {
              auto* tmp = scope_->createEntryBlockAlloca(val->getType(), "");
              u_.builder.CreateStore(val, tmp);
              val_addr = tmp;
            }

            const auto& isym = core::checker::ResolveExprType(
                &u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, argnode), argnode);
            EmitGenericVal(slot, isym.sym, val_addr);
          }

          auto* res = u_.builder.CreateCall(callee, {arr, u_.builder.getInt32(n)});
          u_.builder.CreateLifetimeEnd(arr);
          return res;
        }

        std::vector<llvm::Value*> args;
        args.reserve(m->args->list.size());
        for (const auto* argnode : m->args->list) {
          auto* av = CodegenExpr(argnode);
          args.push_back(av);
          VANADIUM_DEBUG_ASSERT(av != nullptr, "Unknown argument expr type: {}", magic_enum::enum_name(argnode->nkind))
        }

        return u_.builder.CreateCall(callee, args);
      }

      default: {
        VANADIUM_DEBUG_ERROR("Codegen(Expr) fail at NodeKind::{}", magic_enum::enum_name(expr->nkind));
        break;
      }
    }
    return nullptr;
  }

  void EmitGenericVal(llvm::Value* slot, const core::semantic::Symbol* sym, llvm::Value* v) {
    // p field
    auto* p_gep = u_.builder.CreateStructGEP(u_.rt.generic_val_ty, slot, 0);
    u_.builder.CreateStore(v, p_gep);

    // ty field
    auto* ty_gep = u_.builder.CreateStructGEP(u_.rt.generic_val_ty, slot, 1);
    u_.builder.CreateStore(u_.getOrDeclareExternalConst(names::TInfo(sym), u_.rt.typeinfo_ty), ty_gep);
  }

  CodegenUnit& u_;
  std::optional<ScopeManager> scope_;
};
}  // namespace

void CodegenFunction(CodegenUnit& u, const core::semantic::Symbol* sym) {
  assert(sym->Flags() & core::semantic::SymbolFlags::kFunction);
  const auto* n = sym->Declaration();
  switch (n->nkind) {
    case ast::NodeKind::FuncDecl:
      FunctionCodegen(u).Generate(sym, n->As<ast::nodes::FuncDecl>());
      break;
    case ast::NodeKind::ControlPart:
      FunctionCodegen(u).Generate(sym, n->As<ast::nodes::ControlPart>());
      break;
    default:
      VANADIUM_DEBUG_ERROR("Unhandled node: {}", magic_enum::enum_name(n->nkind));
      break;
  }
}

}  // namespace vanadium::compiler
