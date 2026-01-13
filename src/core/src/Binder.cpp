#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/lib/Bitset.h>
#include <vanadium/lib/DelimitedStringView.h>

#include <algorithm>
#include <cassert>
#include <memory>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "vanadium/core/Program.h"
#include "vanadium/core/Semantic.h"

namespace vanadium::core {
namespace semantic {

class ExternalsTracker {
 public:
  ExternalsTracker() : active_(&secondary_) {}

  ModuleExternals Build() {
    ModuleExternals result{
        .primary = std::move(primary_),
        .secondary = std::move(secondary_),
        .augmented = {},
    };

    result.augmented.reserve(augmented_.size());
    for (auto& group : augmented_ | std::ranges::views::values) {
      result.augmented.push_back(std::move(group));
    }

    const auto resize_resolution_set = [](ExternallyResolvedGroup& group) {
      group.resolution_set = lib::Bitset(group.idents.size());
    };
    resize_resolution_set(result.primary);
    resize_resolution_set(result.secondary);
    std::ranges::for_each(result.augmented, resize_resolution_set);

    return result;
  }

  void With(ExternallyResolvedGroup* target, std::invocable auto f) {
    auto* upper = active_;

    active_ = target;
    f();
    active_ = upper;
  }

  void Augmented(std::string_view providers, Scope* scope, std::invocable auto f) {
    auto [group_it, _] =
        augmented_.try_emplace(providers,  //
                               std::vector<const ast::nodes::Ident*>{}, std::vector<semantic::Scope*>{}, providers);
    group_it->second.scopes.emplace_back(scope);
    With(&group_it->second, f);
  }

  ExternallyResolvedGroup* Primary() {
    return &primary_;
  }
  ExternallyResolvedGroup* Secondary() {
    return &primary_;
  }
  ExternallyResolvedGroup* Active() {
    return active_;
  }

 private:
  ExternallyResolvedGroup primary_{};
  ExternallyResolvedGroup secondary_{};
  std::unordered_map<std::string_view, ExternallyResolvedGroup> augmented_;

  ExternallyResolvedGroup* active_;
};

class Binder {
 public:
  explicit Binder(SourceFile& sf) : sf_(sf), inspector_(ast::NodeInspector::create<Binder, &Binder::Inspect>(this)) {}

