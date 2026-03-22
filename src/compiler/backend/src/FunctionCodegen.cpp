#include <cassert>
#include <forward_list>
#include <limits>
#include <optional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/core/utils/SemanticUtils.h>
#include <vanadium/lib/Assert.h>
#include <vanadium/lib/ScopedValue.h>

#include "vanadium/compiler/Codegen.h"
#include "vanadium/core/Builtins.h"

namespace vanadium::compiler {

namespace {

struct AllocatedVar {
  llvm::AllocaInst* alloca;
  const core::semantic::Symbol* sym;
};

class ScopeManager {
 public:
  ScopeManager(CodegenUnit& u, llvm::Function* fn) : u_(u), fn_(fn) {}

  struct Frame {
    enum class Kind : std::uint8_t {
      kRegular,
      kTemporaries,
      kLoop,
    };

    std::unordered_map<std::string_view, AllocatedVar> vars;
    std::forward_list<AllocatedVar> temporaries;
    std::vector<AllocatedVar*> ordered_vars;

    llvm::BasicBlock::iterator begin;
    Kind kind;
  };

  void Enter(Frame::Kind kind) {
    frame_stack_.emplace_back(Frame{
        .begin = u_.builder.GetInsertPoint(),
        .kind = kind,
    });
  }
  void Exit() {
    assert(!frame_stack_.empty());

    auto frame = std::move(frame_stack_.back());
    frame_stack_.pop_back();

    if (!u_.builder.GetInsertBlock()->getTerminator()) {
      EmitDestructors(frame);
    }
  }

  void EmitDestructors() {
    for (const auto& frame : frame_stack_ | std::views::reverse) {
      EmitDestructors(frame);
    }
  }
  void EmitDestructorsInterruptingLoop() {
    for (const auto& frame : frame_stack_ | std::views::reverse) {
      EmitDestructors(frame);
      if (frame.kind == Frame::Kind::kLoop) {
        break;
      }
    }
  }

  llvm::AllocaInst* Alloc(std::string_view name, const core::semantic::Symbol* sym) {
    auto* alloca = createEntryBlockAlloca(u_.GetSymbolType(sym), name);
    u_.builder.CreateLifetimeStart(alloca);

    auto& frame = frame_stack_.back();
    const auto& [it, _] = frame.vars.emplace(name, AllocatedVar{.alloca = alloca, .sym = sym});
    frame.ordered_vars.emplace_back(&it->second);

    return alloca;
  }
  llvm::AllocaInst* AllocTemp(const core::semantic::Symbol* sym) {
    auto* alloca = createEntryBlockAlloca(u_.GetSymbolType(sym), "");
    u_.builder.CreateLifetimeStart(alloca);

    auto& frame = frame_stack_.back();
    auto& var = frame.temporaries.emplace_front(AllocatedVar{.alloca = alloca, .sym = sym});
    frame.ordered_vars.emplace_back(&var);

    return alloca;
  }
  llvm::AllocaInst* AllocTrivialTemp(llvm::Type* ty) {
    auto* alloca = createEntryBlockAlloca(ty, "");
    u_.builder.CreateLifetimeStart(alloca);

    auto& frame = frame_stack_.back();
    auto& var = frame.temporaries.emplace_front(AllocatedVar{.alloca = alloca, .sym = nullptr});
    frame.ordered_vars.emplace_back(&var);

    return alloca;
  }
  llvm::AllocaInst* AllocArg(std::string_view name, const core::semantic::Symbol* sym) {
    auto* alloca = createEntryBlockAlloca(u_.GetSymbolType(sym), name);
    args_[name] = {.alloca = alloca, .sym = sym};
    return alloca;
  }
  const AllocatedVar* Lookup(std::string_view name) {
    for (const auto& frame : frame_stack_ | std::views::reverse) {
      if (auto it = frame.vars.find(name); it != frame.vars.end()) {
        return &it->second;
      }
    }
    if (auto it = args_.find(name); it != args_.end()) {
      return &it->second;
    }
    return nullptr;
  }

  //

  // Ideally should not be public, but it is used to allocate service things
  llvm::AllocaInst* createEntryBlockAlloca(llvm::Type* type, std::string_view name) {
    llvm::IRBuilder<> tmp(&fn_->getEntryBlock(), fn_->getEntryBlock().begin());
    auto* alloca = tmp.CreateAlloca(type, nullptr, name);

    if (!frame_stack_.empty()) {
      auto& frame = frame_stack_.back();
      if (frame.begin->getNextNode() && llvm::dyn_cast<llvm::AllocaInst>(&(*frame.begin->getNextNode()))) {
        frame.begin++;
      }
    }

    return alloca;
  }

