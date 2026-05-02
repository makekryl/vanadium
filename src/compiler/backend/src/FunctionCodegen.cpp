#include <cassert>
#include <forward_list>
#include <limits>
#include <optional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <variant>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/Argument.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Builtins.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/core/utils/SemanticUtils.h>
#include <vanadium/lib/Assert.h>
#include <vanadium/lib/ScopedValue.h>

#include "vanadium/compiler/Codegen.h"
#include "vanadium/compiler/IRHelpers.h"
#include "vanadium/compiler/LiteralsParser.h"
#include "vanadium/compiler/RuntimeBindings.h"
#include "vanadium/compiler/TypeSymbol.h"

namespace vanadium::compiler {

namespace {

struct AllocatedVar {
  llvm::Value* value;
  llvm::Type* ty;
  TypeSymbol ts;
  bool immutable{false};
};

void EmitDestructorCall(CodegenUnit& u, TypeSymbol ts, llvm::Value* val) {
  if (ts) {
    if (!u.IsOpaque(ts)) {
      auto* dtor_f = [&] -> llvm::Function* {
        if (const auto* strb = u.GetStringTypeBindings(ts)) {
          return strb->dtor_f;
        }
        return nullptr;
      }();
      if (dtor_f) {
        // TODO: partial inline (is_bound && is_ext) in RuntimeBindings
        u.builder.CreateCall(dtor_f, {val});
      }
    } else {
      auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(val);
      assert(alloca);
      u.builder.CreateCall(u.rt.type_del_f, {
                                                u.mod.getGlobalVariable(names::TInfo(ts)),
                                                u.builder.CreateLoad(alloca->getAllocatedType(), alloca),
                                            });
    }
  }
  u.builder.CreateLifetimeEnd(val);
}

class ScopeManager {
 public:
  ScopeManager(CodegenUnit& u, llvm::Function* fn) : u_(u), fn_(fn) {}

  struct Frame {
    enum class Kind : std::uint8_t {
      kRegular,
      kTemporaries,
      kLoop,
    };

    // 'temporaries' is used is purely as an AllocatedVar storage
    // TODO: maybe store AllocatedVar* in vars, make ordered_vars a forward_list or any other iterator-stable container;
    //       and get rid of temporaroes
    std::unordered_map<std::string_view, AllocatedVar> vars;
    std::forward_list<AllocatedVar> temporaries;
    std::vector<AllocatedVar*> ordered_vars;

    llvm::BasicBlock::iterator begin;
    Kind kind;

    bool uses_stackalloc{false};
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
      EmitDestructorCalls(frame);
    } else {
      assert(false);
    }
  }

  void EmitDestructorCalls() {
    for (const auto& frame : frame_stack_ | std::views::reverse) {
      EmitDestructorCalls(frame);
    }
  }
  void EmitDestructorsInterruptingLoop() {
    for (const auto& frame : frame_stack_ | std::views::reverse) {
      EmitDestructorCalls(frame);
      if (frame.kind == Frame::Kind::kLoop) {
        break;
      }
    }
  }

