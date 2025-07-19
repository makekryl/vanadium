#include <cassert>
#include <memory>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "Bitset.h"
#include "Program.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"

namespace vanadium::core {
namespace semantic {

class ExternalsTracker {
 public:
  ExternalsTracker() : active_(&top_level_) {}

  std::vector<ExternallyResolvedGroup> Build() {
    std::vector<ExternallyResolvedGroup> result;
    result.reserve(mapping_.size() + (top_level_.idents.empty() ? 0 : 1));

    if (!top_level_.idents.empty()) {
      top_level_.resolution_set = lib::Bitset(top_level_.idents.size());
      result.push_back(std::move(top_level_));
    }

    for (auto& group : mapping_ | std::ranges::views::values) {
      // TODO: do something with this ugly bitset after-construction resizing (actually, replacing the 0-sized one)
      //       also applies to the same thing few lines upper
      group.resolution_set = lib::Bitset(group.idents.size());
      result.push_back(std::move(group));
    }
    return result;
  }

  void Augmented(std::string_view provider, Scope* scope, std::invocable auto f) {
    auto* upper = active_;

    auto [group_it, _] = mapping_.try_emplace(provider, std::vector<const ast::nodes::Ident*>{},
                                              std::vector<semantic::Scope*>{}, provider);
    active_ = &group_it->second;
    active_->scopes.emplace_back(scope);
    //
    f();
    //
    active_ = upper;
  }

  ExternallyResolvedGroup* operator->() {
    return active_;
  }
  ExternallyResolvedGroup& operator*() {
    return *active_;
  }

 private:
  ExternallyResolvedGroup top_level_{{}, {}, ""};
  std::unordered_map<std::string_view, ExternallyResolvedGroup> mapping_;

  ExternallyResolvedGroup* active_;
};

class Binder {
 public:
  explicit Binder(SourceFile& sf) : sf_(sf), inspector_(ast::NodeInspector::create<Binder, &Binder::Inspect>(this)) {}

  void Bind() {
    sf_.ast.root->Accept(ast::NodeInspector::create<Binder, &Binder::Hoist>(this));

    scope_ = nullptr;
    sf_.ast.root->Accept(inspector_);

    for (auto& [name, sym] : enum_values_syms_) {
      if (!sf_.module->scope->symbols.Has(name)) {
        sf_.module->scope->symbols.Add(std::move(sym));
      }
    }
  }

 private:
  void Introspect(const ast::Node* n) {
    n->Accept(inspector_);
  }

  void Visit(const ast::Node* n) {
    if (Inspect(n)) {
      Introspect(n);
    }
  }

  template <ast::IsNode ConcreteNode>
  void Visit(const std::vector<ConcreteNode*>& nodes) {
    for (const auto* n : nodes) {
      Visit(n);
    }
  }

  void MaybeVisit(const ast::Node* n) {
    if (n != nullptr) {
      Visit(n);
    }
  }

  Scope* Scoped(const ast::Node* container, std::invocable auto f) {
    auto* parent = scope_;
    auto* child = sf_.arena.Alloc<Scope>(container, parent);
    scope_ = child;

    //
    f();
    //

    if (parent != nullptr) [[likely]] {
      parent->AddChild(child);
    }
    scope_ = parent;

    return child;
  }

  [[nodiscard]] SymbolTable& NewSymbolTable() {
    return *sf_.arena.Alloc<SymbolTable>();
  }

  struct SymbolAdditionOptions {
    bool redefine_if_exists{true};
  };

  template <SymbolAdditionOptions Options = {}>
  void AddSymbol(SymbolTable& table, Symbol&& sym) {
    if (table.Has(sym.GetName())) {
      if constexpr (!Options.redefine_if_exists) {
        return;
      }
      sf_.semantic_errors.emplace_back(SemanticError{
          .range = sym.Declaration()->nrange,
          .type = SemanticError::Type::kRedefinition,
      });
    }
    table.Add(std::move(sym));
  }

  template <SymbolAdditionOptions Options = {}>
  void AddSymbol(Symbol&& sym) {
    AddSymbol<Options>(scope_->symbols, std::move(sym));
  }

  void EmitError(SemanticError&& err) {
    sf_.semantic_errors.emplace_back(std::move(err));
  }