 private:
  void EmitDestructors(const Frame& frame) {
    if (frame.ordered_vars.empty()) {
      return;
    }

    // todo: check if stackalloc was actually used by the frame
    {
      llvm::IRBuilder<> tmp_builder(frame.begin->getNextNode());
      tmp_builder.CreateCall(u_.rt.stackalloc_mark_f);
    }

    for (const auto& var : frame.ordered_vars | std::views::reverse) {
      if (var->sym) {
        if (var->sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
          if (var->sym == &core::builtins::kCharstring) {
            // TODO: partial inline (is_bound && is_ext) in RuntimeBindings
            u_.builder.CreateCall(u_.rt.charstring_dtor_f, {var->alloca});
          }
        } else {
          u_.builder.CreateCall(u_.rt.type_del_f,
                                {
                                    u_.mod.getGlobalVariable(names::TInfo(var->sym)),
                                    u_.builder.CreateLoad(var->alloca->getAllocatedType(), var->alloca),
                                });
        }
      }
      u_.builder.CreateLifetimeEnd(var->alloca);
    }

    u_.builder.CreateCall(u_.rt.stackalloc_sweep_f);
  }

  std::vector<Frame> frame_stack_;
  std::unordered_map<std::string_view, AllocatedVar> args_;

  CodegenUnit& u_;
  llvm::Function* fn_;
};

class ScopeGuard {
 public:
  ScopeGuard(ScopeManager& m) : m_(m) {}
  ~ScopeGuard() {
    m_.Exit();
  }

 private:
  ScopeManager& m_;
};

class FunctionCodegen {
 public:
  FunctionCodegen(CodegenUnit& u) : u_(u) {}

  void Generate(const core::semantic::Symbol* sym, const ast::nodes::FuncDecl* m) {
    fn_ = u_.GetFunction(sym);
    if (m->external) {
      return;
    }

    scope_.emplace(u_, fn_);

    retbb_ = llvm::BasicBlock::Create(u_.ctx, "exit");

    auto* entry = llvm::BasicBlock::Create(u_.ctx, "entry", fn_);
    u_.builder.SetInsertPoint(entry);

    if (fn_->getReturnType() != u_.builder.getVoidTy()) {
      retval_ = u_.builder.CreateAlloca(fn_->getReturnType());
    } else {
      retval_ = nullptr;
    }

    for (auto [arg, param] : std::views::zip(fn_->args(), m->params->list)) {
      const auto& isym = core::checker::ResolveExprSymbol(&u_.sf, u_.sf.module->scope, param->type);

      const auto& name = Lit(param->name);
      arg.setName(name);

      auto* alloca = scope_->AllocArg(name, isym.sym);
      u_.builder.CreateStore(&arg, alloca);
    }

    CodegenStmt(m->body);

    if (!u_.builder.GetInsertBlock()->getTerminator()) {
      scope_->EmitDestructors();
      u_.builder.CreateBr(retbb_);
    }

    fn_->insert(fn_->end(), retbb_);
    u_.builder.SetInsertPoint(retbb_);
    if (retval_) {
      u_.builder.CreateRet(u_.builder.CreateLoad(fn_->getReturnType(), retval_, "retval"));
    } else {
      u_.builder.CreateRetVoid();
    }
  }

  void Generate(const core::semantic::Symbol* sym, const ast::nodes::ControlPart* m) {}

 private:
  std::string_view Lit(const std::optional<ast::nodes::Ident>& t) {
    return u_.sf.Text(*t);
  }
  std::string_view Lit(const ast::Node* n) {
    return u_.sf.Text(n);
  }

  [[nodiscard]] ScopeGuard EnterStackFrame(ScopeManager::Frame::Kind kind = ScopeManager::Frame::Kind::kRegular) {
    scope_->Enter(kind);
    return *scope_;
  }

  void CodegenStmt(const ast::nodes::Stmt*);
  void CodegenDecl(const ast::nodes::Decl*);
  llvm::Value* CodegenExpr(const ast::nodes::Expr*, llvm::Value* dest = nullptr);