  llvm::AllocaInst* Alloc(std::string_view name, TypeSymbol ts) {
    auto* ty = u_.GetSymbolType(ts);
    auto* alloca = createEntryBlockAlloca(ty, name);
    u_.builder.CreateLifetimeStart(alloca);

    auto& frame = frame_stack_.back();
    const auto& [it, _] = frame.vars.emplace(name, AllocatedVar{
                                                       .value = alloca,
                                                       .ty = ty,
                                                       .ts = ts,
                                                   });
    frame.ordered_vars.emplace_back(&it->second);

    return alloca;
  }
  llvm::AllocaInst* AllocTemp(TypeSymbol ts) {
    auto* ty = u_.GetSymbolType(ts);
    auto* alloca = createEntryBlockAlloca(ty, "tmp");
    u_.builder.CreateLifetimeStart(alloca);

    auto& frame = frame_stack_.back();
    auto& var = frame.temporaries.emplace_front(AllocatedVar{
        .value = alloca,
        .ty = ty,
        .ts = ts,
    });
    frame.ordered_vars.emplace_back(&var);

    return alloca;
  }
  llvm::AllocaInst* AllocTrivialTemp(llvm::Type* ty, std::string_view name = "") {
    auto* alloca = createEntryBlockAlloca(ty, name);
    u_.builder.CreateLifetimeStart(alloca);

    auto& frame = frame_stack_.back();
    auto& var = frame.temporaries.emplace_front(AllocatedVar{.value = alloca, .ty = ty, .ts = nullptr});
    frame.ordered_vars.emplace_back(&var);

    return alloca;
  }
  void BindArgument(std::string_view name, TypeSymbol ts, llvm::Argument* arg, bool immutable) {
    args_[name] = {.value = arg, .ty = u_.GetSymbolType(ts), .ts = ts, .immutable = immutable};
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
  llvm::AllocaInst* createEntryBlockAlloca(llvm::Type* ty, std::string_view name) {
    llvm::IRBuilder<> tmp(&fn_->getEntryBlock(), fn_->getEntryBlock().begin());
    auto* alloca = tmp.CreateAlloca(ty, nullptr, name);

    if (!frame_stack_.empty()) {
      auto& frame = frame_stack_.back();
      if (frame.begin->getNextNode() && llvm::dyn_cast<llvm::AllocaInst>(&(*frame.begin->getNextNode()))) {
        frame.begin++;
      }
    }

    return alloca;
  }

 private:
  void EmitDestructorCalls(const Frame& frame) {
    if (frame.ordered_vars.empty()) {
      return;
    }

    if (frame.uses_stackalloc) {
      llvm::IRBuilder<> tmp_builder(frame.begin->getNextNode());
      tmp_builder.CreateCall(u_.rt.stackalloc_mark_f);
    }

    for (const auto& var : frame.ordered_vars | std::views::reverse) {
      EmitDestructorCall(u_, var->ts, var->value);
    }

    if (frame.uses_stackalloc) {
      u_.builder.CreateCall(u_.rt.stackalloc_sweep_f);
    }
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

struct DestSlot {
  CodegenUnit* u{nullptr};
  llvm::Value* val{nullptr};
  TypeSymbol ts{nullptr};

  operator bool() const {
    return val != nullptr;
  }

  operator llvm::Value*() const {
    if (!ts.is_template) {
      return val;
    }
    return u->builder.CreateCall(u->rt.tpl.value, {u->GetTypeInfo(ts), val});
  }

  [[nodiscard]] bool IsTemplate() const {
    return val != nullptr && ts != nullptr;
  }

  // Even if DestSlot holds a template, returns direct reference
  // (i.e. it's a fancy way to access .val)
  [[nodiscard]] llvm::Value* Unwrap() const {
    return val;
  }
};

class FunctionCodegen {
 public:
  FunctionCodegen(CodegenUnit& u) : u_(u) {}

  void Generate(const core::semantic::Symbol* sym, const ast::nodes::FuncDecl* m) {
    const bool does_return = m->ret != nullptr;

    fn_ = u_.GetFunction(sym);
    if (m->external) {
      return;
    }

    dbgisp_ = nullptr;
    u_.EmitDebugInfo([&](DebugInfo& di) {
      // TODO: set real line numbers
      dbgisp_ = di.builder.createFunction(di.file, fn_->getName(), llvm::StringRef{}, di.file, 5,
                                          di.builder.createSubroutineType(di.builder.getOrCreateTypeArray({})), 5,
                                          llvm::DINode::FlagZero, llvm::DISubprogram::SPFlagDefinition);
      fn_->setSubprogram(dbgisp_);
    });

    scope_.emplace(u_, fn_);

    retbb_ = llvm::BasicBlock::Create(u_.ctx, "exit");

    auto* entry = llvm::BasicBlock::Create(u_.ctx, "", fn_);
    u_.builder.SetInsertPoint(entry);

    if (does_return) {
      assert(fn_->arg_size() >= 1);
      auto* retarg = fn_->getArg(0);
      retarg->setName("retval");
      retval_ = retarg;
    } else {
      retval_ = nullptr;
    }

    for (auto [arg, param] : std::views::zip(fn_->args() | std::views::drop(does_return ? 1 : 0), m->params->list)) {
      const auto& isym = core::checker::ResolveExprSymbol(&u_.sf, u_.sf.module->scope, param->type);

      const auto& name = Lit(param->name);
      arg.setName(name);

      scope_->BindArgument(name, isym, &arg,
                           param->direction == nullptr || param->direction->kind == ast::TokenKind::IN);
    }

    CodegenStmt(m->body);

    if (!u_.builder.GetInsertBlock()->getTerminator()) {
      scope_->EmitDestructorCalls();
      u_.builder.CreateBr(retbb_);
    }

    fn_->insert(fn_->end(), retbb_);
    u_.builder.SetInsertPoint(retbb_);
    u_.builder.CreateRetVoid();
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

  DestSlot asDest(llvm::Value* val) {
    return {.val = val};
  }
  DestSlot asDest(llvm::Value* val, TypeSymbol ts) {
    return {.u = &u_, .val = val, .ts = ts};
  }

  void CodegenStmt(const ast::nodes::Stmt*);
  void CodegenDecl(const ast::nodes::Decl*);
  llvm::Value* CodegenExpr(const ast::nodes::Expr*, DestSlot dest = {});

  void EmitGenericVal(llvm::Value* slot, TypeSymbol ts, llvm::Value* v) {
    // p field
    auto* p_gep = u_.builder.CreateStructGEP(u_.rt.generic_val_ty, slot, 0, "gvt_p");
    u_.builder.CreateStore(v, p_gep);

    // ty field
    auto* ty_gep = u_.builder.CreateStructGEP(u_.rt.generic_val_ty, slot, 1, "gvt_ty");
    u_.builder.CreateStore(u_.getOrDeclareExternalConst(names::TInfo(ts), u_.rt.typeinfo_ty), ty_gep);

    std::println("EmitGenericVal name={}, tpl={}", ts->GetName(), ts.is_template);
  }

  llvm::Value* retval_;
  llvm::BasicBlock* retbb_;
  llvm::DISubprogram* dbgisp_;
  struct {
    llvm::BasicBlock* post;
    llvm::BasicBlock* end;
  } loop_ctx_;
  llvm::Function* fn_;

  std::optional<ScopeManager> scope_;
  CodegenUnit& u_;
};

void FunctionCodegen::CodegenStmt(const ast::nodes::Stmt* n) {
  u_.EmitDebugInfo([&](auto&) {
    const auto ast_loc = u_.sf.ast.lines.Translate(n->nrange.begin);
    u_.builder.SetCurrentDebugLocation(llvm::DILocation::get(u_.ctx, ast_loc.line + 1, ast_loc.column + 1, dbgisp_));
  });
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

      u_.builder.CreateCondBr(
          [&] {
            auto tmp_frame{EnterStackFrame(ScopeManager::Frame::Kind::kTemporaries)};
            return u_.UnwrapBoolOrBoxedBoolPtr(CodegenExpr(m->cond));
          }(),
          then_bb, else_bb);

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
      u_.builder.CreateCondBr(
          [&] {
            auto tmp_frame{EnterStackFrame(ScopeManager::Frame::Kind::kTemporaries)};
            return u_.UnwrapBoolOrBoxedBoolPtr(CodegenExpr(m->cond));
          }(),
          body_bb, end_bb);

      u_.builder.SetInsertPoint(post_bb);
      CodegenStmt(m->post);
      u_.builder.CreateBr(cond_bb);

      u_.builder.SetInsertPoint(body_bb);
      {
        {
          auto loop_frame{EnterStackFrame(ScopeManager::Frame::Kind::kLoop)};
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
      u_.builder.CreateCondBr(
          [&] {
            auto tmp_frame{EnterStackFrame(ScopeManager::Frame::Kind::kTemporaries)};
            return u_.UnwrapBoolOrBoxedBoolPtr(CodegenExpr(m->cond));
          }(),
          body_bb, end_bb);

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
      u_.builder.CreateCondBr(
          [&] {
            auto tmp_frame{EnterStackFrame(ScopeManager::Frame::Kind::kTemporaries)};
            return u_.UnwrapBoolOrBoxedBoolPtr(CodegenExpr(m->cond));
          }(),
          body_bb, end_bb);

      u_.builder.SetInsertPoint(end_bb);

      break;
    }

    case ast::NodeKind::ReturnStmt: {
      const auto* m = n->As<ast::nodes::ReturnStmt>();
      if (m->result) {
        CodegenExpr(m->result, asDest(retval_));
      }
      scope_->EmitDestructorCalls();
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

      const TypeSymbol ts{core::checker::ResolveExprType(&u_.sf, u_.sf.module->scope, m->type).sym,
                          m->restriction != nullptr};
      const bool is_opaque = u_.IsOpaque(ts);

      for (auto* decl : m->decls) {
        auto* alloca = scope_->Alloc(Lit(decl->name), ts);
        // TODO: unify w/ AssignmentExpr
        llvm::Value* dest = alloca;
        if (is_opaque) {
          dest = u_.builder.CreateCall(u_.rt.type_new_f, {u_.GetTypeInfo(ts)});
          u_.builder.CreateStore(dest, alloca);
        }
        if (decl->value) {
          auto tmp_frame{EnterStackFrame(ScopeManager::Frame::Kind::kTemporaries)};
          CodegenExpr(decl->value, asDest(dest, ts));
        } else if (!is_opaque) {
          u_.builder.CreateStore(u_.GetUndef(ts), alloca);
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

llvm::Value* FunctionCodegen::CodegenExpr(const ast::nodes::Expr* expr, DestSlot dest) {
  const auto promote_trivial = [this](llvm::Type* ty, llvm::Value* v) -> llvm::Value* {
    if (v->getType()->isPointerTy()) {
      return u_.builder.CreateLoad(ty, v);
    }
    return u_.WrapValue(v);
  };
  const auto ret_trivial = [&](llvm::Value* rv) -> llvm::Value* {
    if (dest) {
      u_.builder.CreateStore(u_.WrapValue(rv), dest);
      return dest;
    }
    return rv;
  };

  switch (expr->nkind) {
    case ast::NodeKind::Ident: {
      const auto* m = expr->As<ast::nodes::Ident>();

      if (const auto* var = scope_->Lookup(Lit(m))) {
        // todo: deep copy
        if (dest) {
          u_.builder.CreateStore(u_.builder.CreateLoad(var->ty, var->value), dest);
          return dest;
        }

        return var->value;
      }

      const auto& isym =
          core::checker::ResolveExprType(&u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, m), m);
      assert(isym);
      if (isym->Flags() & core::semantic::SymbolFlags::kEnumMember) {
        // TODO(tc): calculate actual value taking explicitly set members into account
        // TODO: support EnumSpec, move in another function
        const auto* etd = isym->Declaration()->parent->As<ast::nodes::EnumTypeDecl>();
        std::int32_t val{0};
        for (const auto* v : etd->values) {
          if (v == isym->Declaration()) {
            break;
          }
          ++val;
        }
        return u_.rt.GetInt(val);
      }

      return nullptr;
    }

    case ast::NodeKind::IndexExpr: {
      const auto* m = expr->As<ast::nodes::IndexExpr>();

      auto* vx = CodegenExpr(m->x);
      auto* vidx = CodegenExpr(m->index);

      // TODO: remove all FindScope in FunctionCodegen
      const auto* xsym =
          core::checker::ResolveExprType(&u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, m), m->x).sym;

      if (xsym->Flags() & core::semantic::SymbolFlags::kBuiltinString) {
        const StringTypeBindings* strb = u_.GetStringTypeBindings(xsym);
        VANADIUM_DEBUG_ASSERT(strb, "Unhandled string type");

        auto* out = dest ? dest : scope_->AllocTemp(xsym);
        u_.builder.CreateCall(strb->singular_f, {
                                                    out,
                                                    vx,
                                                    u_.UnwrapValue(vidx),
                                                });
        return out;
      }

      VANADIUM_DEBUG_ERROR("Unhandled IndexExpr type");
      return nullptr;
    }

    case ast::NodeKind::ValueLiteral: {
      const auto* m = expr->As<ast::nodes::ValueLiteral>();
      switch (m->tok.kind) {
        case ast::TokenKind::INT: {
          const auto& intv = literals::ParseInt(u_.sf.Text(m->tok));
          assert(std::holds_alternative<RuntimeBindings::NativeIntType>(intv));
          return ret_trivial(u_.rt.GetRawInt(std::get<RuntimeBindings::NativeIntType>(intv)));
        }

        case ast::TokenKind::FLOAT: {
          return ret_trivial(u_.rt.GetRawFloat(literals::ParseFloat(u_.sf.Text(m->tok))));
        }

        case ast::TokenKind::TRUE:
        case ast::TokenKind::FALSE: {
          return ret_trivial(u_.builder.getInt1(m->tok.kind == ast::TokenKind::TRUE));
        }

        case ast::TokenKind::STRING: {
          auto* out = dest ? dest : scope_->AllocTemp(&core::builtins::kCharstring);
          const auto& sv = literals::ParseCharstring(u_.sf.Text(m->tok));
          assert(sv.length() <= std::numeric_limits<std::uint32_t>::max());
          u_.builder.CreateCall(u_.rt.charstring.init_f,
                                {
                                    out,
                                    u_.builder.CreateGlobalStringPtr(sv),
                                    u_.builder.getInt32(static_cast<std::uint32_t>(sv.length())),
                                });
          return out;
        }

        case ast::TokenKind::OCTETSTRING: {
          auto* out = dest ? dest : scope_->AllocTemp(&core::builtins::kOctetstring);
          const auto& sv = literals::ParseOctetstring(u_.sf.Text(m->tok));
          assert(sv.size() <= std::numeric_limits<std::uint32_t>::max());
          u_.builder.CreateCall(u_.rt.octetstring.init_f,
                                {
                                    out,
                                    helpers::CreateGlobalBytePtr(u_.mod, sv),
                                    u_.builder.getInt32(static_cast<std::uint32_t>(sv.size())),
                                });
          return out;
        }

        case ast::TokenKind::BITSTRING: {
          auto* out = dest ? dest : scope_->AllocTemp(&core::builtins::kBitstring);
          const auto& [sv, bits] = literals::ParseBitstring(u_.sf.Text(m->tok));
          u_.builder.CreateCall(u_.rt.bitstring.init_f, {
                                                            out,
                                                            helpers::CreateGlobalBytePtr(u_.mod, sv),
                                                            u_.builder.getInt32(bits),
                                                        });
          return out;
        }

        case ast::TokenKind::HEXSTRING: {
          auto* out = dest ? dest : scope_->AllocTemp(&core::builtins::kHexstring);
          const auto& [sv, nibbles] = literals::ParseHexstring(u_.sf.Text(m->tok));
          u_.builder.CreateCall(u_.rt.hexstring.init_f, {
                                                            out,
                                                            helpers::CreateGlobalBytePtr(u_.mod, sv),
                                                            u_.builder.getInt32(nibbles),
                                                        });
          return out;
        }

        default:
          VANADIUM_DEBUG_ERROR("ValueLiteral unhandled token kind: {}", magic_enum::enum_name(m->tok.kind));
          break;
      }
      break;
    }

    case ast::NodeKind::BinaryExpr: {
      const auto* m = expr->As<ast::nodes::BinaryExpr>();

      auto* vx = CodegenExpr(m->x);
      auto* vy = CodegenExpr(m->y);

      const auto* sym =
          core::checker::ResolveExprType(&u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, m->x), m->x).sym;

      if (sym == &core::builtins::kBoolean) {
        // TODO: cleanup promote_trivial calls here and below
        vx = promote_trivial(u_.rt.boolt.ty, vx);
        vy = promote_trivial(u_.rt.boolt.ty, vy);
        switch (m->op.kind) {
          case ast::TokenKind::EQ:
            return u_.builder.CreateCall(u_.rt.boolt.eq_f, {vx, vy});
          case ast::TokenKind::NE:
            return u_.builder.CreateCall(u_.rt.boolt.ne_f, {vx, vy});
          //
          default:
            VANADIUM_DEBUG_ERROR("Unhandled BinaryExpr bool op = {}", magic_enum::enum_name(m->op.kind));
            break;
        }
      } else if (sym == &core::builtins::kInteger) {
        if (m->op.kind == ast::TokenKind::RANGE) {
          // TODO: hook into CodegenExpr above to check for UnaryExpr w/ '!'
          u_.builder.CreateCall(u_.rt.tpl.range_integer, {
                                                             dest.Unwrap(),  //
                                                             u_.UnwrapValue(vx),
                                                             u_.builder.getFalse(),  // todo
                                                             u_.UnwrapValue(vy),
                                                             u_.builder.getFalse(),  // todo
                                                         });
          return dest.Unwrap();
        }
        vx = promote_trivial(u_.rt.integer.ty, vx);
        vy = promote_trivial(u_.rt.integer.ty, vy);
        switch (m->op.kind) {
          case ast::TokenKind::EQ:
            return u_.builder.CreateCall(u_.rt.integer.eq_f, {vx, vy});
          case ast::TokenKind::NE:
            return u_.builder.CreateCall(u_.rt.integer.ne_f, {vx, vy});
          //
          case ast::TokenKind::LT:
            return u_.builder.CreateCall(u_.rt.integer.lt_f, {vx, vy});
          case ast::TokenKind::LE:
            return u_.builder.CreateCall(u_.rt.integer.le_f, {vx, vy});
          case ast::TokenKind::GT:
            return u_.builder.CreateCall(u_.rt.integer.gt_f, {vx, vy});
          case ast::TokenKind::GE:
            return u_.builder.CreateCall(u_.rt.integer.ge_f, {vx, vy});
          //
          case ast::TokenKind::ADD:
            return ret_trivial(u_.builder.CreateCall(u_.rt.integer.add_f, {vx, vy}));
          case ast::TokenKind::SUB:
            return ret_trivial(u_.builder.CreateCall(u_.rt.integer.sub_f, {vx, vy}));
          case ast::TokenKind::MUL:
            return ret_trivial(u_.builder.CreateCall(u_.rt.integer.mul_f, {vx, vy}));
          case ast::TokenKind::DIV:
            return ret_trivial(u_.builder.CreateCall(u_.rt.integer.div_f, {vx, vy}));
          default:
            VANADIUM_DEBUG_ERROR("Unhandled BinaryExpr int op = {}", magic_enum::enum_name(m->op.kind));
            break;
        }
      } else if (sym == &core::builtins::kFloat) {
        vx = promote_trivial(u_.rt.floatt.ty, vx);
        vy = promote_trivial(u_.rt.floatt.ty, vy);
        switch (m->op.kind) {
          case ast::TokenKind::EQ:
            return u_.builder.CreateCall(u_.rt.floatt.eq_f, {vx, vy});
          case ast::TokenKind::NE:
            return u_.builder.CreateCall(u_.rt.floatt.ne_f, {vx, vy});
          //
          case ast::TokenKind::LT:
            return u_.builder.CreateCall(u_.rt.floatt.lt_f, {vx, vy});
          case ast::TokenKind::LE:
            return u_.builder.CreateCall(u_.rt.floatt.le_f, {vx, vy});
          case ast::TokenKind::GT:
            return u_.builder.CreateCall(u_.rt.floatt.gt_f, {vx, vy});
          case ast::TokenKind::GE:
            return u_.builder.CreateCall(u_.rt.floatt.ge_f, {vx, vy});
          //
          case ast::TokenKind::ADD:
            return ret_trivial(u_.builder.CreateCall(u_.rt.floatt.add_f, {vx, vy}));
          case ast::TokenKind::SUB:
            return ret_trivial(u_.builder.CreateCall(u_.rt.floatt.sub_f, {vx, vy}));
          case ast::TokenKind::MUL:
            return ret_trivial(u_.builder.CreateCall(u_.rt.floatt.mul_f, {vx, vy}));
          case ast::TokenKind::DIV:
            return ret_trivial(u_.builder.CreateCall(u_.rt.floatt.div_f, {vx, vy}));
          default:
            VANADIUM_DEBUG_ERROR("Unhandled BinaryExpr int op = {}", magic_enum::enum_name(m->op.kind));
            break;
        }
      } else {
        const StringTypeBindings* strb = u_.GetStringTypeBindings(sym);
        VANADIUM_DEBUG_ASSERT(strb, "Unhandled BinaryExpr xsym = {}", sym->GetName());

        switch (m->op.kind) {
          case ast::TokenKind::EQ:
            return u_.builder.CreateCall(strb->eq_f, {vx, vy});
          case ast::TokenKind::NE:
            return u_.builder.CreateCall(strb->ne_f, {vx, vy});
          default:
            break;
        }

        auto* out = dest ? dest : scope_->AllocTemp(sym);
        switch (m->op.kind) {
          case ast::TokenKind::CONCAT: {
            u_.builder.CreateCall(strb->concat_f, {out, vx, vy});
            break;
          }
          case ast::TokenKind::ROL: {
            u_.builder.CreateCall(strb->rotate_left_f, {out, vx, u_.UnwrapValue(vy)});
            break;
          }
          case ast::TokenKind::ROR: {
            u_.builder.CreateCall(strb->rotate_right_f, {out, vx, u_.UnwrapValue(vy)});
            break;
          }
          case ast::TokenKind::SHL: {
            u_.builder.CreateCall(strb->shift_left_f, {out, vx, u_.UnwrapValue(vy)});
            break;
          }
          case ast::TokenKind::SHR: {
            u_.builder.CreateCall(strb->shift_right_f, {out, vx, u_.UnwrapValue(vy)});
            break;
          }
          case ast::TokenKind::AND4B: {
            u_.builder.CreateCall(strb->and4b_f, {out, vx, vy});
            break;
          }
          case ast::TokenKind::OR4B: {
            u_.builder.CreateCall(strb->or4b_f, {out, vx, vy});
            break;
          }
          case ast::TokenKind::XOR4B: {
            u_.builder.CreateCall(strb->xor4b_f, {out, vx, vy});
            break;
          }
          default: {
            VANADIUM_DEBUG_ERROR("Unhandled BinaryExpr str operation = {}", magic_enum::enum_name(m->op.kind));
            break;
          }
        }
        return out;
      }

      break;
    }

    case ast::NodeKind::UnaryExpr: {
      const auto* m = expr->As<ast::nodes::UnaryExpr>();

      auto* vx = CodegenExpr(m->x);

      const auto* sym =
          core::checker::ResolveExprType(&u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, m->x), m->x).sym;

      if (sym == &core::builtins::kBoolean) {
        return ret_trivial(u_.builder.CreateCall(u_.rt.boolt.not_f, {promote_trivial(u_.rt.boolt.ty, vx)}));
      } else if (sym == &core::builtins::kInteger) {
        vx = promote_trivial(u_.rt.integer.ty, vx);
        switch (m->op.kind) {
          case ast::TokenKind::SUB:
            return ret_trivial(u_.builder.CreateCall(u_.rt.integer.neg_f, {vx}));
          default:
            VANADIUM_DEBUG_ERROR("Unhandled UnaryExpr int op = {}", magic_enum::enum_name(m->op.kind));
            break;
        }
      } else if (sym == &core::builtins::kFloat) {
        vx = promote_trivial(u_.rt.floatt.ty, vx);
        switch (m->op.kind) {
          case ast::TokenKind::SUB:
            return ret_trivial(u_.builder.CreateCall(u_.rt.floatt.neg_f, {vx}));
          default:
            VANADIUM_DEBUG_ERROR("Unhandled UnaryExpr int op = {}", magic_enum::enum_name(m->op.kind));
            break;
        }
      } else {
        const StringTypeBindings* strb = u_.GetStringTypeBindings(sym);
        VANADIUM_DEBUG_ASSERT(strb, "Unhandled BinaryExpr xsym = {}", sym->GetName());

        auto* out = dest ? dest : scope_->AllocTemp(sym);
        switch (m->op.kind) {
          case ast::TokenKind::NOT4B: {
            u_.builder.CreateCall(strb->not4b_f, {out, vx});
            break;
          }
          default: {
            VANADIUM_DEBUG_ERROR("Unhandled UnaryExpr str operation = {}", magic_enum::enum_name(m->op.kind));
            break;
          }
        }
        return out;
      }

      break;
    }

    case ast::NodeKind::SelectorExpr: {
      const auto* m = expr->As<ast::nodes::SelectorExpr>();

      // TODO: optimize xsym chain resolution
      auto* vx = CodegenExpr(m->x);
      if (llvm::dyn_cast<llvm::AllocaInst>(vx)) {
        vx = u_.builder.CreateLoad(u_.builder.getPtrTy(), vx);
      }
      const auto& xsym =
          core::checker::ResolveExprType(&u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, m), m->x);
      return u_.builder.CreateCall(
          u_.getOrDeclareExternalFunc(names::Getter(xsym, Lit(m->sel)), u_.rt.generic_getter_fn_ty), {vx});
    }

    case ast::NodeKind::AssignmentExpr: {
      const auto* m = expr->As<ast::nodes::AssignmentExpr>();

      switch (m->property->nkind) {
        case ast::NodeKind::Ident: {
          const auto* var = scope_->Lookup(Lit(m->property));
          if (var->immutable) {
            auto* owned_var_copy = scope_->Alloc(var->value->getName(), var->ts);
            CodegenExpr(m->value, asDest(owned_var_copy));
          } else {
            if (u_.IsTrivial(var->ty)) {
              // no dtor call needed
              CodegenExpr(m->value, asDest(var->value));
            } else {
              if (var->ty->isPointerTy()) {
                auto* old_val = u_.builder.CreateLoad(var->ty, var->value);
                CodegenExpr(m->value, asDest(var->value));
                EmitDestructorCall(u_, var->ts, old_val);
              } else {
                // charstring, ...
                auto* tmp_alloca = scope_->createEntryBlockAlloca(var->ty, "tmpref");
                u_.builder.CreateLifetimeStart(tmp_alloca);
                const auto& align = tmp_alloca->getAlign();
                u_.builder.CreateMemCpy(tmp_alloca, align, var->value, align,
                                        u_.mod.getDataLayout().getTypeAllocSize(var->ty));
                //
                CodegenExpr(m->value, asDest(var->value));
                EmitDestructorCall(u_, var->ts, tmp_alloca);
                //
                u_.builder.CreateLifetimeEnd(tmp_alloca);
              }
            }
          }

          return nullptr;
        }
        case ast::NodeKind::SelectorExpr: {
          // TODO: this should be rewriten during implementing of OOP support
          // a.b().c().d := ...;
          const auto cgen_mutable_val = [&](this auto&& self, const ast::nodes::Expr* xse) -> llvm::Value* {
            switch (xse->nkind) {
              case ast::NodeKind::SelectorExpr: {
                const auto* se = xse->As<ast::nodes::SelectorExpr>();
                auto* xval = self(se->x);
                // TODO: optimize xsym chain resolution
                const auto& xsym = core::checker::ResolveExprType(
                    &u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, m), se->x);
                return u_.builder.CreateCall(
                    u_.getOrDeclareExternalFunc(names::Getter(xsym, Lit(se->sel)), u_.rt.generic_getter_fn_ty), {xval});
              }
              case ast::NodeKind::Ident: {
                const auto* root_var = scope_->Lookup(Lit(xse));
                assert(u_.IsOpaque(root_var->ts));
                auto* root_val = root_var->value;
                if (root_var->immutable) {
                  root_val = scope_->Alloc(root_var->value->getName(), root_var->ts);
                  // TODO: deep copy
                }
                if (llvm::dyn_cast<llvm::AllocaInst>(root_val)) {
                  root_val = u_.builder.CreateLoad(u_.builder.getPtrTy(), root_val);
                }
                return root_val;
              }
              default:
                // CallExpr?
                return CodegenExpr(xse);
            }
          };

          const auto* pse = m->property->As<ast::nodes::SelectorExpr>();
          auto* pxval = cgen_mutable_val(pse->x);
          // TODO: optimize xsym chain resolution
          const auto& pxsym =
              core::checker::ResolveExprType(&u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, m), pse->x);
          auto* pxval_ptr = u_.builder.CreateCall(
              u_.getOrDeclareExternalFunc(names::Muttor(pxsym, Lit(pse->sel)), u_.rt.generic_getter_fn_ty), {pxval});
          CodegenExpr(m->value, asDest(pxval_ptr, pxsym));

          return nullptr;
        }
        default: {
          assert(false);
          break;
        }
      }

      break;
    }

    case ast::NodeKind::CompositeLiteral: {
      assert(dest);
      assert(dest.ts);
      // TODO: list support

      const auto* m = expr->As<ast::nodes::CompositeLiteral>();
      for (const auto& attr : m->list) {
        // todo: positionals
        assert(attr->nkind == ast::NodeKind::AssignmentExpr);
        const auto* ae = attr->As<ast::nodes::AssignmentExpr>();
        const auto& get_accessor =
            (ae->value->nkind == ast::NodeKind::CompositeLiteral) ? names::Getter : names::Muttor;

        const auto& attr_sym = dest.ts.Derive([&] {
          const auto& property_name = Lit(ae->property);
          const auto* property_sym = dest.ts->Members()->Lookup(property_name);
          const auto* fnode = property_sym->Declaration()->As<ast::nodes::Field>();
          if (fnode->type->nkind == ast::NodeKind::RefSpec) {
            const auto* fnode_file = ast::utils::SourceFileOf(fnode);
            return core::checker::ResolveTypeSpecSymbol(fnode_file, fnode->type);
          }
          return dest.ts->Members()->LookupShadow(property_name);
        }());

        auto* pxval_ptr = u_.builder.CreateCall(
            u_.getOrDeclareExternalFunc(get_accessor(dest.ts, Lit(ae->property)), u_.rt.generic_getter_fn_ty), {dest});
        CodegenExpr(ae->value, asDest(pxval_ptr, attr_sym));
      }

      return dest.Unwrap();
    }

    case ast::NodeKind::CallExpr: {
      const auto* m = expr->As<ast::nodes::CallExpr>();

      const auto* func_sym = core::checker::ResolveExprSymbol(&u_.sf, u_.sf.module->scope, m->fun).sym;
      assert(func_sym->Flags() & core::semantic::SymbolFlags::kFunction);

      const bool does_return = ast::utils::DoesFunctionLikeReturn(func_sym->Declaration());
      if (does_return && !dest) {
        const auto ret_isym = core::checker::ResolveCallableReturnType(
            ast::utils::SourceFileOf(func_sym->Declaration()), func_sym->Declaration()->As<ast::nodes::Decl>());
        assert(ret_isym);
        dest = asDest(scope_->AllocTemp(ret_isym), ret_isym);
      }

      auto tmp_frame{EnterStackFrame(ScopeManager::Frame::Kind::kTemporaries)};
      const auto prepare_argument = [this](llvm::Value* av) -> llvm::Value* {
        if (!av->getType()->isPointerTy()) {
          auto* val_slot = scope_->AllocTrivialTemp(av->getType());
          u_.builder.CreateStore(av, val_slot);
          return val_slot;
        }
        if (auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(av); alloca && alloca->getAllocatedType()->isPointerTy()) {
          return u_.builder.CreateLoad(alloca->getAllocatedType(), alloca);
        }
        return av;
      };

      auto* callee = u_.GetFunction(func_sym);

      if (callee->hasFnAttribute(kVarargsAttr)) {
        const std::size_t n = m->args->list.size();

        auto* array_ty = llvm::ArrayType::get(u_.rt.generic_val_ty, n);
        auto* arr = scope_->AllocTrivialTemp(array_ty, "vargs_arr");
        for (const auto& [idx, argnode] : m->args->list | std::views::enumerate) {
          auto* slot =
              u_.builder.CreateGEP(array_ty, arr, {u_.builder.getInt32(0), u_.builder.getInt32(idx)}, "generic_arg");

          auto* av = prepare_argument(u_.WrapValue(CodegenExpr(argnode)));
          const auto& isym = core::checker::ResolveExprType(
              &u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, argnode), argnode);
          EmitGenericVal(slot, isym, av);
        }
        auto* res = u_.builder.CreateCall(callee, {arr, u_.builder.getInt32(n)});
        return res;
      }

      std::vector<llvm::Value*> args;
      args.reserve(m->args->list.size() + (does_return ? 1 : 0));
      if (does_return) {
        assert(dest);
        args.push_back(dest);
      }
      for (const auto& [idx, argnode] : m->args->list | std::views::enumerate) {
        auto* av = prepare_argument(u_.WrapValue(CodegenExpr(argnode)));
        VANADIUM_DEBUG_ASSERT(av != nullptr, "Unknown argument expr type: {}", magic_enum::enum_name(argnode->nkind));
        if (!callee->getAttributes().hasParamAttr((does_return ? 1 : 0) + idx, kGenericArgAttr)) {
          args.push_back(av);
        } else {
          auto* slot = scope_->AllocTrivialTemp(u_.rt.generic_val_ty, "garg");
          const auto& isym = core::checker::ResolveExprType(
              &u_.sf, core::semantic::utils::FindScope(u_.sf.module->scope, argnode), argnode);
          EmitGenericVal(slot, isym, av);
          args.push_back(slot);
        }
      }

      u_.builder.CreateCall(callee, args);

      return dest.Unwrap();
    }

    case ast::NodeKind::ParenExpr: {
      const auto* m = expr->As<ast::nodes::ParenExpr>();
      if (m->list.size() == 1) {
        return CodegenExpr(m->list.front(), dest);
      }
      assert(dest.IsTemplate());

      auto tmp_frame{EnterStackFrame(ScopeManager::Frame::Kind::kTemporaries)};
      auto* esz_slot = scope_->AllocTrivialTemp(u_.rt.sizet_ty, "esz_slot");
      llvm::Value* vlist_ptr = u_.builder.CreateCall(
          u_.rt.tpl.list, {
                              u_.GetTypeInfo(dest.ts),
                              dest.Unwrap(),
                              llvm::ConstantInt::get(u_.rt.tpl.listsize_ty, m->list.size()),
                              esz_slot,
                              llvm::ConstantInt::get(u_.rt.tpl.tsel_ty, 5),  // TODO: UNHARDCODE THIS CONSTANT
                          });
      auto* esz = u_.builder.CreateLoad(esz_slot->getAllocatedType(), esz_slot, "esz");
      for (const auto& el : m->list) {
        CodegenExpr(el, asDest(vlist_ptr, dest.ts));
        vlist_ptr = u_.builder.CreateGEP(u_.builder.getInt8Ty(), vlist_ptr, esz);
      }

      return dest.Unwrap();
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
