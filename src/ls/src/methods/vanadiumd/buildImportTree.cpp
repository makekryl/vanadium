#include <cstddef>
#include <optional>
#include <string_view>
#include <unordered_set>

#include <LSProtocol.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/core/utils/ImportVisitor.h>
#include <vanadium/core/utils/SemanticUtils.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/LanguageServerSession.h"

namespace vanadium::ls {
namespace {
/**
 * @param f(imported_module, via)
 */
bool VisitAllImports(core::Program* program, const core::ModuleDescriptor& module, auto f,
                     core::ModuleDescriptor* via = nullptr) {
  for (const auto& [import, descriptor] : module.imports) {
    auto* imported_module = program->GetModule(import);
    if (!imported_module) {
      continue;
    }

    if (!VisitAllImports(program, *imported_module, f, via ? via : imported_module)) {
      return false;
    }
  }

  return true;
}

std::string BuildImportTree(const lsp::TextDocumentIdentifierContainer&, const core::SourceFile& file, LsSessionRef) {
  std::string buf;

  std::size_t depth{1};
  const auto write_leftpad = [&] {
    for (std::size_t i = 0; i < depth; ++i) {
      buf += "| ";
    }
  };
  // std::unordered_set<std::string_view> seen;

  [&](this auto&& self, const core::ModuleDescriptor& module) -> void {
    for (const auto& [import, descriptor] : module.imports) {
      write_leftpad();
      buf += "- ";
      if (descriptor.transit) {
        buf += "[+] ";
      }
      if (descriptor.is_public) {
        buf += "[P] ";
      }
      buf += import;
      buf += "\n";

      // auto [_, inserted] = seen.insert(import);
      // if (!inserted) {
      //   return;
      // }

      ++depth;
      auto* imported_module = module.sf->program->GetModule(import);
      if (imported_module) {
        self(*imported_module);
      } else {
        write_leftpad();
        buf += "<NOT FOUND>\n";
      }
      --depth;
    }
  }(*file.module);
  VisitAllImports(const_cast<core::Program*>(file.program), *file.module,
                  [&](const auto& imported_module, [[maybe_unused]] const auto* via) {
                    if (via) {
                      buf += via->name;
                    }
                    buf += " -> ";
                    buf += imported_module;
                    buf += "\n";
                    return true;
                  });

  return buf;
}
}  // namespace

rpc::ExpectedResult<std::string> methods::vanadiumd::buildImportTree::invoke(
    LsContext& ctx, const lsp::TextDocumentIdentifierContainer& params) {
  return ctx.WithFile<std::string>(params, BuildImportTree).value_or("Failed to build import tree");
}
}  // namespace vanadium::ls
