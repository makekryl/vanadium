#include <filesystem>

#include <llvm/IR/DIBuilder.h>
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

CodegenUnit::CodegenUnit(const core::SourceFile& sf_, bool debug)
    : sf(sf_),
      builder{ctx},
      mod{sf.module->name, ctx},
      debug_info_(debug ? decltype(debug_info_){*this} : std::nullopt),
      rt{ctx, mod} {}

llvm::Type* CodegenUnit::GetSymbolType(const core::semantic::Symbol* sym) {
  assert(sym->Flags() & core::semantic::SymbolFlags::kType);
  if (sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    if (sym == &core::builtins::kInteger) {
      return rt.integer.ty;
    }
    if (sym == &core::builtins::kFloat) {
      return rt.floatt.ty;
    }
    if (sym == &core::builtins::kCharstring) {
      return rt.charstring_ty;
    }
    if (sym == &core::builtins::kOctetstring) {
      return rt.octetstring.ty;
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
    return rt.integer.undef;
  }
  if (sym == &core::builtins::kFloat) {
    return rt.floatt.undef;
  }
  if (sym == &core::builtins::kCharstring) {
    return rt.charstring_undef;
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

std::variant<RuntimeBindings::NativeIntType, std::string_view> CodegenUnit::ParseInt(
    const ast::nodes::ValueLiteral* m) {
  assert(m->tok.kind == ast::TokenKind::INT);

  const auto& s = sf.Text(m);

  std::int64_t result;
  std::from_chars(s.data(), s.data() + s.size(), result);

  // TODO: return s on out of range
  return result;
}

double CodegenUnit::ParseFloat(const ast::nodes::ValueLiteral* m) {
  assert(m->tok.kind == ast::TokenKind::FLOAT);

  const auto& s = sf.Text(m);

  double result;
  std::from_chars(s.data(), s.data() + s.size(), result);
  return result;
}

std::string_view CodegenUnit::ParseCharstring(const ast::nodes::ValueLiteral* m) {
  assert(m->tok.kind == ast::TokenKind::STRING);

  auto s = sf.Text(m);
  s.remove_prefix(1);  // "
  s.remove_suffix(1);  // "
  return s;
}

std::string CodegenUnit::ParseOctetstring(const ast::nodes::ValueLiteral* m) {
  assert(m->tok.kind == ast::TokenKind::OCTETSTRING);

  auto s = sf.Text(m);
  s.remove_prefix(1);  // '
  s.remove_suffix(2);  // 'O

  // TODO: it will be needed in runtime, extract, cover with tests
  constexpr auto kChar2Hex = [](char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    assert(false);
  };

  // TODO(lexer): verify length
  assert(s.length() % 2 == 0);

  std::string result;
  result.reserve(s.length() / 2);
  //
  for (std::size_t i = 0; i < s.length(); i += 2) {
    const std::uint8_t high = kChar2Hex(s[i]);
    const std::uint8_t low = kChar2Hex(s[i + 1]);
    result.push_back((high << 4) | low);
  }

  return result;
}

}  // namespace vanadium::compiler