  bool Hoist(const ast::Node*);
  bool Inspect(const ast::Node*);

  [[nodiscard]] std::string_view Lit(const ast::nodes::Ident* ident) const {
    return ident->On(sf_.ast.src);
  }
  [[nodiscard]] std::string_view Lit(const ast::nodes::Ident& ident) const {
    return Lit(&ident);
  }

  void BindReference(const ast::nodes::Ident* ident) {
    const auto name = Lit(ident);

    if (hoisted_names_.contains(name)) {
      return;
    }

    if (const auto* sym = scope_->Resolve(name); sym) {
      if (sym->Flags() & SymbolFlags::kImportedModule) [[unlikely]] {
        required_imports_.insert(sym->GetName());
      }
      return;
    }

    externals_->idents.emplace_back(ident);
  }

  template <ast::IsNode ConcreteNode>
  void HoistName(const ast::Node* n) {
    const auto* m = n->As<ConcreteNode>();
    if (m->name) {
      hoisted_names_.insert(Lit(*m->name));
    }
  };

  std::unordered_multimap<std::string_view, ImportDescriptor> imports_;
  std::unordered_set<std::string_view> required_imports_;
  ExternalsTracker externals_;

  std::unordered_set<std::string_view> hoisted_names_;
  std::unordered_map<std::string_view, Symbol> enum_values_syms_;

  Scope* scope_;

  SourceFile& sf_;
  const ast::NodeInspector inspector_;
};

bool Binder::Hoist(const ast::Node* n) {
  switch (n->nkind) {
    case ast::NodeKind::BlockStmt: {
      return false;
    }

    case ast::NodeKind::ComponentTypeDecl: {
      HoistName<ast::nodes::ComponentTypeDecl>(n);
      return false;
    }
    case ast::NodeKind::StructTypeDecl: {
      HoistName<ast::nodes::StructTypeDecl>(n);
      return false;
    }
    case ast::NodeKind::FuncDecl: {
      HoistName<ast::nodes::FuncDecl>(n);
      return false;
    }
    case ast::NodeKind::TemplateDecl: {
      HoistName<ast::nodes::TemplateDecl>(n);
      return false;
    }
    case ast::NodeKind::SubTypeDecl: {
      const auto* m = n->As<ast::nodes::SubTypeDecl>();
      HoistName<ast::nodes::Field>(m->field);
      return false;
    }
    case ast::NodeKind::ClassTypeDecl: {
      HoistName<ast::nodes::ClassTypeDecl>(n);
      return false;
    }

    case ast::NodeKind::EnumTypeDecl: {
      const auto* m = n->As<ast::nodes::EnumTypeDecl>();
      HoistName<ast::nodes::EnumTypeDecl>(m);

      for (const auto* item : m->enums) {
        const ast::nodes::Ident* valname = ast::utils::GetEnumValueNamePart(item);
        if (valname != nullptr) [[likely]] {
          hoisted_names_.insert(Lit(valname));
        }
      }

      return false;
    }

    default:
      break;
  }

  return true;
}

bool Binder::Inspect(const ast::Node* n) {
  switch (n->nkind) {
    case ast::NodeKind::Module: {
      if (sf_.module.has_value()) [[unlikely]] {
        EmitError(SemanticError{
            .range = n->nrange,
            .type = SemanticError::Type::kToDo,  // 1 module per file
        });
        return false;
      }

      const auto* m = n->As<ast::nodes::Module>();

      scope_ = nullptr;
      Scoped(m, [&] {
        Visit(m->defs);
        if (m->name) {
          const auto name = Lit(std::addressof(*m->name));

          sf_.module.emplace();
          sf_.module->name = name;
          sf_.module->sf = &sf_;
          sf_.module->scope = scope_;
          sf_.module->imports = std::move(imports_);
          sf_.module->required_imports = std::move(required_imports_);
          sf_.module->externals = externals_.Build();

          imports_ = {};
          externals_ = {};
          required_imports_ = {};
        }
      });

      return false;
    }

    case ast::NodeKind::ImportDecl: {
      const auto* m = n->As<ast::nodes::ImportDecl>();

      if (m->module) {
        const auto* visibility = m->parent->As<ast::nodes::ModuleDef>()->visibility;
        const bool transit = !m->list.empty() && m->list[0]->kind.kind == ast::TokenKind::IMPORT;
        imports_.emplace(Lit(std::addressof(*m->module)),
                         ImportDescriptor{
                             .transit = transit,
                             .is_public = (visibility != nullptr) && visibility->kind == ast::TokenKind::PUBLIC,
                             .declaration = m,
                         });

        // TODO
        if (!transit) {
          AddSymbol({
              Lit(std::addressof(*m->module)),
              m,
              SymbolFlags::kImportedModule,
          });
        }
      }

      return false;
    }

    case ast::NodeKind::ComponentTypeDecl: {
      const auto* m = n->As<ast::nodes::ComponentTypeDecl>();

      auto& members = NewSymbolTable();
      for (const auto* stmt : m->body->stmts) {
        if (stmt->nkind == ast::NodeKind::DeclStmt) {
          const auto* d = stmt->As<ast::nodes::DeclStmt>();
          if (d->decl->nkind == ast::NodeKind::ValueDecl) {
            const auto* vd = d->decl->As<ast::nodes::ValueDecl>();
            Visit(vd->type);
            for (const auto* declarator : vd->decls) {
              if (declarator->name) {
                AddSymbol(members, {
                                       Lit(std::addressof(*declarator->name)),
                                       declarator,
                                       SymbolFlags::kField,
                                   });
              }
            }
          }
        }
      }

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kStructural,
            &members,
        });
        // TODO
      }

