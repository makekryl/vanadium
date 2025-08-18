#include <cassert>
#include <memory>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "AST.h"
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
    if (sf_.ast.root->nodes.empty() || sf_.ast.root->nodes.front()->nkind != ast::NodeKind::Module) [[unlikely]] {
      return;
    }

    const auto* top_level_node = sf_.ast.root->nodes.front()->As<ast::nodes::Module>();

    HoistNamesOf<ast::nodes::Module, &Binder::hoisted_names_>(top_level_node);

    Scope trash_scope(nullptr);  // to avoid scope_ != nullptr checks
    scope_ = &trash_scope;
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

  void BindReference(const ast::nodes::Ident* ident) {
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

    externals_->idents.emplace_back(ident);
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

  std::optional<Symbol> BindTypeSpec(std::string_view name, SymbolFlags::Value flags, const ast::nodes::TypeSpec*);
  SymbolTable& BindFields(const std::vector<ast::nodes::Field*>&);
  SymbolTable& BindEnumMembers(const std::vector<ast::nodes::Expr*>&);

  std::unordered_multimap<std::string_view, ImportDescriptor> imports_;
  std::unordered_set<std::string_view> required_imports_;
  ExternalsTracker externals_;

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
            SymbolFlags::kComponentStructuralType,
            &members,
        });
        // TODO
      }

      return false;
    }

    case ast::NodeKind::StructTypeDecl: {
      const auto* m = n->As<ast::nodes::StructTypeDecl>();

      auto& members = BindFields(m->fields);

      SymbolFlags::Value flags = SymbolFlags::kStructuralType;
      if (m->kind.kind == ast::TokenKind::UNION) {
        flags = SymbolFlags::kUnionStructuralType;
      }

      if (m->name) {
        AddSymbol({
            Lit(std::addressof(*m->name)),
            m,
            flags,
            &members,
        });
        // TODO
      }

      return false;
    }

    case ast::NodeKind::PortTypeDecl: {
      const auto* m = n->As<ast::nodes::PortTypeDecl>();

      Visit(m->attrs);

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
        const auto process = [&] {
          MaybeVisit(m->params);
          MaybeVisit(m->body);
        };

        if (m->runs_on) {
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

      const auto* field = m->field;
      Visit(field->type);
      Visit(field->arraydef);
      MaybeVisit(field->pars);
      MaybeVisit(field->value_constraint);

      if (field->name) {
        if (field->type->nkind == ast::NodeKind::ListSpec) {
          AddSymbol(semantic::Symbol{
              Lit(*field->name),
              field,
              SymbolFlags::kListType,
          });
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

        const auto process = [&] {
          Visit(m->defs);
        };

        // this mixed hot mess copied from FuncDecl will be cleaned up after supporting multi-augmentation
        if (m->runs_on) {
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
        } else if (!m->extends.empty()) {
          const auto augment_by = Lit(m->extends.front());  // <-- TODO
          if (const auto* sym = scope_->Resolve(augment_by)) {
            if (sym->Flags() & SymbolFlags::kClass) {
              scope_->augmentation.push_back(&sym->OriginatedScope()->symbols);
            } else {
              // TODO: try to also bring up this error from xbind
              EmitError(SemanticError{
                  .range = m->runs_on->nrange,
                  .type = SemanticError::Type::kClassCanBeExtendedByClassOnly,
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

std::optional<Symbol> Binder::BindTypeSpec(std::string_view name, SymbolFlags::Value flags,
                                           const ast::nodes::TypeSpec* spec) {
  switch (spec->nkind) {
    case ast::NodeKind::StructSpec:
      return Symbol{name, spec, SymbolFlags::Value(flags | SymbolFlags::kStructuralType),
                    &BindFields(spec->As<ast::nodes::StructSpec>()->fields)};
    case ast::NodeKind::EnumSpec:
      Visit(spec);
      return Symbol{name, spec, SymbolFlags::Value(flags | SymbolFlags::kEnumType),
                    &BindEnumMembers(spec->As<ast::nodes::EnumSpec>()->values)};
    case ast::NodeKind::ListSpec:
      Visit(spec);
      return Symbol{name, spec, SymbolFlags::Value(flags | SymbolFlags::kListType)};
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
