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
      rt{ctx, mod} {}

bool CodegenUnit::IsOpaque(TypeSymbol ts) const {
  return !bool(ts->Flags() & core::semantic::SymbolFlags::kBuiltin) || ts.is_template;
}

llvm::Type* CodegenUnit::GetSymbolType(TypeSymbol ts) {
  // TODO: make builtin types templates non-opaque
  if (IsOpaque(ts)) {
    return builder.getPtrTy();
  }

  if (ts == &core::builtins::kInteger) {
    return rt.integer.ty;
  }
  if (ts == &core::builtins::kFloat) {
    return rt.floatt.ty;
  }
  if (ts == &core::checker::symbols::kVoidType) {
    return builder.getVoidTy();
  }
  if (const auto* strb = GetStringTypeBindings(ts)) {
    return strb->ty;
  }

  assert(false);
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
  // for (const auto& [idx, param] : m->params->list | std::views::enumerate) {
  //   if (!param->direction || param->direction->kind == ast::TokenKind::IN) {
  //     fn->addParamAttr(idx, llvm::Attribute::NoCapture);
  //     fn->addParamAttr(idx, llvm::Attribute::ReadOnly);
  //   }
  // }

  return fn;
}

llvm::GlobalVariable* CodegenUnit::GetTypeInfo(TypeSymbol ts) {
  return getOrDeclareExternalConst(names::TInfo(ts), rt.typeinfo_ty);
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

}  // namespace vanadium::compiler
