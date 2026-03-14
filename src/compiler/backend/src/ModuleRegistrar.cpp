#include "vanadium/compiler/ModuleRegistrar.h"

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
#include <vanadium/lib/Metaprogramming.h>

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

void GenerateModuleRegistrationCode(const core::SourceFile& sf, llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder,
                                    llvm::Module& mod) {
  auto* testcase_ty = llvm::StructType::create(ctx, "vrt_testcase_t");
  testcase_ty->setBody({
      builder.getPtrTy(),  // const char* name
      builder.getPtrTy(),  // void (*fn)()
      builder.getInt1Ty()  // bool parametrized
  });

  auto* module_ty = llvm::StructType::create(ctx, "vrt_module_t");
  module_ty->setBody({
      builder.getPtrTy(),   // const char* name
      builder.getPtrTy(),   // vrt_testcase_t**
      builder.getInt1Ty(),  // bool has_control
  });

  std::vector<llvm::Constant*> testcase_constants;

  ForEachTestcase(sf, [&](const ast::nodes::FuncDecl* f) {
    testcase_constants.emplace_back(llvm::ConstantExpr::getBitCast(
        new llvm::GlobalVariable(
            mod, testcase_ty, true, llvm::GlobalValue::PrivateLinkage,
            llvm::ConstantStruct::get(
                testcase_ty,
                {
                    builder.CreateGlobalStringPtr(sf.Text(*f->name)),
                    llvm::ConstantExpr::getBitCast(mod.getFunction(sf.Text(*f->name)), builder.getPtrTy()),
                    builder.getInt1(!f->params->list.empty()),
                })),
        builder.getPtrTy()));
  });
  testcase_constants.emplace_back(llvm::ConstantPointerNull::get(builder.getPtrTy()));

  auto* testcase_array =
      llvm::ConstantArray::get(llvm::ArrayType::get(builder.getPtrTy(), testcase_constants.size()), testcase_constants);

  auto* g_module = new llvm::GlobalVariable(
      mod, module_ty, true, llvm::GlobalValue::InternalLinkage,
      llvm::ConstantStruct::get(
          module_ty,
          {
              builder.CreateGlobalStringPtr(sf.module->name),
              llvm::ConstantExpr::getBitCast(
                  new llvm::GlobalVariable(mod, testcase_array->getType(), true, llvm::GlobalValue::PrivateLinkage,
                                           testcase_array, "testcases"),
                  builder.getPtrTy()),
              llvm::ConstantInt::get(builder.getInt1Ty(), 1),
          }),
      "vrt_module");

  auto* init_ty = llvm::FunctionType::get(builder.getVoidTy(), false);
  auto* init_fn = llvm::Function::Create(init_ty, llvm::GlobalValue::InternalLinkage, "__vrt_module_init", mod);
  auto* init_bb = llvm::BasicBlock::Create(ctx, "entry", init_fn);
  llvm::IRBuilder<> init_builder(init_bb);
  init_builder.CreateCall(
      llvm::Function::Create(llvm::FunctionType::get(builder.getVoidTy(), {builder.getPtrTy()}, false),
                             llvm::GlobalValue::ExternalLinkage, "vrt_register_module", mod),
      {g_module});
  init_builder.CreateRetVoid();

  llvm::appendToGlobalCtors(mod, init_fn, 65535);
}

}  // namespace vanadium::compiler
