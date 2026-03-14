#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <vanadium/core/TypeChecker.h>

#include "vanadium/ast/ASTNodes.h"
#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler {

[[nodiscard]] llvm::Type* CodegenContext::GetSymbolType(const core::semantic::Symbol* sym) {
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

[[nodiscard]] llvm::Value* CodegenContext::GetUndef(const core::semantic::Symbol* sym) {
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

std::variant<RuntimeBindings::NativeIntType, std::string_view> CodegenContext::ParseInt(
    const ast::nodes::ValueLiteral* m) {
  const auto& s = sf.Text(m);

  std::int64_t result;
  std::from_chars(s.data(), s.data() + s.size(), result);

  // TODO: return s on out of range
  return result;
}

std::string_view CodegenContext::ParseCharstring(const ast::nodes::ValueLiteral* m) {
  auto s = sf.Text(m);
  s.remove_prefix(1);
  s.remove_suffix(1);
  return s;
}

}  // namespace vanadium::compiler
