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

void GenerateModuleRegistrationCode(CodegenContext& ctx) {
  auto* testcase_ty = llvm::StructType::create(ctx.llvm_ctx, "vrt_testcase_t");
  testcase_ty->setBody({
      ctx.builder.getPtrTy(),  // const char* name
      ctx.builder.getPtrTy(),  // void (*fn)()
      ctx.builder.getInt1Ty()  // bool parametrized
  });

  auto* module_ty = llvm::StructType::create(ctx.llvm_ctx, "vrt_module_t");
  module_ty->setBody({
      ctx.builder.getPtrTy(),   // const char* name
      ctx.builder.getPtrTy(),   // vrt_testcase_t**
      ctx.builder.getInt1Ty(),  // bool has_control
  });

  std::vector<llvm::Constant*> testcase_constants;

  ForEachTestcase(ctx.sf, [&](const ast::nodes::FuncDecl* f) {
    const auto* sym = core::checker::ResolveExprSymbol(&ctx.sf, ctx.sf.module->scope, &(*f->name)).sym;
    testcase_constants.emplace_back(llvm::ConstantExpr::getBitCast(
        new llvm::GlobalVariable(
            ctx.mod, testcase_ty, true, llvm::GlobalValue::PrivateLinkage,
            llvm::ConstantStruct::get(testcase_ty,
                                      {
                                          ctx.builder.CreateGlobalStringPtr(ctx.sf.Text(*f->name)),
                                          llvm::ConstantExpr::getBitCast(ctx.GetFunction(sym), ctx.builder.getPtrTy()),
                                          ctx.builder.getInt1(!f->params->list.empty()),
                                      })),
        ctx.builder.getPtrTy()));
  });
  testcase_constants.emplace_back(llvm::ConstantPointerNull::get(ctx.builder.getPtrTy()));

  auto* testcase_array = llvm::ConstantArray::get(
      llvm::ArrayType::get(ctx.builder.getPtrTy(), testcase_constants.size()), testcase_constants);

  auto* g_module = new llvm::GlobalVariable(
      ctx.mod, module_ty, true, llvm::GlobalValue::InternalLinkage,
      llvm::ConstantStruct::get(
          module_ty,
          {
              ctx.builder.CreateGlobalStringPtr(ctx.sf.module->name),
              llvm::ConstantExpr::getBitCast(
                  new llvm::GlobalVariable(ctx.mod, testcase_array->getType(), true, llvm::GlobalValue::PrivateLinkage,
                                           testcase_array, "testcases"),
                  ctx.builder.getPtrTy()),
              llvm::ConstantInt::get(ctx.builder.getInt1Ty(), 1),
          }),
      "vrt_module");

  auto* init_ty = llvm::FunctionType::get(ctx.builder.getVoidTy(), false);
  auto* init_fn = llvm::Function::Create(init_ty, llvm::GlobalValue::InternalLinkage, "__vrt_module_init", ctx.mod);
  auto* init_bb = llvm::BasicBlock::Create(ctx.llvm_ctx, "entry", init_fn);
  llvm::IRBuilder<> init_builder(init_bb);
  init_builder.CreateCall(
      llvm::Function::Create(llvm::FunctionType::get(ctx.builder.getVoidTy(), {ctx.builder.getPtrTy()}, false),
                             llvm::GlobalValue::ExternalLinkage, "vrt_register_module", ctx.mod),
      {g_module});
  init_builder.CreateRetVoid();

  llvm::appendToGlobalCtors(ctx.mod, init_fn, 65535);
}

}  // namespace vanadium::compiler
