#include <string_view>

#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Transforms/Utils/ModuleUtils.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/lib/Metaprogramming.h>

#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler {

namespace {
void ForEachTestcase(const core::SourceFile& sf, mp::Consumer<const ast::nodes::FuncDecl*> auto accept) {
  for (const auto* def : sf.ast.root->nodes[0]->As<ast::nodes::Module>()->defs) {
    if (def->def->nkind != ast::NodeKind::FuncDecl) {
      continue;
    }
    const auto* f = def->def->As<ast::nodes::FuncDecl>();
    if (f->kind.kind == ast::TokenKind::TESTCASE) {
      accept(f);
    }
  }
}
}  // namespace

void GenerateModuleRegistrationCode(CodegenUnit& u) {
  auto* testcase_ty = llvm::StructType::create(u.ctx, "vrt_testcase_t");
  testcase_ty->setBody({
      u.builder.getPtrTy(),  // const char* name
      u.builder.getPtrTy(),  // void (*fn)()
      u.builder.getInt1Ty()  // bool parametrized
  });

  auto* module_ty = llvm::StructType::create(u.ctx, "vrt_module_t");
  module_ty->setBody({
      u.builder.getPtrTy(),   // const char* name
      u.builder.getPtrTy(),   // vrt_testcase_t**
      u.builder.getInt1Ty(),  // bool has_control
  });

  std::vector<llvm::Constant*> testcase_constants;

  ForEachTestcase(u.sf, [&](const ast::nodes::FuncDecl* f) {
    const auto* sym = core::checker::ResolveExprSymbol(&u.sf, u.sf.module->scope, &(*f->name)).sym;
    testcase_constants.emplace_back(llvm::ConstantExpr::getBitCast(
        new llvm::GlobalVariable(
            u.mod, testcase_ty, true, llvm::GlobalValue::PrivateLinkage,
            llvm::ConstantStruct::get(testcase_ty,
                                      {
                                          u.builder.CreateGlobalStringPtr(u.sf.Text(*f->name)),
                                          llvm::ConstantExpr::getBitCast(u.GetFunction(sym), u.builder.getPtrTy()),
                                          u.builder.getInt1(!f->params->list.empty()),
                                      })),
        u.builder.getPtrTy()));
  });
  testcase_constants.emplace_back(llvm::ConstantPointerNull::get(u.builder.getPtrTy()));

  auto* testcase_array = llvm::ConstantArray::get(llvm::ArrayType::get(u.builder.getPtrTy(), testcase_constants.size()),
                                                  testcase_constants);

  auto* g_module = new llvm::GlobalVariable(
      u.mod, module_ty, true, llvm::GlobalValue::InternalLinkage,
      llvm::ConstantStruct::get(
          module_ty,
          {
              u.builder.CreateGlobalStringPtr(u.sf.module->name),
              llvm::ConstantExpr::getBitCast(
                  new llvm::GlobalVariable(u.mod, testcase_array->getType(), true, llvm::GlobalValue::PrivateLinkage,
                                           testcase_array, "__testcases"),
                  u.builder.getPtrTy()),
              llvm::ConstantInt::get(u.builder.getInt1Ty(), 1),
          }),
      "__vrt_module");

  auto* init_ty = llvm::FunctionType::get(u.builder.getVoidTy(), false);
  auto* init_fn = llvm::Function::Create(init_ty, llvm::GlobalValue::InternalLinkage, "__vrt_module_init", u.mod);
  auto* init_bb = llvm::BasicBlock::Create(u.ctx, "entry", init_fn);
  llvm::IRBuilder<> init_builder(init_bb);
  init_builder.CreateCall(
      llvm::Function::Create(llvm::FunctionType::get(u.builder.getVoidTy(), {u.builder.getPtrTy()}, false),
                             llvm::GlobalValue::ExternalLinkage, "vrt_register_module", u.mod),
      {g_module});
  init_builder.CreateRetVoid();

  llvm::appendToGlobalCtors(u.mod, init_fn, 65535);
}

}  // namespace vanadium::compiler