  llvm::Value* CodegenExprAsBool(const ast::nodes::Expr* expr) {
    auto* v = CodegenExpr(expr);
    if (v->getType() == u_.builder.getInt1Ty()) {
      return v;
    }
    assert(v->getType() == u_.rt.bool_ty);
    return u_.builder.CreateCall(u_.rt.bool_get_f, {v});
  }

  bool IsTrivial(llvm::Type* ty) const {
    // int won't be trivial after bignum support
    return ty == u_.rt.bool_ty || ty == u_.rt.int_ty;
  }

  void EmitGenericVal(llvm::Value* slot, const core::semantic::Symbol* sym, llvm::Value* v) {
    // p field
    auto* p_gep = u_.builder.CreateStructGEP(u_.rt.generic_val_ty, slot, 0, "gvt_p");
    u_.builder.CreateStore(v, p_gep);

    // ty field
    auto* ty_gep = u_.builder.CreateStructGEP(u_.rt.generic_val_ty, slot, 1, "gvt_ty");
    u_.builder.CreateStore(u_.getOrDeclareExternalConst(names::TInfo(sym), u_.rt.typeinfo_ty), ty_gep);
  }

  llvm::AllocaInst* retval_;
  llvm::BasicBlock* retbb_;
  struct {
    llvm::BasicBlock* post;
    llvm::BasicBlock* end;
  } loop_ctx_;
  llvm::Function* fn_;

