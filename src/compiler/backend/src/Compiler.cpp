#include "vanadium/compiler/Compiler.h"

#include <cassert>
#include <format>
#include <ranges>

#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Builtins.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>

#include "vanadium/compiler/Codegen.h"

namespace vanadium::compiler {

namespace {

class Codegen {
 public:
  Codegen(const core::SourceFile& sf) : ctx_(sf) {}

  void Gen() {
    CodegenTypes();
    CodegenFunctions();

    GenerateModuleRegistrationCode(ctx_);

    std::error_code ec;
    llvm::raw_fd_ostream dest(std::format("{}.ll", ctx_.sf.path), ec);
    ctx_.mod.print(dest, nullptr);
  }

 private:
  void CodegenTypes() {
    for (const auto& sym : ctx_.sf.module->scope->symbols.Enumerate() | std::views::values) {
      if (!(sym.Flags() & core::semantic::SymbolFlags::kType)) {
        continue;
      }
      CodegenType(ctx_, &sym);
    }
  }

  void CodegenFunctions() {
    for (const auto* def : ctx_.sf.ast.root->nodes.front()->As<ast::nodes::Module>()->defs) {
      const auto* n = def->def;
      switch (n->nkind) {
        case ast::NodeKind::FuncDecl:
          CodegenFunction(ctx_, n->As<ast::nodes::FuncDecl>());
          break;
        case ast::NodeKind::ControlPart: {
          CodegenFunction(ctx_, n->As<ast::nodes::ControlPart>());
          break;
        }
        default:
          break;
      }
    }
  }

  //

  CodegenContext ctx_;
};

}  // namespace

//

void CompileIR(const core::SourceFile& sf) {
  Codegen(sf).Gen();
}

}  // namespace vanadium::compiler
