#include "vanadium/ls/detail/References.h"

#include <algorithm>
#include <memory>
#include <unordered_set>

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>

#include "vanadium/ls/LanguageServerLogger.h"
#include "vanadium/ls/detail/Definition.h"

namespace vanadium::ls::detail {

namespace {
const ast::nodes::Ident* GetDeclarationIdent(const core::semantic::Symbol* sym) {
  const auto* decl = sym->Declaration();
  const auto* decl_name_ident = [&] -> const ast::Node* {
    switch (decl->nkind) {
      case ast::NodeKind::Declarator:
        return std::addressof(*decl->As<ast::nodes::Declarator>()->name);
      case ast::NodeKind::FormalPar:
        return std::addressof(*decl->As<ast::nodes::FormalPar>()->name);
      default:
        return decl;
    }
  }();
  if (decl_name_ident->nkind == ast::NodeKind::Ident) {
    return decl_name_ident->As<ast::nodes::Ident>();
  }
  return nullptr;
}

// for the locally bound symbols, i.e. a dumb by-name comparison is sufficient
void VisitLocalReferences(const core::SourceFile* file, const core::semantic::Scope* scope, std::string_view name,
                          lib::Consumer<const ast::Node*> accept) {
  scope->Container()->Accept([&](this auto&& self, const ast::Node* n) {
    if (n->nkind == ast::NodeKind::AssignmentExpr) {
      const auto* ae = n->As<ast::nodes::AssignmentExpr>();
      if (ae->parent->nkind == ast::NodeKind::CompositeLiteral || ae->parent->nkind == ast::NodeKind::ParenExpr) {
        ast::Inspect(ae->value, self);
        return false;
      }
      return true;
    }

    if (n->nkind == ast::NodeKind::SelectorExpr) {
      n = ast::utils::TraverseSelectorExpressionStart(n->As<ast::nodes::SelectorExpr>());
    }
    if (n->nkind != ast::NodeKind::Ident) {
      return true;
    }

    if (/*n != decl &&*/ file->Text(n) == name) {
      accept(n);
    }

    return true;
  });
}

void VisitTypeReferencesInFile(const core::semantic::Symbol* type_sym, const core::SourceFile* sf,
                               lib::Consumer<const ast::Node*> accept) {
  const std::string_view type_name = type_sym->GetName();
  const bool is_anon = bool(type_sym->Flags() & core::semantic::SymbolFlags::kAnonymous);

  const auto try_consume = [&](const ast::nodes::Expr* expr) {
    switch (expr->nkind) {
      case ast::NodeKind::Ident: {
        // for anon types dumb name comparing is useless
        if (!is_anon) {
          if (type_name == sf->Text(expr)) {
            accept(expr);
          }
        }
        break;
      }
      case ast::NodeKind::SelectorExpr: {
        if (core::checker::ResolveExprSymbol(sf, sf->module->scope, expr).sym == type_sym) {
          accept(expr);
        }
        break;
      }
      default:  // ???
        break;
    }
  };
  sf->ast.root->Accept([&](const ast::Node* n) {
    switch (n->nkind) {
      case ast::NodeKind::Field: {
        const auto* m = n->As<ast::nodes::Field>();
        if (const auto* t = m->type->Maybe<ast::nodes::RefSpec>()) {
          try_consume(t->x);
        }
        return false;
      }
      case ast::NodeKind::FormalPar: {
        const auto* m = n->As<ast::nodes::FormalPar>();
        try_consume(m->type);
        return false;
      }
      case ast::NodeKind::ValueDecl: {
        const auto* m = n->As<ast::nodes::ValueDecl>();
        try_consume(m->type);
        return false;
      }
      case ast::NodeKind::BinaryExpr: {
        const auto* m = n->As<ast::nodes::BinaryExpr>();
        if (m->op.kind == ast::TokenKind::COLON) {
          // TODO: distinct node?
          try_consume(m->x);
        }
        return true;
      }
      default: {
        return true;
      }
    }
    return true;
  });
}

void VisitComponentAugmentedScopesInFile(const core::semantic::Symbol* comp_sym, const core::SourceFile* sf,
                                         lib::Consumer<const core::semantic::Scope*> accept) {
  for (const auto* scope : sf->module->scope->GetChildren()) {
    if (std::ranges::contains(scope->augmentation, &comp_sym->OriginatedScope()->symbols)) {
      accept(scope);
    }
  }
}

void VisitFunctionReferencesInFile(const core::semantic::Symbol* func_sym, const core::SourceFile* sf,
                                   lib::Consumer<const ast::Node*> accept) {
  const std::string_view func_name = func_sym->GetName();

  sf->ast.root->Accept([&](const ast::Node* n) {
    switch (n->nkind) {
      case ast::NodeKind::CallExpr: {
        const auto* m = n->As<ast::nodes::CallExpr>();
        if (const auto* se = m->fun->Maybe<ast::nodes::SelectorExpr>()) {
          if (core::checker::ResolveExprSymbol(sf, sf->module->scope, se).sym == func_sym) {
            accept(se);
          }
        } else if (m->fun->nkind == ast::NodeKind::Ident && sf->Text(m->fun) == func_name) {
          accept(m->fun);
        }
        return true;
      }
      case ast::NodeKind::StructTypeDecl:
      case ast::NodeKind::EnumTypeDecl:
      case ast::NodeKind::PortTypeDecl:
      case ast::NodeKind::ComponentTypeDecl:
      case ast::NodeKind::SelectorExpr:
        return false;
      default:
        return true;
    }
  });
}

// global_search flag here is kinda stupid given the function name, also bool flags are crap. <-- TODO!
void PerformProjectWideSearch(const core::semantic::Symbol* sym, const auto& search, bool global,
                              const auto& make_consumer) {
  const auto* sf = ast::utils::SourceFileOf(sym->Declaration());
  search(sym, sf, make_consumer(sf));
  if (global) {
    // btw maybe it's cheaper to track fully analyzed files rather than the lazily analyzed ones
    std::vector<core::SourceFile*> analyzed_files;
    {
      // NOTE: we can't just use sf->module->dependents there because for the lazily analyzed modules actual
      //       dependencies are not known yet
      std::unordered_set<core::Program*> affected_programs;
      sf->program->VisitOwnAndDependentModules([&](const core::ModuleDescriptor& mod) {
        // TODO: check the similar hack in didOpen, try to get rid of it / wrap in something
        if (mod.sf->skip_analysis) {
          analyzed_files.emplace_back(const_cast<core::SourceFile*>(mod.sf))->skip_analysis = false;
          affected_programs.emplace(mod.sf->program);
        }
        return true;
      });
      VLS_INFO("Running full analysis on previously shallowly checked {} files in {} programs", analyzed_files.size(),
               affected_programs.size());
      for (auto* program : affected_programs) {
        program->Commit([](auto&) {});
      }
    }
    for (const auto* mod : sf->module->dependents) {
      search(sym, mod->sf, make_consumer(mod->sf));
    }
    for (auto* asf : analyzed_files) {
      asf->skip_analysis = true;
    }
  }
}
}  // namespace

void VisitReferences(const core::SourceFile* file, SymbolSearchResult symres, ReferencesVisitParams params,
                     lib::Consumer<const core::SourceFile*, const ast::Node*> accept) {
  if (symres.node->nkind != ast::NodeKind::Ident) {
    // TODO: use the name ident inside StructTypeDecl, ...
    return;
  }

  if (params.include_declaration) {
    if (const auto* decl_ident = GetDeclarationIdent(symres.type.sym)) {
      accept(ast::utils::SourceFileOf(decl_ident), decl_ident);
    }
  }

  const auto accept_from_file = [&accept](const core::SourceFile* sf) {
    return [&accept, sf](const ast::Node* m) {
      accept(sf, m);
    };
  };

  const auto name = symres.type->GetName();

  if (!symres.scope) {
    VLS_WARN("Scope not found");
    return;
  }

  if (symres.type->Flags() & core::semantic::SymbolFlags::kArgument) {
    VisitLocalReferences(file, symres.scope, name, accept_from_file(file));
    return;
  }
  if (symres.type->Flags() & core::semantic::SymbolFlags::kVariable) {
    if (ast::utils::GetPredecessor<ast::nodes::FuncDecl>(symres.node)) {
      VisitLocalReferences(file, symres.scope, name, accept_from_file(file));
      return;
    }
    // todo: ControlPart, ...
    return;
  }
  if (symres.type->Flags() & core::semantic::SymbolFlags::kField) {
    // things are starting to become not so easy...
    if (const auto* comp_node = ast::utils::GetPredecessor<ast::nodes::ComponentTypeDecl>(symres.type->Declaration())) {
      const auto* comp_file = ast::utils::SourceFileOf(symres.type->Declaration());
      const auto comp_name = comp_file->Text(*comp_node->name);
      const auto* comp_sym = comp_file->module->scope->ResolveDirect(comp_name);
      if (!comp_sym) {
        VLS_WARN("Failed to find symbol '{}' in file '{}'", comp_name, comp_file->path);
        return;
      }
      if (!(comp_sym->Flags() & core::semantic::SymbolFlags::kComponent)) {
        VLS_WARN("Symbol '{}' is not a component", comp_sym->GetName());
        return;
      }
      // todo: dive into classes (their methods can have "runs on")
      PerformProjectWideSearch(comp_sym, VisitComponentAugmentedScopesInFile, params.global_search,
                               [&](const auto* sf) {
                                 return [&, sf](const core::semantic::Scope* augmented_scope) {
                                   VisitLocalReferences(sf, augmented_scope, name, accept_from_file(sf));
                                 };
                               });
      return;
    }

    if ([[maybe_unused]] const auto* class_node = ast::utils::GetPredecessor<ast::nodes::ClassTypeDecl>(symres.node)) {
      VLS_WARN("The symbol is a class member, not supported yet");
      return;
    }
  }
  if (symres.type->Flags() & core::semantic::SymbolFlags::kType) {
    PerformProjectWideSearch(symres.type.sym, VisitTypeReferencesInFile, params.global_search, [&](const auto* sf) {
      return accept_from_file(sf);
    });
    return;
  }
  if (symres.type->Flags() & core::semantic::SymbolFlags::kFunction) {
    PerformProjectWideSearch(symres.type.sym, VisitFunctionReferencesInFile, params.global_search, [&](const auto* sf) {
      return accept_from_file(sf);
    });
    return;
  }
}

}  // namespace vanadium::ls::detail