  void Bind() {
    if (sf_.ast.root->nodes.empty() || sf_.ast.root->nodes.front()->nkind != ast::NodeKind::Module) [[unlikely]] {
      return;
    }

    const auto* top_level_node = sf_.ast.root->nodes.front()->As<ast::nodes::Module>();

    HoistNamesOf<ast::nodes::Module, &Binder::hoisted_names_>(top_level_node);

    Scope trash_scope(nullptr);  // to avoid scope_ != nullptr checks
    scope_ = &trash_scope;

    vd_types_external_group_ = externals_.Secondary();

    Visit(top_level_node);
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

  template <bool SetParent = true>
  Scope* Scoped(const ast::Node* container, std::invocable auto f) {
    auto* parent = scope_;
    auto* child = sf_.arena.Alloc<Scope>(container, SetParent ? parent : nullptr);
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
          .range = ast::utils::GetActualNameRange(sym.Declaration()),
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

  bool Inspect(const ast::Node*);

  [[nodiscard]] std::string_view Lit(const ast::Node* n) const {
    return sf_.Text(n);
  }
  [[nodiscard]] std::string_view Lit(const ast::Node& n) const {
    return Lit(&n);
  }
  [[nodiscard]] std::string_view Lit(const ast::Token* n) const {
    return sf_.Text(n);
  }

  void BindReference(const ast::nodes::Ident* ident, ExternallyResolvedGroup* externals_group) {
    const auto name = Lit(ident);

    if (hoisted_names_.contains(name) || hoisted_inner_names_.contains(name)) {
      return;
    }

    if (const auto* sym = scope_->Resolve(name); sym) {
      if (sym->Flags() & SymbolFlags::kImportedModule) [[unlikely]] {
        required_imports_.insert(sym->GetName());
      }
      return;
    }

    externals_group->idents.emplace_back(ident);
  }
  void BindReference(const ast::nodes::Ident* ident) {
    BindReference(ident, externals_.Active());
  }

  template <ast::IsNode NodeToInspect, auto TargetSetPtr>
  void HoistNamesOf(const NodeToInspect* n) {
    // concretizing NodeToInspect allows to avoid extra indirect call to n->Accept(...) switch
    n->Accept(ast::NodeInspector::create<Binder, &Binder::Hoist<TargetSetPtr>>(this));
  }
  template <auto TargetSetPtr>
  bool Hoist(const ast::Node*);

  template <ast::IsNode ConcreteNode, auto TargetSetPtr>
  void HoistName(const ast::Node* n) {
    const auto* m = n->As<ConcreteNode>();
    if (m->name) {
      (this->*TargetSetPtr).insert(Lit(*m->name));
    }
  };

  // returns true if resolution is done internally, otherwise false
  bool AugmentByExtensible(std::string_view extensible, SymbolFlags::Value flags_mask, std::invocable auto f,
                           const ast::Range& errrange = {});
  bool AugmentByExtensibles(std::string_view extensibles, SymbolFlags::Value flags_mask, std::invocable auto f);

  Symbol BindListSpec(std::string_view name, SymbolFlags::Value flags, const ast::nodes::ListSpec*);
  std::optional<Symbol> BindTypeSpec(std::string_view name, SymbolFlags::Value flags, const ast::nodes::TypeSpec*);
  SymbolTable& BindFields(const std::vector<ast::nodes::Field*>&);
  SymbolTable& BindEnumMembers(const std::vector<ast::nodes::Expr*>&);

  std::unordered_multimap<std::string_view, ImportDescriptor> imports_;
  std::unordered_set<std::string_view> required_imports_;

  ExternalsTracker externals_;
  ExternallyResolvedGroup* vd_types_external_group_;

  std::unordered_set<std::string_view> hoisted_names_;
  std::unordered_set<std::string_view> hoisted_inner_names_;

  Scope* scope_;

  SourceFile& sf_;
  const ast::NodeInspector inspector_;
};

template <auto TargetSetPtr>
bool Binder::Hoist(const ast::Node* n) {
  switch (n->nkind) {
    case ast::NodeKind::BlockStmt: {
      return false;
    }

    case ast::NodeKind::ComponentTypeDecl: {
      HoistName<ast::nodes::ComponentTypeDecl, TargetSetPtr>(n);
      return false;
    }
    case ast::NodeKind::StructTypeDecl: {
      HoistName<ast::nodes::StructTypeDecl, TargetSetPtr>(n);
      return false;
    }
    case ast::NodeKind::FuncDecl: {
      HoistName<ast::nodes::FuncDecl, TargetSetPtr>(n);
      return false;
    }
    case ast::NodeKind::TemplateDecl: {
      HoistName<ast::nodes::TemplateDecl, TargetSetPtr>(n);
      return false;
    }
    case ast::NodeKind::SubTypeDecl: {
      const auto* m = n->As<ast::nodes::SubTypeDecl>();
      HoistName<ast::nodes::Field, TargetSetPtr>(m->field);
      return false;
    }
    case ast::NodeKind::ClassTypeDecl: {
      HoistName<ast::nodes::ClassTypeDecl, TargetSetPtr>(n);
      return false;
    }
    case ast::NodeKind::EnumTypeDecl: {
      HoistName<ast::nodes::EnumTypeDecl, TargetSetPtr>(n);
      return false;
    }
    case ast::NodeKind::PortTypeDecl: {
      HoistName<ast::nodes::PortTypeDecl, TargetSetPtr>(n);
      return false;
    }

    case ast::NodeKind::ValueDecl: {
      const auto* m = n->As<ast::nodes::ValueDecl>();
      for (const auto* decl : m->decls) {
        HoistName<ast::nodes::Declarator, TargetSetPtr>(decl);
      }
      return false;
    }

    case ast::NodeKind::ConstructorDecl: {
      (this->*TargetSetPtr).insert("create");
      return false;
    }

    default:
      break;
  }

  return true;
}

bool Binder::AugmentByExtensible(std::string_view extensible, SymbolFlags::Value flags_mask, std::invocable auto f,
                                 const ast::Range& errrange) {
  const auto* sym = scope_->ResolveOwn(extensible);
  if (!sym) {
    externals_.Augmented(extensible, scope_, f);
    return false;
  }

  if (!(sym->Flags() & flags_mask)) {
    // TODO: add this check to xbind
    if (errrange.Length() > 0) {
      EmitError(SemanticError{
          .range = errrange,
          .type = SemanticError::Type::kClassCanBeExtendedByClassOnly,  // TODO: create proper error code
      });
    }
    return true;
  }

  const auto inject_internal_augmentation_provider = [&](const Symbol* provider_sym) {
    const semantic::SymbolTable* augmentation_table = (provider_sym->Flags() & semantic::SymbolFlags::kStructural)
                                                          ? provider_sym->Members()
                                                          : &provider_sym->OriginatedScope()->symbols;
    scope_->augmentation.push_back(augmentation_table);
  };

  if (const auto& extension_base = ast::utils::GetExtendsBase(sym->Declaration()); !extension_base.empty()) {
    bool pass_to_xbind{false};
    for (const auto* ename : extension_base) {
      const auto* esym = scope_->ResolveOwn(Lit(ename));
      if (!esym || !ast::utils::GetExtendsBase(esym->Declaration()).empty()) {
        // can be resolved here recursively, but such cases are very rare
        pass_to_xbind = true;
        break;
      }
    }

    inject_internal_augmentation_provider(sym);  // we can access it and it will be 1st

    // to avoid augmenting twice - partially hereby and completely (with duplicating) during xbind
    if (pass_to_xbind) {
      externals_.Augmented(sf_.Text(ast::Range{
                               .begin = extension_base.front()->nrange.begin,
                               .end = extension_base.front()->nrange.end,
                           }),
                           scope_, f);
      return false;
    }

    for (const auto* ename : extension_base) {
      inject_internal_augmentation_provider(scope_->ResolveOwn(Lit(ename)));
    }
  }

  inject_internal_augmentation_provider(sym);
  //
  f();

  return true;
}

bool Binder::AugmentByExtensibles(std::string_view extensibles, SymbolFlags::Value flags_mask, std::invocable auto f) {
  if (auto extension_base = lib::DelimitedStringView<','>{extensibles}.range(); !extension_base.empty()) {
    bool pass_to_xbind{false};
    for (const auto& ename : extension_base) {
      const auto* esym = scope_->ResolveOwn(ename);
      if (!esym || !ast::utils::GetExtendsBase(esym->Declaration()).empty()) {
        // can be resolved here recursively, but such cases are very rare
        pass_to_xbind = true;
        break;
      }
    }

    // to avoid augmenting twice - partially hereby and completely (with duplicating) during xbind
    if (pass_to_xbind) {
      externals_.Augmented(extensibles, scope_, f);
      return false;
    }

    for (const auto& ename : extension_base) {
      const auto* esym = scope_->ResolveOwn(ename);
      const semantic::SymbolTable* augmentation_table =
          (esym->Flags() & semantic::SymbolFlags::kStructural) ? esym->Members() : &esym->OriginatedScope()->symbols;
      scope_->augmentation.push_back(augmentation_table);
    }
    //
    f();
  }

  return true;
}

bool Binder::Inspect(const ast::Node* n) {
  switch (n->nkind) {
    case ast::NodeKind::Module: {
      if (sf_.module.has_value()) [[unlikely]] {
        EmitError(SemanticError{
            .range = n->nrange,
            .type = SemanticError::Type::kFileCanContainOnlyOneModule,
        });
        return false;
      }

      const auto* m = n->As<ast::nodes::Module>();

      Scoped<false>(m, [&] {
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

    case ast::NodeKind::Definition: {
      auto* pg = vd_types_external_group_;

      vd_types_external_group_ = externals_.Primary();
      Introspect(n);
      vd_types_external_group_ = pg;

      return false;
    }

    case ast::NodeKind::ImportDecl: {
      const auto* m = n->As<ast::nodes::ImportDecl>();

      if (m->module) {
        const auto* visibility = m->parent->As<ast::nodes::Definition>()->visibility;
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

      Visit(m->extends);
      auto* originated_scope = Scoped(m, [&] {
        const auto process = [&] {
          for (const auto* stmt : m->body->stmts) {
            if (stmt->nkind != ast::NodeKind::DeclStmt) {
              continue;
            }
            const auto* d = stmt->As<ast::nodes::DeclStmt>();
            if (d->decl->nkind != ast::NodeKind::ValueDecl) {
              continue;
            }
            const auto* vd = d->decl->As<ast::nodes::ValueDecl>();
            externals_.With(externals_.Primary(), [&] {
              Visit(vd->type);
            });
            for (const auto* declarator : vd->decls) {
              if (declarator->name) {
                AddSymbol({
                    Lit(std::addressof(*declarator->name)),
                    declarator,
                    SymbolFlags::kField,
                });
              }
            }
          }
        };

        if (m->extends.empty()) {
          process();
        } else {
          const auto augment_by = Lit(m->extends.front());  // <-- TODO
          if (const auto* sym = scope_->Resolve(augment_by)) {
            if (sym->Flags() & SymbolFlags::kComponent) {
              scope_->augmentation.push_back(&sym->OriginatedScope()->symbols);
            } else {
              // TODO: try to also bring up this error from xbind
              EmitError(SemanticError{
                  .range = m->extends.front()->nrange,
                  .type = SemanticError::Type::kClassCanBeExtendedByClassOnly,
              });
            }
            process();
          } else {
            externals_.Augmented(augment_by, scope_, process);
          }
        }
      });

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kComponentType,
            originated_scope,
        });
        // TODO
      }

      return false;
    }

    case ast::NodeKind::StructTypeDecl: {
      const auto* m = n->As<ast::nodes::StructTypeDecl>();

      SymbolTable* members;
      externals_.With(externals_.Primary(), [&] {
        members = &BindFields(m->fields);
      });

      SymbolFlags::Value flags = SymbolFlags::kStructuralType;
      if (m->kind.kind == ast::TokenKind::UNION) {
        flags = SymbolFlags::kUnionStructuralType;
      }

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            flags,
            members,
        });
        // TODO
      }

      return false;
    }

    case ast::NodeKind::PortTypeDecl: {
      const auto* m = n->As<ast::nodes::PortTypeDecl>();

      externals_.With(externals_.Primary(), [&] {
        Visit(m->attrs);
      });

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kPortType,
        });
        // TODO
      }