      return false;
    }

    case ast::NodeKind::StructTypeDecl: {
      const auto* m = n->As<ast::nodes::StructTypeDecl>();

      auto& members = NewSymbolTable();
      for (const auto* field : m->fields) {
        if (!field->name) {
          continue;
        }
        Visit(field->type);
        AddSymbol(members, {
                               Lit(std::addressof(*field->name)),
                               field,
                               SymbolFlags::kField,
                           });
      }

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kStructural,
            &members,
        });
        // TODO
      }

      return false;
    }

    case ast::NodeKind::FuncDecl: {
      const auto* m = n->As<ast::nodes::FuncDecl>();

      auto* originated_scope = Scoped(m, [&] {
        Visit(m->params);
        const auto process = [&] {
          if (m->body) {
            Visit(m->body);
          }
        };
        if (m->runs_on) {
          Visit(m->runs_on);
        }
        if (m->system) {
          Visit(m->system);
        }
        if (m->ret) {
          Visit(m->ret);
        }

        const bool augmenting = !!m->runs_on;
        if (augmenting) {
          const auto augment_by = Lit(m->runs_on->comp);
          if (const auto* sym = scope_->Resolve(augment_by)) {
            if (sym->Flags() & SymbolFlags::kComponent) {
              scope_->augmentation.push_back(sym->Members());
            } else {
              // TODO: try to also bring up this error from xbind
              EmitError(SemanticError{
                  .range = m->runs_on->nrange,
                  .type = SemanticError::Type::kRunsOnRequiresComponent,
              });
            }
            process();
          } else {
            externals_.Augmented(augment_by, scope_, process);
          }
        } else {
          process();
        }
      });

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kFunction,
            originated_scope,
        });
      }

      return false;
    }

    case ast::NodeKind::ControlPart: {
      const auto* m = n->As<ast::nodes::ControlPart>();

      AddSymbol({
          "__CONTROLPART__",
          m,
          SymbolFlags::kControl,
          Scoped(m,
                 [&] {
                   Introspect(m);
                 }),
      });

      return false;
    }

    case ast::NodeKind::BlockStmt: {
      const auto* m = n->As<ast::nodes::BlockStmt>();

      Scoped(m, [&] {
        Visit(m->stmts);
      });

      return false;
    }

    case ast::NodeKind::ForStmt: {
      const auto* m = n->As<ast::nodes::ForStmt>();

      Scoped(m, [&] {
        Visit(m->init);
        Visit(m->cond);
        Visit(m->post);
        Visit(m->body);
      });

      return false;
    }

    case ast::NodeKind::TemplateDecl: {
      const auto* m = n->As<ast::nodes::TemplateDecl>();

      Visit(m->type);
      auto* originated_scope = Scoped(m, [&] {
        MaybeVisit(m->params);  // really maybe?
        const auto process = [&] {
          Visit(m->value);
        };
        if (m->base && m->base->nkind == ast::NodeKind::Ident) {
          Visit(m->base);
          externals_.Augmented(Lit(m->base->As<ast::nodes::Ident>()), scope_, process);
        } else {
          process();
        }
      });

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kTemplate,
            originated_scope,
        });
      }

      return false;
    }

    case ast::NodeKind::FormalPar: {
      const auto* m = n->As<ast::nodes::FormalPar>();
      Visit(m->type);
      MaybeVisit(m->value);
      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kArgument,
        });
      }
      return false;
    }

    case ast::NodeKind::Declarator: {
      const auto* m = n->As<ast::nodes::Declarator>();

      MaybeVisit(m->value);

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kVariable,
        });
      }

      return false;
    }

    case ast::NodeKind::SubTypeDecl: {
      const auto* m = n->As<ast::nodes::SubTypeDecl>();

      // todo: why is it stored as a Field?!
      const auto* field = m->field;
      Visit(field->type);
      Visit(field->arraydef);
      MaybeVisit(field->pars);
      MaybeVisit(field->value_constraint);

      if (field->name) {
        AddSymbol(semantic::Symbol{
            Lit(*field->name),
            m,
            SymbolFlags::kSubType,
        });
      }

      return false;
    }

    case ast::NodeKind::EnumTypeDecl: {
      const auto* m = n->As<ast::nodes::EnumTypeDecl>();

      auto& members = NewSymbolTable();
      for (const auto* item : m->enums) {
        const ast::nodes::Ident* valname = ast::utils::GetEnumValueNamePart(item);
        if (valname != nullptr) [[likely]] {
          const auto item_name = Lit(valname);
          semantic::Symbol sym{
              item_name,
              valname,
              SymbolFlags::kEnumValue,
          };
          enum_values_syms_.emplace(item_name, sym);
          AddSymbol(members, std::move(sym));
        }
      }

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kEnum,
            &members,
        });
      }

      return false;
    }

    case ast::NodeKind::ClassTypeDecl: {
      const auto* m = n->As<ast::nodes::ClassTypeDecl>();

      auto& members = NewSymbolTable();
      // !!! TODO !!!
      // bind properly and remove the visits below - it is just a stub
      Scoped(m, [&] {
        Visit(m->defs);
      });

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kStructural,
            &members,
        });
      }

      return true;
    }

      // case ast::NodeKind::Field: {
      //   const auto* m = n->As<ast::nodes::Field>();

      //   if (m->name) {
      //     members_->Add({
      //         Lit(std::addressof(*m->name)),
      //         m,
      //         SymbolFlags::kField,
      //     });
      //   }
      //   return false;
      // }

    case ast::NodeKind::CompositeLiteral: {
      const auto* m = n->As<ast::nodes::CompositeLiteral>();
      for (const auto* e : m->list) {
        if (e->nkind != ast::NodeKind::AssignmentExpr) {
          Visit(e);
          continue;
        }
        const auto* ae = e->As<ast::nodes::AssignmentExpr>();
        Visit(ae->value);
      }
      return false;
    }
    case ast::NodeKind::ParenExpr: {
      const auto* m = n->As<ast::nodes::ParenExpr>();
      for (const auto* e : m->list) {
        if (e->nkind != ast::NodeKind::AssignmentExpr) {
          Visit(e);
          continue;
        }
        const auto* ae = e->As<ast::nodes::AssignmentExpr>();
        Visit(ae->value);
      }
      return false;
    }

    case ast::NodeKind::SelectorExpr: {
      const auto* se = n->As<ast::nodes::SelectorExpr>();
      Visit(ast::utils::TraverseSelectorExpressionStart(se));
      return false;
    }

    case ast::NodeKind::Ident: {
      const auto* m = n->As<ast::nodes::Ident>();

      BindReference(m);

      return true;
    }

    default:
      break;
  }

  return true;
}

}  // namespace semantic

//

void Bind(SourceFile& sf) {
  semantic::Binder(sf).Bind();
}

}  // namespace vanadium::core
