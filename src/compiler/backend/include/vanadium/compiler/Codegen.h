#pragma once

#include <optional>
#include <string_view>
#include <variant>

#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>

#include "vanadium/compiler/RuntimeBindings.h"
#include "vanadium/core/Semantic.h"
#include "vanadium/lib/Metaprogramming.h"

namespace vanadium::compiler {

struct CodegenUnit;

struct DebugInfo {
  llvm::DIBuilder builder;
  llvm::DIFile* file;
  llvm::DICompileUnit* unit;

  DebugInfo(CodegenUnit&);
};

struct CodegenUnit {
  CodegenUnit(const core::SourceFile& sf_, bool debug);

  const core::SourceFile& sf;

  llvm::LLVMContext ctx;
  llvm::IRBuilder<> builder;
  llvm::Module mod;

 private:
  std::optional<DebugInfo> debug_info_;

 public:
  const RuntimeBindings rt;

  //

  bool DebugInfoEnabled() const noexcept {
    return debug_info_.has_value();
  }
  void EmitDebugInfo(mp::Consumer<DebugInfo&> auto f) {
    if (debug_info_) {
      f(*debug_info_);
    }
  }

  //

  llvm::Function* declareExternalFunc(std::string_view name, llvm::FunctionType* ty) {
    return llvm::Function::Create(ty, llvm::Function::ExternalLinkage, name, mod);
  }
  llvm::Function* getOrDeclareExternalFunc(std::string_view name, llvm::FunctionType* ty) {
    if (auto* fn = mod.getFunction(name); fn) {
      return fn;
    }
    return declareExternalFunc(name, ty);
  }

  llvm::GlobalVariable* declareExternalConst(std::string_view name, llvm::Type* ty) {
    return new llvm::GlobalVariable(mod, ty, true, llvm::GlobalValue::ExternalLinkage, nullptr, name);
  }
  llvm::GlobalVariable* getOrDeclareExternalConst(std::string_view name, llvm::Type* ty) {
    if (auto* gv = mod.getNamedGlobal(name)) {
      return gv;
    }
    return declareExternalConst(name, ty);
  }

  //

  llvm::Type* GetSymbolType(const core::semantic::Symbol*);
  llvm::Value* GetUndef(const core::semantic::Symbol*);

  //

  llvm::Function* GetFunction(const core::semantic::Symbol*);

  //

  std::variant<RuntimeBindings::NativeIntType, std::string_view> ParseInt(const ast::nodes::ValueLiteral*);
  std::string_view ParseCharstring(const ast::nodes::ValueLiteral*);
  std::string ParseOctetstring(const ast::nodes::ValueLiteral*);
};

inline constexpr std::string_view kVarargsAttr = "vrt-varargs";

namespace names {
std::string Func(const core::semantic::Symbol*);
std::string Ctor(const core::semantic::Symbol*);
std::string Dtor(const core::semantic::Symbol*);
std::string CopyCtor(const core::semantic::Symbol*);
std::string Getter(const core::semantic::Symbol* holder, std::string_view member);
std::string TInfo(const core::semantic::Symbol*);
}  // namespace names

namespace values {}

void CodegenType(CodegenUnit&, const core::semantic::Symbol*);
void CodegenFunction(CodegenUnit&, const core::semantic::Symbol*);

void GenerateModuleRegistrationCode(CodegenUnit&);

}  // namespace vanadium::compiler