  std::optional<ScopeManager> scope_;
  CodegenUnit& u_;
};

void FunctionCodegen::CodegenStmt(const ast::nodes::Stmt* n) {
  switch (n->nkind) {
    case ast::NodeKind::BlockStmt: {
      const auto* m = n->As<ast::nodes::BlockStmt>();
      auto stack_frame{EnterStackFrame()};
      for (const auto* stmt : m->stmts) {
        CodegenStmt(stmt);
      }
      break;
    }

    case ast::NodeKind::IfStmt: {
      const auto* m = n->As<ast::nodes::IfStmt>();

      auto* then_bb = llvm::BasicBlock::Create(u_.ctx, "if.then");
      auto* end_bb = llvm::BasicBlock::Create(u_.ctx, "if.end");
      auto* else_bb = m->alternate ? llvm::BasicBlock::Create(u_.ctx, "if.else") : end_bb;

      u_.builder.CreateCondBr(CodegenExprAsBool(m->cond), then_bb, else_bb);

      fn_->insert(fn_->end(), then_bb);
      u_.builder.SetInsertPoint(then_bb);
      CodegenStmt(m->consequent);
      if (!u_.builder.GetInsertBlock()->getTerminator()) {
        u_.builder.CreateBr(end_bb);
      }

      if (m->alternate) {
        fn_->insert(fn_->end(), else_bb);
        u_.builder.SetInsertPoint(else_bb);
        CodegenStmt(m->alternate);
        if (!u_.builder.GetInsertBlock()->getTerminator()) {
          u_.builder.CreateBr(end_bb);
        }
      }

      fn_->insert(fn_->end(), end_bb);
      u_.builder.SetInsertPoint(end_bb);

      break;
    }

    case ast::NodeKind::ForStmt: {
      const auto* m = n->As<ast::nodes::ForStmt>();

      auto for_frame{EnterStackFrame()};  // fake scope to drop loop variable after ForStmt end
      //
      CodegenStmt(m->init);

      auto* cond_bb = llvm::BasicBlock::Create(u_.ctx, "for.cond", fn_);
      auto* body_bb = llvm::BasicBlock::Create(u_.ctx, "for.body", fn_);
      auto* post_bb = llvm::BasicBlock::Create(u_.ctx, "for.post", fn_);
      auto* end_bb = llvm::BasicBlock::Create(u_.ctx, "for.end", fn_);

      u_.builder.CreateBr(cond_bb);
      u_.builder.SetInsertPoint(cond_bb);
      u_.builder.CreateCondBr(CodegenExprAsBool(m->cond), body_bb, end_bb);

      u_.builder.SetInsertPoint(post_bb);
      CodegenStmt(m->post);
      u_.builder.CreateBr(cond_bb);

      u_.builder.SetInsertPoint(body_bb);
      {
        auto loop_frame{EnterStackFrame(ScopeManager::Frame::Kind::kLoop)};
        {
          lib::ScopedValue loop_ctx_guard(loop_ctx_, {.post = post_bb, .end = end_bb});
          CodegenStmt(m->body);
        }
        if (!u_.builder.GetInsertBlock()->getTerminator()) {
          u_.builder.CreateBr(post_bb);
        }
      }

      u_.builder.SetInsertPoint(end_bb);

      break;
    }

    case ast::NodeKind::WhileStmt: {
      const auto* m = n->As<ast::nodes::WhileStmt>();

      auto* cond_bb = llvm::BasicBlock::Create(u_.ctx, "while.cond", fn_);
      auto* body_bb = llvm::BasicBlock::Create(u_.ctx, "while.body", fn_);
      auto* end_bb = llvm::BasicBlock::Create(u_.ctx, "while.end", fn_);

      u_.builder.CreateBr(cond_bb);
      u_.builder.SetInsertPoint(cond_bb);
      u_.builder.CreateCondBr(CodegenExprAsBool(m->cond), body_bb, end_bb);

      u_.builder.SetInsertPoint(body_bb);
      {
        auto loop_frame{EnterStackFrame(ScopeManager::Frame::Kind::kLoop)};
        {
          lib::ScopedValue loop_ctx_guard(loop_ctx_, {.post = body_bb, .end = end_bb});
          CodegenStmt(m->body);
        }
        if (!u_.builder.GetInsertBlock()->getTerminator()) {
          u_.builder.CreateBr(cond_bb);
        }
      }

      u_.builder.SetInsertPoint(end_bb);

      break;
    }

    case ast::NodeKind::DoWhileStmt: {
      const auto* m = n->As<ast::nodes::DoWhileStmt>();

      auto* cond_bb = llvm::BasicBlock::Create(u_.ctx, "do.cond", fn_);
      auto* body_bb = llvm::BasicBlock::Create(u_.ctx, "do.body", fn_);
      auto* end_bb = llvm::BasicBlock::Create(u_.ctx, "do.end", fn_);

      u_.builder.SetInsertPoint(body_bb);
      {
        auto loop_frame{EnterStackFrame(ScopeManager::Frame::Kind::kLoop)};
        {
          lib::ScopedValue loop_ctx_guard(loop_ctx_, {.post = body_bb, .end = end_bb});
          CodegenStmt(m->body);
        }
        if (!u_.builder.GetInsertBlock()->getTerminator()) {
          u_.builder.CreateBr(cond_bb);
        }
      }

      u_.builder.CreateBr(cond_bb);
      u_.builder.SetInsertPoint(cond_bb);
      u_.builder.CreateCondBr(CodegenExprAsBool(m->cond), body_bb, end_bb);

      u_.builder.SetInsertPoint(end_bb);

      break;
    }

    case ast::NodeKind::ReturnStmt: {
      const auto* m = n->As<ast::nodes::ReturnStmt>();
      if (m->result) {
        u_.builder.CreateStore(CodegenExpr(m->result), retval_);
      }
      scope_->EmitDestructors();
      u_.builder.CreateBr(retbb_);
      break;
    }

    case ast::NodeKind::BranchStmt: {
      const auto* m = n->As<ast::nodes::BranchStmt>();
      scope_->EmitDestructorsInterruptingLoop();
      switch (m->kind.kind) {
        case ast::TokenKind::CONTINUE: {
          u_.builder.CreateBr(loop_ctx_.post);
          break;
        }
        case ast::TokenKind::BREAK: {
          u_.builder.CreateBr(loop_ctx_.end);
          break;
        }
        default: {
          assert(false);
          break;
        }
      }
      break;
    }

    case ast::NodeKind::DeclStmt: {
      const auto* m = n->As<ast::nodes::DeclStmt>();
      CodegenDecl(m->decl);
      break;
    }

    case ast::NodeKind::ExprStmt: {
      const auto* m = n->As<ast::nodes::ExprStmt>();
      CodegenExpr(m->expr);
      break;
    }

    default: {
      std::println("Codegen(Stmt) fail at NodeKind::{}", magic_enum::enum_name(n->nkind));
      break;
    }
  }
}

void FunctionCodegen::CodegenDecl(const ast::nodes::Decl* n) {
  switch (n->nkind) {
    case ast::NodeKind::ValueDecl: {
      const auto* m = n->As<ast::nodes::ValueDecl>();

      const auto itype = core::checker::ResolveExprType(&u_.sf, u_.sf.module->scope, m->type);
      for (auto* decl : m->decls) {
        auto* alloca = scope_->Alloc(Lit(decl->name), itype.sym);
        if (decl->value) {
          llvm::Value* v;
          {
            auto tmp_frame{EnterStackFrame(ScopeManager::Frame::Kind::kTemporaries)};
            v = CodegenExpr(decl->value, alloca);
          }
          if (IsTrivial(v->getType())) {
            u_.builder.CreateStore(v, alloca);
          }
        } else {
          u_.builder.CreateStore(u_.GetUndef(itype.sym), alloca);
        }
      }

      break;
    }

    default: {
      std::println("Codegen(Decl) fail at NodeKind::{}", magic_enum::enum_name(n->nkind));
      break;
    }
  }
}

llvm::Value* FunctionCodegen::CodegenExpr(const ast::nodes::Expr* expr, llvm::Value* dest) {
  switch (expr->nkind) {
    case ast::NodeKind::Ident: {
      const auto* m = expr->As<ast::nodes::Ident>();

      const auto& name = Lit(m);
      auto* alloca = scope_->Lookup(name)->alloca;
      return alloca;
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

        case ast::TokenKind::STRING: {
          auto* out = dest ? dest : scope_->AllocTemp(&core::builtins::kCharstring);
          const auto& sv = u_.ParseCharstring(m);
          assert(sv.length() <= std::numeric_limits<std::uint32_t>::max());
          u_.builder.CreateCall(u_.rt.charstring_init_f,
                                {
                                    out,
                                    u_.builder.CreateGlobalStringPtr(sv),
                                    u_.builder.getInt32(static_cast<std::uint32_t>(sv.length())),
                                });
          return out;
        }

        case ast::TokenKind::TRUE:
          return u_.rt.GetBool(true);
        case ast::TokenKind::FALSE:
          return u_.rt.GetBool(false);

        default:
          VANADIUM_DEBUG_ERROR("ValueLiteral unhandled token kind: {}", magic_enum::enum_name(m->tok.kind));
          break;
      }
    }

    case ast::NodeKind::BinaryExpr: {
      const auto* m = expr->As<ast::nodes::BinaryExpr>();

      auto* vx = CodegenExpr(m->x);
      auto* vy = CodegenExpr(m->y);

      const auto* sym =
          core::checker::ResolveExprType(&u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, m->x), m->x).sym;
      if (sym == &core::builtins::kInteger) {
        if (vx->getType() == u_.builder.getPtrTy()) {
          vx = u_.builder.CreateLoad(u_.rt.int_ty, vx);
        }
        if (vy->getType() == u_.builder.getPtrTy()) {
          vy = u_.builder.CreateLoad(u_.rt.int_ty, vy);
        }
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
            VANADIUM_DEBUG_ERROR("Unhandled BinaryExpr int op = {}", magic_enum::enum_name(m->op.kind));
            break;
        }
      } else if (sym == &core::builtins::kCharstring) {
        switch (m->op.kind) {
          case ast::TokenKind::CONCAT: {
            auto* out = dest ? dest : scope_->AllocTemp(&core::builtins::kCharstring);
            u_.builder.CreateCall(u_.rt.charstring_concat_f, {out, vx, vy});
            return out;
          }
          default:
            break;
        }
      } else {
        VANADIUM_DEBUG_ERROR("Unhandled BinaryExpr xsym = {}", sym->GetName());
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
                  names::CopyCtor(propsym),
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
      return u_.builder.CreateCall(u_.rt.type_new_f,
                                   {u_.mod.getOrInsertGlobal(names::TInfo(itype.sym), u_.rt.typeinfo_ty)});
    }

    case ast::NodeKind::CallExpr: {
      const auto* m = expr->As<ast::nodes::CallExpr>();

      auto tmp_frame{EnterStackFrame(ScopeManager::Frame::Kind::kTemporaries)};

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
          if (IsTrivial(val->getType())) {
            auto* val_slot = scope_->AllocTrivialTemp(val->getType());
            u_.builder.CreateStore(val, val_slot);
            val = val_slot;
          }

          const auto& isym = core::checker::ResolveExprType(
              &u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, argnode), argnode);
          EmitGenericVal(slot, isym.sym, val);
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

      auto* res = u_.builder.CreateCall(callee, args);

      return res;
    }

    case ast::NodeKind::ParenExpr: {
      const auto* m = expr->As<ast::nodes::ParenExpr>();
      assert(m->list.size() == 1);
      return CodegenExpr(m->list.front(), dest);
    }

    default: {
      VANADIUM_DEBUG_ERROR("Codegen(Expr) fail at NodeKind::{}", magic_enum::enum_name(expr->nkind));
      break;
    }
  }
  return nullptr;
}

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