      return false;
    }

    case ast::NodeKind::FuncDecl: {
      const auto* m = n->As<ast::nodes::FuncDecl>();

      MaybeVisit(m->runs_on);
      MaybeVisit(m->system);
      MaybeVisit(m->ret);

      if (m->modif && Lit(m->modif) == "@abstract") {
        const auto* outer = m->parent->parent;
        if (outer->nkind == ast::NodeKind::ClassTypeDecl) {
          const auto* cdecl = outer->As<ast::nodes::ClassTypeDecl>();
          if (!cdecl->modif || Lit(cdecl->modif) != "@abstract") {
            // TODO: try to also bring up this error from xbind
            EmitError(SemanticError{
                .range = m->modif->range,
                .type = SemanticError::Type::kCannotHaveAbstractFunctionInNonAbstractClass,
            });
          }
        }
      }

      auto* originated_scope = Scoped(m, [&] {
        const auto bindf = [&] {
          externals_.With(externals_.Primary(), [&] {
            MaybeVisit(m->params);
          });
          MaybeVisit(m->body);
        };
        if (m->runs_on) {
          AugmentByExtensible(Lit(m->runs_on->comp), SymbolFlags::kComponent, bindf, m->runs_on->comp->nrange);
        } else {
          bindf();
        }
      });

      if (m->name) [[likely]] {
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
        externals_.With(externals_.Primary(), [&] {
          MaybeVisit(m->params);
        });
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

      externals_.With(externals_.Primary(), [&] {
        Visit(m->type);
      });

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

    case ast::NodeKind::ValueDecl: {
      const auto* m = n->As<ast::nodes::ValueDecl>();

      externals_.With(vd_types_external_group_, [&] {
        Visit(m->type);
      });
      Visit(m->decls);

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

      const auto* field = m->field;
      externals_.With(externals_.Primary(), [&] {
        Visit(field->type);
      });
      Visit(field->arraydef);
      MaybeVisit(field->pars);
      MaybeVisit(field->value_constraint);

      if (field->name) {
        if (field->type->nkind == ast::NodeKind::ListSpec) {
          AddSymbol(BindListSpec(Lit(*field->name), SymbolFlags::kNone, field->type->As<ast::nodes::ListSpec>()));
        } else {
          AddSymbol(semantic::Symbol{
              Lit(*field->name),
              m,
              SymbolFlags::kSubTypeType,
          });
        }
      }

      return false;
    }

    case ast::NodeKind::EnumTypeDecl: {
      const auto* m = n->As<ast::nodes::EnumTypeDecl>();

      auto& members = BindEnumMembers(m->values);

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kEnumType,
            &members,
        });
      }

