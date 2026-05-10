#include <cassert>
#include <cctype>
#include <filesystem>

#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Builtins.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/lib/Assert.h>

#include "vanadium/compiler/ABIHelper.h"
#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler {

DebugInfo::DebugInfo(CodegenUnit& u)
    : builder{u.mod},
      file{[&] {
        const std::filesystem::path p(u.sf.path);  // TODO: virtual modules
        return builder.createFile(p.filename().string(), p.parent_path().string());
      }()},
      // TODO: provide vanadiumc ver
      unit{builder.createCompileUnit(llvm::dwarf::DW_LANG_C, file, "vanadiumc", false, "", 0)} {}

CodegenUnit::CodegenUnit(llvm::Module& mod_, const core::SourceFile& sf_, bool debug)
    : sf(sf_),
      ctx(mod_.getContext()),
      builder{ctx},
      mod{mod_},
      debug_info_(debug ? decltype(debug_info_){*this} : std::nullopt),
      rt{ctx, mod} {
  ConfigureTargetTriple(mod);
}

bool CodegenUnit::IsOpaque(TypeSymbol ts) const {
  return ts.is_template || !bool(ts->Flags() & core::semantic::SymbolFlags::kBuiltin);
}

llvm::Type* CodegenUnit::GetSymbolType(TypeSymbol ts) {
  assert(ts);

  // TODO: make builtin types templates non-opaque
  if (IsOpaque(ts)) {
    return builder.getPtrTy();
  }

  if (ts == &core::checker::symbols::kVoidType) {
    return builder.getVoidTy();
  }
  if (ts == &core::builtins::kInteger) {
    return rt.integer.ty;
  }
  if (ts == &core::builtins::kFloat) {
    return rt.floatt.ty;
  }
  if (ts == &core::builtins::kBoolean) {
    return rt.boolt.ty;
  }
  if (const auto* strb = GetStringTypeBindings(ts)) {
    return strb->ty;
  }

  VANADIUM_DEBUG_ERROR("GetSymbolType failed for symbol '{}'", ts->GetName());
}

llvm::Value* CodegenUnit::GetUndef(TypeSymbol ts) {
  if (IsOpaque(ts)) {
    return llvm::ConstantPointerNull::get(builder.getPtrTy());
  }

  if (ts == &core::builtins::kInteger) {
    return rt.integer.undef;
  }
  if (ts == &core::builtins::kFloat) {
    return rt.floatt.undef;
  }
  if (ts == &core::builtins::kBoolean) {
    return rt.boolt.undef;
  }
  if (const auto* strb = GetStringTypeBindings(ts)) {
    return strb->undef;
  }

  return nullptr;
}

const StringTypeBindings* CodegenUnit::GetStringTypeBindings(const core::semantic::Symbol* sym) {
  if (sym == &core::builtins::kCharstring) {
    return &rt.charstring;
  }
  if (sym == &core::builtins::kOctetstring) {
    return &rt.octetstring;
  }
  if (sym == &core::builtins::kBitstring) {
    return &rt.bitstring;
  }
  if (sym == &core::builtins::kHexstring) {
    return &rt.hexstring;
  }
  return nullptr;
}

