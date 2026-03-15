#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>

#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler {

llvm::Type* CodegenUnit::GetSymbolType(const core::semantic::Symbol* sym) {
  assert(sym->Flags() & core::semantic::SymbolFlags::kType);
  if (sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    if (sym == &core::builtins::kInteger) {
      return rt.int_ty;
    }
    if (sym == &core::builtins::kCharstring) {
      return rt.charstring_ty;
    }
    if (sym == &core::checker::symbols::kVoidType) {
      return builder.getVoidTy();
    }
    assert(false);
  }
  return builder.getPtrTy();
}

llvm::Value* CodegenUnit::GetUndef(const core::semantic::Symbol* sym) {
  assert(sym->Flags() & core::semantic::SymbolFlags::kType);
  if (!(sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
    return llvm::ConstantPointerNull::get(builder.getPtrTy());
  }

  if (sym == &core::builtins::kInteger) {
    return rt.int_undef;
  }
  if (sym == &core::builtins::kCharstring) {
    return rt.charstring_undef;
  }

  return nullptr;
}

llvm::Function* CodegenUnit::GetFunction(const core::semantic::Symbol* sym) {
  const auto* m = sym->Declaration()->As<ast::nodes::FuncDecl>();

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
      params = {
          builder.getPtrTy(),    // const vrt_val_t* args
          builder.getInt64Ty(),  // std::size_t n
      };
    } else {
      params.reserve(m->params->list.size());
      for (const auto* param : m->params->list) {
        const auto& isym = core::checker::ResolveExprSymbol(&sf, sf.module->scope, param->type);
        params.emplace_back(GetSymbolType(isym.sym));
      }
    }

    const auto& ret_sym = core::checker::ResolveCallableReturnType(&sf, m);
    auto* ret_ty = GetSymbolType(ret_sym.sym);

    return llvm::FunctionType::get(ret_ty, params, false);
  }();

  const auto& name = names::Func(sym);

  if (auto* fn = mod.getFunction(name)) {
    return fn;
  }

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

std::variant<RuntimeBindings::NativeIntType, std::string_view> CodegenUnit::ParseInt(
    const ast::nodes::ValueLiteral* m) {
  const auto& s = sf.Text(m);

  std::int64_t result;
  std::from_chars(s.data(), s.data() + s.size(), result);

  // TODO: return s on out of range
  return result;
}

std::string_view CodegenUnit::ParseCharstring(const ast::nodes::ValueLiteral* m) {
  auto s = sf.Text(m);
  s.remove_prefix(1);
  s.remove_suffix(1);
  return s;
}

}  // namespace vanadium::compiler