      return false;
    }

    case ast::NodeKind::ClassTypeDecl: {
      const auto* m = n->As<ast::nodes::ClassTypeDecl>();

      // TODOs:
      //  - support multiple inheritance (extend xbind to support > 1 augmentation providers)
      //  -  ^ do the same to support runs on

      MaybeVisit(m->runs_on);
      MaybeVisit(m->system);

      HoistNamesOf<ast::nodes::ClassTypeDecl, &Binder::hoisted_inner_names_>(m);  // <--- hoist/start
      auto* originated_scope = Scoped(m, [&] {
        AddSymbol({
            "this",
            m,
            SymbolFlags::kThis,
        });

        if (!hoisted_inner_names_.contains("create")) {
          auto* pseudoctor = sf_.arena.Alloc<ast::nodes::ConstructorDecl>();
          pseudoctor->parent = const_cast<ast::nodes::ClassTypeDecl*>(m);
          pseudoctor->nrange = m->nrange;
          pseudoctor->params = [&] {
            auto* pseudoparams = sf_.arena.Alloc<ast::nodes::FormalPars>();
            pseudoparams->parent = pseudoctor;
            pseudoparams->nrange = {};
            //
            m->Accept([&](const ast::Node* cn) {
              if (cn->nkind == ast::NodeKind::Definition) {
                return true;
              }
              if (cn->nkind == ast::NodeKind::ValueDecl) {
                const auto* cvd = cn->As<ast::nodes::ValueDecl>();
                for (const auto* cd : cvd->decls) {
                  pseudoparams->list.emplace_back([&] {
                    auto* pseudopar = sf_.arena.Alloc<ast::nodes::FormalPar>();
                    pseudopar->parent = pseudoctor;
                    pseudopar->nrange = cd->nrange;

                    pseudopar->arraydef = cd->arraydef;
                    pseudopar->modif = cvd->modif;
                    pseudopar->restriction = cvd->template_restriction;
                    pseudopar->type = cvd->type;

                    pseudopar->name.emplace();
                    pseudopar->name->parent = cd->name->parent;
                    pseudopar->name->nrange = cd->name->nrange;

                    return pseudopar;
                  }());
                }
              }
              return false;
            });
            //
            return pseudoparams;
          }();
          pseudoctor->body = [&] {
            auto* pseudobody = sf_.arena.Alloc<ast::nodes::BlockStmt>();
            pseudobody->parent = pseudoctor;
            pseudobody->nrange = {};
            return pseudobody;
          }();
          AddSymbol({
              "create",
              pseudoctor,
              SymbolFlags::kConstructorFunction,
              scope_,
          });
        }

        const auto bindf = [&] {
          Visit(m->defs);
        };

        if (m->runs_on && m->extends.empty()) {
          AugmentByExtensible(Lit(m->runs_on->comp), SymbolFlags::kClass, bindf);
        } else if (!m->runs_on && !m->extends.empty()) {
          AugmentByExtensibles(sf_.Text(ast::Range{
                                   .begin = m->extends.front()->nrange.begin,
                                   .end = m->extends.back()->nrange.end,
                               }),
                               SymbolFlags::kClass, bindf);
        } else if (m->runs_on && !m->extends.empty()) {
          std::size_t len{m->runs_on->comp->nrange.Length()};
          for (const auto* ename : m->extends) {
            len += 1;  // ,
            len += ename->nrange.Length();
          }

          auto buf = sf_.arena.AllocStringBuffer(len);
          //
          auto it = std::ranges::copy(Lit(m->runs_on->comp), buf.begin()).out;
          for (const auto* ename : m->extends) {
            *it = ',';
            it = std::ranges::copy(Lit(ename), it).out;
          }

          AugmentByExtensibles(std::string_view(buf), SymbolFlags::kClass, bindf);
        } else {  // !m.runs_on && m.extends.empty()
          bindf();
        }
      });
      hoisted_inner_names_.clear();  // <--- hoist/end

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            SymbolFlags::kClassType,
            originated_scope,
        });
      }

      return false;
    }

    case ast::NodeKind::ConstructorDecl: {
      const auto* m = n->As<ast::nodes::ConstructorDecl>();

      auto* originated_scope = Scoped(m, [&] {
        Visit(m->params);
        if (m->body) {
          Visit(m->body);
        }
      });

      AddSymbol({
          "create",
          m,
          SymbolFlags::kConstructorFunction,
          originated_scope,
      });

      return false;
    }

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

    case ast::NodeKind::SelectStmt: {
      const auto* m = n->As<ast::nodes::SelectStmt>();

      Visit(m->tag);
      for (const auto* clause : m->clauses) {
        Visit(clause->body);
        if (!m->is_union) [[likely]] {
          // should be validated by typechecker
          Visit(clause->cond);
        }
      }

      return false;
    }

    case ast::NodeKind::DynamicExpr: {
      const auto* m = n->As<ast::nodes::DynamicExpr>();

      Scoped(m, [&] {
        AddSymbol({
            "value",
            m->parent->As<ast::nodes::TemplateDecl>()->type,
            SymbolFlags::kVariable,
        });
        Visit(m->body);
      });

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

Symbol Binder::BindListSpec(std::string_view name, SymbolFlags::Value flags, const ast::nodes::ListSpec* ls) {
  SymbolTable* containment{nullptr};
  //
  if (ls->elemtype->nkind != ast::NodeKind::RefSpec) {
    containment = sf_.arena.Alloc<SymbolTable>();
    containment->Add(std::move(*BindTypeSpec(
        [&] {
          std::span<char> shadow_name = sf_.arena.AllocStringBuffer(1);
          shadow_name.front() = kShadowStaticMemberPrefix;
          return std::string_view{shadow_name};
        }(),
        SymbolFlags::kAnonymous, ls->elemtype)));
  } else {
    Visit(ls->elemtype);
  }
  //
  return Symbol{name, ls, SymbolFlags::Value(flags | SymbolFlags::kListType), containment};
}

std::optional<Symbol> Binder::BindTypeSpec(std::string_view name, SymbolFlags::Value flags,
                                           const ast::nodes::TypeSpec* spec) {
  switch (spec->nkind) {
    case ast::NodeKind::StructSpec:
      return Symbol{name, spec, SymbolFlags::Value(flags | SymbolFlags::kStructuralType),
                    &BindFields(spec->As<ast::nodes::StructSpec>()->fields)};
    case ast::NodeKind::EnumSpec: {
      const auto* m = spec->As<ast::nodes::EnumSpec>();
      for (const auto* val : m->values) {
        if (val->nkind == ast::NodeKind::CallExpr) {
          Visit(val->As<ast::nodes::CallExpr>()->args);
        }
      }
      return Symbol{name, spec, SymbolFlags::Value(flags | SymbolFlags::kEnumType),
                    &BindEnumMembers(spec->As<ast::nodes::EnumSpec>()->values)};
    }
    case ast::NodeKind::ListSpec: {
      return BindListSpec(name, flags, spec->As<ast::nodes::ListSpec>());
    }
    default:
      return std::nullopt;
  }
}

SymbolTable& Binder::BindFields(const std::vector<ast::nodes::Field*>& fields) {
  auto& members = NewSymbolTable();
  for (const auto* field : fields) {
    if (!field->name) {
      continue;
    }

    const auto name = Lit(std::addressof(*field->name));
    AddSymbol(members, {
                           name,
                           field,
                           SymbolFlags::kField,
                       });

    if (field->type->nkind == ast::NodeKind::RefSpec) {
      Visit(field->type);
      continue;
    }
    std::span<char> shadow_name = sf_.arena.AllocStringBuffer(1 + name.length());
    shadow_name.front() = kShadowStaticMemberPrefix;
    std::ranges::copy(name, shadow_name.begin() + 1);
    if (auto sub = BindTypeSpec(std::string_view{shadow_name}, SymbolFlags::kAnonymous, field->type); sub) {
      // we don't want to use AddSymbol as its additional checks had already been applied to parent
      members.Add(std::move(*sub));
    }
  }
  return members;
}

SymbolTable& Binder::BindEnumMembers(const std::vector<ast::nodes::Expr*>& values) {
  auto& members = NewSymbolTable();
  for (const auto* item : values) {
    const ast::nodes::Ident* valname = ast::utils::GetEnumValueNamePart(item);
    if (valname != nullptr) [[likely]] {
      const auto item_name = Lit(valname);
      semantic::Symbol sym{
          item_name,
          valname,
          SymbolFlags::kEnumMember,
      };
      AddSymbol(members, std::move(sym));
    }
  }
  return members;
}

std::string_view ShadowMemberKey(std::string_view opaque_name) {
  static thread_local std::string buf;
  buf.resize(1 + opaque_name.size());
  buf.front() = semantic::kShadowStaticMemberPrefix;
  std::ranges::copy(opaque_name, buf.begin() + 1);
  return buf;
}

void Bind(SourceFile& sf) {
  Binder(sf).Bind();
}

}  // namespace semantic
}  // namespace vanadium::core