llvm::Function* CodegenUnit::GetFunction(const core::semantic::Symbol* sym) {
  const auto& name = names::Func(sym);
  if (auto* fn = mod.getFunction(name)) {
    return fn;
  }

  const auto* m = sym->Declaration()->As<ast::nodes::FuncDecl>();
  const bool does_return = m->ret != nullptr;

  const bool is_variadic = [&] -> bool {
    if (!(sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
      return false;
    }
    if (m->params->list.size() != 1) {
      return false;
    }
    const auto* func_file = ast::utils::SourceFileOf(m);
    // it would be better to resolve the argument symbol and compare against &checker::symbols::kVarargsType,
    // but that's would be significantly slower
    return func_file->Text(*m->params->list.front()->type) == "__vargs_t";
  }();

  auto* ty = [&] {
    std::vector<llvm::Type*> params;
    if (is_variadic) {
      // TODO: variadic fns that return heavy types
      params = {
          builder.getPtrTy(),    // const vrt_val_t* args
          builder.getInt64Ty(),  // std::size_t n
      };
    } else {
      params.reserve(m->params->list.size() + (does_return ? 1 : 0));
      if (does_return) {
        params.push_back(builder.getPtrTy());
      }
      for (const auto* param : m->params->list) {
        // const auto& isym = core::checker::ResolveExprSymbol(&sf, sf.module->scope, param->type);
        // params.emplace_back(GetSymbolType(isym.sym));
        params.push_back(builder.getPtrTy());
      }
    }

    return llvm::FunctionType::get(builder.getVoidTy(), params, false);
  }();

  auto* fn = declareExternalFunc(name, ty);
  if (is_variadic) {
    fn->addFnAttr(kVarargsAttr);
  }

  const auto* sf = ast::utils::SourceFileOf(m);

  const auto bidx = does_return ? 1 : 0;
  for (const auto& [idx, param] : m->params->list | std::views::enumerate) {
    fn->addParamAttr(bidx + idx, llvm::Attribute::NoCapture);
    if (ast::utils::GetParamDirection(param) == ast::TokenKind::IN) {
      fn->addParamAttr(bidx + idx, llvm::Attribute::ReadOnly);
    }
    if (sf->Text(param->type) == "__infer_arg_t") {
      // it's faster than resolving the type through the entire checker pipeline
      fn->addParamAttr(bidx + idx, llvm::Attribute::get(ctx, kGenericArgAttr));
    }
  }

  return fn;
}

llvm::GlobalVariable* CodegenUnit::GetTypeInfo(TypeSymbol ts) {
  return getOrDeclareExternalConst(names::TInfo(ts), rt.typeinfo_ty);
}

bool CodegenUnit::IsTrivial(TypeSymbol ts) const {
  // TODO: unify w/ IsTrivial(llvm::Type*)
  assert(ts);
  return !ts.is_template && (ts.sym == &core::builtins::kBoolean || ts.sym == &core::builtins::kInteger ||
                             ts.sym == &core::builtins::kFloat);
}

bool CodegenUnit::IsTrivial(llvm::Type* ty) const {
  // int won't be trivial after bignum support
  return ty == rt.boolt.ty || ty == rt.integer.ty || ty == rt.floatt.ty;
}

llvm::Value* CodegenUnit::WrapValue(llvm::Value* v) {
  llvm::Function* wrap_f = nullptr;
  if (v->getType() == builder.getInt1Ty()) {
    if (auto* ci = llvm::dyn_cast<llvm::ConstantInt>(v)) {
      return rt.GetBool(ci->isOne());
    }
    wrap_f = rt.boolt.wrap_f;
  } else if (v->getType() == builder.getInt64Ty()) {
    if (auto* ci = llvm::dyn_cast<llvm::ConstantInt>(v)) {
      return rt.GetInt(ci->getSExtValue());
    }
    wrap_f = rt.integer.wrap_f;
  } else if (v->getType() == builder.getDoubleTy()) {
    if (auto* cfp = llvm::dyn_cast<llvm::ConstantFP>(v)) {
      return rt.GetFloat(cfp->getValueAPF().convertToDouble());
    }
    wrap_f = rt.floatt.wrap_f;
  }
  return wrap_f ? builder.CreateCall(wrap_f, {v}) : v;
}

llvm::Value* CodegenUnit::UnwrapValue(llvm::Value* v) {
  llvm::Function* get_f = nullptr;
  if (v->getType() == rt.boolt.ty) {
    get_f = rt.boolt.get_f;
  } else if (v->getType() == rt.integer.ty) {
    get_f = rt.integer.get_f;
  } else if (v->getType() == rt.floatt.ty) {
    get_f = rt.floatt.get_f;
  }
  return get_f ? builder.CreateCall(get_f, {v}) : v;
}

llvm::Value* CodegenUnit::UnwrapBoolOrBoxedBoolPtr(llvm::Value* v) {
  assert(v);
  if (v->getType()->isPointerTy()) {
    v = builder.CreateLoad(rt.boolt.ty, v);
  }
  return UnwrapValue(v);
}

void CodegenUnit::EmitDestructorInvocation(TypeSymbol ts, llvm::Value* val) {
  if (IsOpaque(ts)) {
    builder.CreateCall(rt.type_del_f, {
                                          mod.getGlobalVariable(names::TInfo(ts)),
                                          builder.CreateLoad(builder.getPtrTy(), val),
                                      });
  } else {
    auto* dtor_f = [&] -> llvm::Function* {
      if (const auto* strb = GetStringTypeBindings(ts)) {
        return strb->dtor_f;
      }
      return nullptr;
    }();
    if (dtor_f) {
      // TODO: partial inline (is_bound && is_ext) in RuntimeBindings
      builder.CreateCall(dtor_f, {val});
    }
  }
}

}  // namespace vanadium::compiler
