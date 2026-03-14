#pragma once

#include <string_view>
#include <variant>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>

#include "vanadium/compiler/RuntimeBindings.h"
#include "vanadium/core/Semantic.h"

namespace vanadium::compiler {

struct CodegenContext {
  CodegenContext(const core::SourceFile& sf_) : sf(sf_) {}

  const core::SourceFile& sf;

  llvm::LLVMContext llvm_ctx;
  llvm::IRBuilder<> builder{llvm_ctx};
  llvm::Module mod{sf.module->name, llvm_ctx};

  const RuntimeBindings rt{llvm_ctx, mod};

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

  std::variant<RuntimeBindings::NativeIntType, std::string_view> ParseInt(const ast::nodes::ValueLiteral*);
  std::string_view ParseCharstring(const ast::nodes::ValueLiteral*);
};

namespace names {
std::string Ctor(const core::semantic::Symbol*);
std::string Dtor(const core::semantic::Symbol*);
std::string Getter(const core::semantic::Symbol* holder, std::string_view member);
std::string TInfo(const core::semantic::Symbol*);
}  // namespace names

namespace values {}

void CodegenType(CodegenContext&, const core::semantic::Symbol*);
void CodegenFunction(CodegenContext&, const ast::nodes::FuncDecl*);
void CodegenFunction(CodegenContext&, const ast::nodes::ControlPart*);

void GenerateModuleRegistrationCode(CodegenContext&);

}  // namespace vanadium::compiler
