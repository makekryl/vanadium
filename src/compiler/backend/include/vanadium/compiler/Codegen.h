#pragma once

#include <cassert>
#include <optional>
#include <string_view>

#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/lib/Metaprogramming.h>

#include "vanadium/compiler/RuntimeBindings.h"
#include "vanadium/compiler/TypeSymbol.h"

namespace vanadium::compiler {

struct CodegenUnit;

struct DebugInfo {
  llvm::DIBuilder builder;
  llvm::DIFile* file;
  llvm::DICompileUnit* unit;

  DebugInfo(CodegenUnit&);
};

struct CodegenUnit {
  CodegenUnit(llvm::Module&, const core::SourceFile&, bool debug);

  const core::SourceFile& sf;

  llvm::LLVMContext& ctx;
  llvm::IRBuilder<> builder;
  llvm::Module& mod;

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

  bool IsOpaque(TypeSymbol) const;
  llvm::Type* GetSymbolType(TypeSymbol);
  llvm::Value* GetUndef(TypeSymbol);

  const StringTypeBindings* GetStringTypeBindings(const core::semantic::Symbol*);

  //

  llvm::Function* GetFunction(const core::semantic::Symbol*);
  llvm::GlobalVariable* GetTypeInfo(TypeSymbol);

  //

  bool IsTrivial(llvm::Type*) const;

  //

  llvm::Value* WrapValue(llvm::Value*);    // primitive -> primitive wrapper (integer, float, bool)
  llvm::Value* UnwrapValue(llvm::Value*);  // primitive wrapper -> primitive

  llvm::Value* UnwrapBoolOrBoxedBoolPtr(llvm::Value*);

  //

  void EmitDestructorInvocation(TypeSymbol ts, llvm::Value* val);
};

inline constexpr std::string_view kVarargsAttr = "vrt-varargs";
inline constexpr std::string_view kGenericArgAttr = "vrt-garg";

namespace names {
std::string Func(const core::semantic::Symbol*);
std::string Type(TypeSymbol);
std::string Ctor(TypeSymbol);
std::string Dtor(TypeSymbol);
std::string CopyCtor(TypeSymbol);
std::string TInfo(TypeSymbol);
std::string Getter(TypeSymbol holder, std::string_view member);
std::string Muttor(TypeSymbol holder, std::string_view member);
std::string TplValCtor(TypeSymbol);
}  // namespace names

namespace values {}

void CodegenType(CodegenUnit&, const core::semantic::Symbol*);
void CodegenFunction(CodegenUnit&, const core::semantic::Symbol*);

void GenerateModuleRegistrationCode(CodegenUnit&);

}  // namespace vanadium::compiler
