#include "vanadium/format/AstSerializer.h"

#include <optional>
#include <string_view>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/Assert.h>
#include <vanadium/lib/Metaprogramming.h>

namespace vanadium::format {

namespace {

Sequence& operator+=(Sequence& seq, Unit u) {
  seq.units.emplace_back(std::move(u));
  return seq;
}

class AstSerializer {
 public:
  AstSerializer(std::string_view src, lib::Arena& arena) : src_(src), arena_(arena) {}

  Unit Serialize(const ast::Node* n) {
    return S(n);
  }

 private:
  Unit S(const ast::Node*);

  Unit S(const std::optional<ast::nodes::Ident>& ident) {
    if (!ident) {
      return EmptyUnit{};
    }
    return S(std::addressof(*ident));
  }

  Unit S(const ast::Token& tok) {
    return tok.On(src_);
  }
  Unit S(const ast::Token* tok) {
    return S(*tok);
  }

  Sequence* NewSequence(Sequence::Attribute attrs, mp::Consumer<Sequence&> auto f) {
    auto* seq = arena_.Alloc<Sequence>(Sequence{.attributes = attrs, .units = {}});
    f(*seq);
    return seq;
  }

  template <typename T>
  void Join(Sequence& target, const std::vector<T>& items, Unit separator) {
    for (std::size_t i = 0; i + 1 < items.size(); ++i) {
      target += S(items[i]);
      target += separator;
    }
    if (!items.empty()) {
      target += S(items.back());
    }
  }

  std::string_view src_;
  lib::Arena& arena_;
};

Unit AstSerializer::S(const ast::Node* n) {  // NOLINT(readability-function-size)
  // NOLINTNEXTLINE(readability-identifier-naming)
  const auto S_pars = [this](Sequence& seq, const ast::nodes::FormalPars* pars) -> void {
    seq += "<";
    seq += S(pars);
    seq += ">";
  };

  switch (n->nkind) {
    case ast::NodeKind::RootNode: {
      const auto* m = n->As<ast::RootNode>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        m->Accept([&](const auto* cn) {
          seq += S(cn);
          return false;
        });
      });
    }

    case ast::NodeKind::Ident: {
      const auto* m = n->As<ast::nodes::Ident>();
      return m->On(src_);
    }

    case ast::NodeKind::CompositeIdent: {
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        const auto* m = n->As<ast::nodes::CompositeIdent>();
        seq += S(m->tok1);
        seq += PrintDirective::kSpace;
        seq += S(m->tok2);
      });
    }

    case ast::NodeKind::Module: {
      const auto* m = n->As<ast::nodes::Module>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "module";
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        if (m->language) {
          seq += PrintDirective::kSpace;
          seq += S(m->language);
        }
        seq += PrintDirective::kSpace;
        seq += PrintDirective::kHardLine;
        seq += "{";
        seq += PrintDirective::kHardLine;
        if (!m->defs.empty()) {
          if (m->defs.front()->def->nkind != ast::NodeKind::ImportDecl) {
            seq += PrintDirective::kHardLine;
          }
          Join(seq, m->defs, PrintDirective::kHardLine);
          seq += PrintDirective::kHardLine;
          seq += PrintDirective::kHardLine;
        }
        seq += "}";
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
    }

    case ast::NodeKind::Definition: {
      const auto* m = n->As<ast::nodes::Definition>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->visibility) {
          seq += S(m->visibility);
          seq += PrintDirective::kSpace;
        }
        seq += S(m->def);
        switch (m->def->nkind) {
          case ast::NodeKind::ImportDecl:
          case ast::NodeKind::SubTypeDecl:
          case ast::NodeKind::ValueDecl:
            seq += PrintDirective::kSemicolon;
            break;
          default:
            break;
        }
      });
    }

    case ast::NodeKind::StructTypeDecl: {
      const auto* m = n->As<ast::nodes::StructTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "type";
        seq += PrintDirective::kSpace;
        seq += S(m->kind);
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        seq += PrintDirective::kSpace;
        seq += PrintDirective::kHardLine;
        seq += "{";
        seq += PrintDirective::kHardLine;
        seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& fseq) {
          Join(fseq, m->fields, NewSequence(Sequence::Attribute::kNone, [](auto& sseq) {
                 sseq += ",";
                 sseq += PrintDirective::kHardLine;
               }));
        });
        seq += PrintDirective::kHardLine;
        seq += "}";
      });
    }

    case ast::NodeKind::Field: {
      const auto* m = n->As<ast::nodes::Field>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->type);
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        for (const auto* pe : m->arraydef) {
          seq += "[";
          // TODO: check why it is being parsed as a list
          for (const auto* expr : pe->list) {
            seq += S(expr);
          }
          seq += "]";
        }
        if (m->length) {
          seq += PrintDirective::kSpace;
          seq += S(m->length);
        }
        if (m->value_constraint) {
          seq += PrintDirective::kSpace;
          seq += S(m->value_constraint);
        }
        if (m->optional) {
          seq += PrintDirective::kSpace;
          seq += "optional";
        }
      });
    }

    case ast::NodeKind::SubTypeDecl: {
      const auto* m = n->As<ast::nodes::SubTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "type";
        seq += PrintDirective::kSpace;
        seq += S(m->field);
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
    }

    case ast::NodeKind::RefSpec: {
      const auto* m = n->As<ast::nodes::RefSpec>();
      return S(m->x);
    }

    case ast::NodeKind::ListSpec: {
      const auto* m = n->As<ast::nodes::ListSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        seq += PrintDirective::kSpace;
        seq += "of";
        if (m->length) {
          seq += PrintDirective::kSpace;
          seq += S(m->length);
        }
        seq += PrintDirective::kSpace;
        seq += S(m->elemtype);
      });
    }

    case ast::NodeKind::FuncDecl: {
      const auto* m = n->As<ast::nodes::FuncDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->external) {
          seq += "external";
          seq += PrintDirective::kSpace;
        }
        seq += S(m->kind);
        if (m->modif) {
          seq += PrintDirective::kSpace;
          seq += S(m->modif);
        }
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        if (m->params) {
          seq += "(";
          seq += S(m->params);
          seq += ")";
        }
        if (m->runs_on) {
          seq += PrintDirective::kSpace;
          seq += S(m->runs_on);
        }
        if (m->mtc) {
          seq += PrintDirective::kSpace;
          seq += S(m->runs_on);
        }
        if (m->system) {
          seq += PrintDirective::kSpace;
          seq += S(m->system);
        }
        if (m->ret) {
          seq += PrintDirective::kSpace;
          seq += S(m->ret);
        }

        if (!m->external) {
          seq += PrintDirective::kHardLine;
          seq += S(m->body);
        }
      });
    }

    case ast::NodeKind::FormalPars: {
      const auto* m = n->As<ast::nodes::FormalPars>();
      return NewSequence(Sequence::Attribute::kGrouped, [&](auto& seq) {
        seq += PrintDirective::kSoftLine;
        Join(seq, m->list, NewSequence(Sequence::Attribute::kNone, [](auto& sseq) {
               sseq += ",";
               sseq += PrintDirective::kSpaceOrLine;
             }));
        seq += PrintDirective::kSoftLine;
      });
    }

    case ast::NodeKind::BlockStmt: {
      const auto* m = n->As<ast::nodes::BlockStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "{";
        if (!m->stmts.empty()) {
          seq += PrintDirective::kHardLine;
          seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& iseq) {
            const auto t_stmt = [&](const ast::nodes::Stmt* stmt) {
              iseq += S(stmt);
              switch (stmt->nkind) {
                case ast::NodeKind::ReturnStmt:
                case ast::NodeKind::BranchStmt:
                  iseq += PrintDirective::kSemicolon;
                  break;
                default:
                  break;
              }
            };
            for (std::size_t i = 0; i + 1 < m->stmts.size(); ++i) {
              t_stmt(m->stmts[i]);
              iseq += PrintDirective::kHardLine;
              // todo: chk [i+1]
            }
            if (!m->stmts.empty()) {
              t_stmt(m->stmts.back());
            }
          });
          seq += PrintDirective::kHardLine;
        } else {
          seq += PrintDirective::kSpace;
        }
        seq += "}";
      });
    }

    case ast::NodeKind::ExprStmt: {
      const auto* m = n->As<ast::nodes::ExprStmt>();
      return S(m->expr);
    }
    case ast::NodeKind::DeclStmt: {
      const auto* m = n->As<ast::nodes::DeclStmt>();
      return S(m->decl);
    }

    case ast::NodeKind::ValueDecl: {
      const auto* m = n->As<ast::nodes::ValueDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->kind) {
          seq += S(m->kind);
          seq += PrintDirective::kSpace;
        }
        if (m->template_restriction) {
          seq += S(m->template_restriction);
          seq += PrintDirective::kSpace;
        }
        if (m->modif) {
          seq += S(m->modif);
          seq += PrintDirective::kSpace;
        }
        seq += S(m->type);
        seq += PrintDirective::kSpace;
        seq += NewSequence(Sequence::Attribute::kGrouped, [&](auto& dseq) {
          Join(dseq, m->decls, NewSequence(Sequence::Attribute::kNone, [](auto& sseq) {
                 sseq += ",";
                 sseq += PrintDirective::kSpaceOrLine;
               }));
        });
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
    }

    case ast::NodeKind::Declarator: {
      const auto* m = n->As<ast::nodes::Declarator>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->name);
        for (const auto* pe : m->arraydef) {
          seq += "[";
          // TODO: check why it is being parsed as a list
          for (const auto* expr : pe->list) {
            seq += S(expr);
          }
          seq += "]";
        }
        if (m->value) {
          seq += PrintDirective::kSpace;
          seq += ":=";
          seq += PrintDirective::kSpace;
          seq += S(m->value);
        }
      });
    }

    case ast::NodeKind::CompositeLiteral: {
      const auto* m = n->As<ast::nodes::CompositeLiteral>();
      return NewSequence(Sequence::Attribute::kGrouped, [&](auto& seq) {
        seq += "{";
        if (!m->list.empty()) {
          seq += PrintDirective::kSpaceOrLine;
          Join(seq, m->list, NewSequence(Sequence::Attribute::kNone, [](auto& sseq) {
                 sseq += ",";
                 sseq += PrintDirective::kSpaceOrLine;
               }));
          seq += PrintDirective::kSpaceOrLine;
        }
        seq += "}";
      });
    }

    case ast::NodeKind::ValueLiteral: {
      const auto* m = n->As<ast::nodes::ValueLiteral>();
      return m->tok.On(src_);
    }

    case ast::NodeKind::StructSpec: {
      const auto* m = n->As<ast::nodes::StructSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        seq += "{";
        seq += PrintDirective::kHardLine;
        seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& fseq) {
          Join(fseq, m->fields, NewSequence(Sequence::Attribute::kNone, [](auto& sseq) {
                 sseq += ",";
                 sseq += PrintDirective::kHardLine;
               }));
        });
        seq += PrintDirective::kHardLine;
        seq += "}";
      });
      break;
    }
    case ast::NodeKind::MapSpec: {
      const auto* m = n->As<ast::nodes::MapSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "map";
        seq += PrintDirective::kSpace;
        seq += "from";
        seq += PrintDirective::kSpace;
        seq += S(m->from);
        seq += PrintDirective::kSpace;
        seq += "to";
        seq += PrintDirective::kSpace;
        seq += S(m->to);
      });
      break;
    }
    case ast::NodeKind::BehaviourSpec: {
      const auto* m = n->As<ast::nodes::BehaviourSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        seq += PrintDirective::kSpace;
        seq += S(m->params);
        if (m->runs_on) {
          seq += PrintDirective::kSpace;
          seq += S(m->runs_on);
        }
        if (m->system) {
          seq += PrintDirective::kSpace;
          seq += S(m->system);
        }
        if (m->ret) {
          seq += PrintDirective::kSpace;
          seq += S(m->ret);
        }
      });
      break;
    }
    case ast::NodeKind::TemplateDecl: {
      const auto* m = n->As<ast::nodes::TemplateDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "template";
        if (m->restriction) {
          seq += PrintDirective::kSpace;
          seq += S(m->restriction);
        }
        if (m->modif) {
          seq += PrintDirective::kSpace;
          seq += S(m->modif);
        }
        seq += PrintDirective::kSpace;
        seq += S(m->type);
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        if (m->params) {
          seq += S(m->params);
        }
        if (m->base) {
          seq += PrintDirective::kSpace;
          seq += "modifies";
          seq += PrintDirective::kSpace;
          seq += S(m->base);
        }
        seq += PrintDirective::kSpace;
        seq += ":=";
        seq += PrintDirective::kSpace;
        seq += S(m->value);
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::ModuleParameterGroup: {
      const auto* m = n->As<ast::nodes::ModuleParameterGroup>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "modulepar";
        seq += PrintDirective::kSpace;
        seq += "{";
        if (!m->decls.empty()) {
          seq += PrintDirective::kSpaceOrLine;
          seq += NewSequence(Sequence::Attribute::kGrouped, [&](auto& dseq) {
            Join(dseq, m->decls, NewSequence(Sequence::Attribute::kNone, [](auto& sseq) {
                   sseq += PrintDirective::kSemicolon;
                   sseq += PrintDirective::kHardLine;
                 }));
            dseq += PrintDirective::kSemicolon;
            dseq += PrintDirective::kSpaceOrLine;
          });
        }
        seq += "}";
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::ConstructorDecl: {
      const auto* m = n->As<ast::nodes::ConstructorDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "constructor";
        seq += "(";
        seq += S(m->params);
        seq += ")";
        if (m->body) {
          seq += PrintDirective::kSpace;
          seq += S(m->body);
        }
      });
      break;
    }
    case ast::NodeKind::SignatureDecl: {
      const auto* m = n->As<ast::nodes::SignatureDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "signature";
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        seq += S(m->params);
        if (m->noblock) {
          seq += PrintDirective::kSpace;
          seq += "noblock";
        }
        if (m->ret) {
          seq += PrintDirective::kSpace;
          seq += S(m->ret);
        }
        if (m->exception) {
          seq += PrintDirective::kSpace;
          seq += "exception";
          seq += PrintDirective::kSpace;
          seq += S(m->exception);
        }
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::BranchStmt: {
      const auto* m = n->As<ast::nodes::BranchStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        if (m->label) {
          seq += PrintDirective::kSpace;
          seq += S(m->label);
        }
      });
      break;
    }
    case ast::NodeKind::ReturnStmt: {
      const auto* m = n->As<ast::nodes::ReturnStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "return";
        if (m->result) {
          seq += PrintDirective::kSpace;
          seq += S(m->result);
        }
      });
      break;
    }
    case ast::NodeKind::AltStmt: {
      const auto* m = n->As<ast::nodes::AltStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        if (m->no_default) {
          seq += PrintDirective::kSpace;
          seq += S(m->no_default);
        }
        seq += PrintDirective::kSpace;
        seq += S(m->body);
      });
      break;
    }
    case ast::NodeKind::CallStmt: {
      const auto* m = n->As<ast::nodes::CallStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->stmt);
        seq += S(m->body);
      });
      break;
    }
    case ast::NodeKind::ForStmt: {
      const auto* m = n->As<ast::nodes::ForStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "for";
        seq += PrintDirective::kSpace;
        seq += "(";
        seq += S(m->init);
        seq += PrintDirective::kSemicolon;
        seq += PrintDirective::kSpace;
        seq += S(m->cond);
        seq += PrintDirective::kSemicolon;
        seq += PrintDirective::kSpace;
        seq += S(m->post->expr);
        seq += ")";
        seq += PrintDirective::kSpace;
        seq += S(m->body);
      });
      break;
    }
    case ast::NodeKind::ForRangeStmt: {
      const auto* m = n->As<ast::nodes::ForRangeStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "for";
        seq += PrintDirective::kSpace;
        seq += "(";
        seq += S(m->init);
        seq += PrintDirective::kSpace;
        seq += "in";
        seq += PrintDirective::kSpace;
        seq += S(m->range);
        seq += ")";
        seq += PrintDirective::kSpace;
        seq += S(m->body);
      });
      break;
    }
    case ast::NodeKind::WhileStmt: {
      const auto* m = n->As<ast::nodes::WhileStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "while";
        seq += PrintDirective::kSpace;
        seq += S(m->cond);
        seq += PrintDirective::kSpace;
        seq += S(m->body);
      });
      break;
    }
    case ast::NodeKind::DoWhileStmt: {
      const auto* m = n->As<ast::nodes::DoWhileStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "do";
        seq += PrintDirective::kSpace;
        seq += S(m->body);
        seq += PrintDirective::kSpace;
        seq += "while";
        seq += PrintDirective::kSpace;
        seq += S(m->cond);
      });
      break;
    }
    case ast::NodeKind::IfStmt: {
      const auto* m = n->As<ast::nodes::IfStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "if";
        seq += PrintDirective::kSpace;
        seq += "(";
        seq += S(m->cond);
        seq += ")";
        seq += PrintDirective::kSpace;
        seq += S(m->consequent);
        if (m->alternate) {
          seq += PrintDirective::kSpace;
          seq += "else";
          seq += PrintDirective::kSpace;
          seq += S(m->alternate);
        }
      });
      break;
    }
    case ast::NodeKind::SelectStmt: {
      const auto* m = n->As<ast::nodes::SelectStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "select";
        if (m->is_union) {
          seq += PrintDirective::kSpace;
          seq += "union";
        }
        seq += PrintDirective::kSpace;
        seq += "(";
        seq += S(m->tag);
        seq += ")";
        seq += PrintDirective::kHardLine;
        seq += "{";
        seq += PrintDirective::kHardLine;
        seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& cseq) {
          Join(cseq, m->clauses, PrintDirective::kHardLine);
        });
        seq += PrintDirective::kHardLine;
        seq += "}";
      });
      break;
    }
    case ast::NodeKind::CaseClause: {
      const auto* m = n->As<ast::nodes::CaseClause>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "case";
        seq += PrintDirective::kSpace;
        // todo: "case else" proper chk
        if (m->cond.empty()) {
          seq += "else";
        } else {
          seq += "(";
          Join(seq, m->cond, ", ");
          seq += ")";
        }
        seq += PrintDirective::kSpace;
        seq += S(m->body);
      });
      break;
    }
    case ast::NodeKind::CommClause: {
      const auto* m = n->As<ast::nodes::CommClause>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "[";
        if (m->x) {
          seq += S(m->x);
        }
        seq += "]";
        seq += PrintDirective::kSpace;
        seq += S(m->comm->expr);
        if (m->body) {
          seq += PrintDirective::kSpace;
          seq += S(m->body);
        } else {
          seq += PrintDirective::kSemicolon;
        }
      });
      break;
    }
    case ast::NodeKind::LanguageSpec: {
      const auto* m = n->As<ast::nodes::LanguageSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "language";
        seq += PrintDirective::kSpace;
        Join(seq, m->list, ", ");
      });
      break;
    }
    case ast::NodeKind::WithSpec: {
      const auto* m = n->As<ast::nodes::WithSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "with";
        seq += PrintDirective::kSpace;
        seq += "{";
        if (!m->list.empty()) {
          seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& cseq) {
            cseq += PrintDirective::kHardLine;
            Join(cseq, m->list, PrintDirective::kHardLine);
            if (m->list.back()->value->On(src_).ends_with("\n\"")) {
              cseq += PrintDirective::kSpace;
            } else {
              cseq += PrintDirective::kHardLine;
            }
          });
        }
        seq += "}";
      });
      break;
    }
    case ast::NodeKind::WithStmt: {
      const auto* m = n->As<ast::nodes::WithStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        if (m->overrides) {
          // TODO: CHECK PARSER
          seq += PrintDirective::kSpace;
          seq += "override";
        }
        if (!m->list.empty()) {
          seq += PrintDirective::kSpace;
          seq += "(";
          Join(seq, m->list, ", ");
          seq += ")";
        }
        seq += PrintDirective::kSpace;
        seq += S(m->value);
      });
      break;
    }
    case ast::NodeKind::SelectorExpr: {
      const auto* m = n->As<ast::nodes::SelectorExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->x);
        seq += ".";
        seq += S(m->sel);
      });
      break;
    }
    case ast::NodeKind::DefKindExpr: {
      const auto* m = n->As<ast::nodes::DefKindExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        seq += PrintDirective::kSpace;
        Join(seq, m->list, ", ");
      });
      break;
    }
    case ast::NodeKind::ExceptExpr: {
      const auto* m = n->As<ast::nodes::ExceptExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->x);
        Join(seq, m->list, ", ");
      });
      break;
    }
    case ast::NodeKind::FromExpr: {
      const auto* m = n->As<ast::nodes::FromExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        seq += PrintDirective::kSpace;
        seq += S(m->from);
        seq += PrintDirective::kSpace;
        seq += S(m->x);
      });
      break;
    }
    case ast::NodeKind::ModifiesExpr: {
      const auto* m = n->As<ast::nodes::ModifiesExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "modifies";
        seq += PrintDirective::kSpace;
        seq += S(m->x);
        seq += PrintDirective::kSpace;
        seq += ":=";
        seq += PrintDirective::kSpace;
        seq += S(m->y);
      });
      break;
    }
    case ast::NodeKind::ParenExpr: {
      const auto* m = n->As<ast::nodes::ParenExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "(";
        seq += NewSequence(Sequence::Attribute::kGrouped, [&](auto& eseq) {
          Join(eseq, m->list, ", ");
        });
        seq += ")";
      });
      break;
    }
    case ast::NodeKind::PostExpr: {
      const auto* m = n->As<ast::nodes::PostExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->x);
        seq += S(m->op);
      });
      break;
    }
    case ast::NodeKind::BinaryExpr: {
      const auto* m = n->As<ast::nodes::BinaryExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        const bool spacing = m->op.kind != ast::TokenKind::COLON && m->op.kind != ast::TokenKind::RANGE;
        seq += S(m->x);
        if (spacing) {
          seq += PrintDirective::kSpace;
        }
        seq += S(m->op);
        if (spacing) {
          seq += PrintDirective::kSpace;
        }
        seq += S(m->y);
      });
      break;
    }
    case ast::NodeKind::UnaryExpr: {
      const auto* m = n->As<ast::nodes::UnaryExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->op);
        if (m->op.range.Length() > 1) {
          seq += PrintDirective::kSpace;
        }
        seq += S(m->x);
      });
      break;
    }
    case ast::NodeKind::ValueExpr: {
      const auto* m = n->As<ast::nodes::ValueExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->x);
        seq += S(m->y);
      });
      break;
    }
    case ast::NodeKind::ParamExpr: {
      const auto* m = n->As<ast::nodes::ParamExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->x);
        seq += S(m->y);
      });
      break;
    }
    case ast::NodeKind::ImportDecl: {
      const auto* m = n->As<ast::nodes::ImportDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "import";
        seq += PrintDirective::kSpace;
        seq += "from";
        seq += PrintDirective::kSpace;
        seq += S(m->module);
        if (m->language) {
          seq += PrintDirective::kSpace;
          seq += S(m->language);
        }
        seq += PrintDirective::kSpace;
        if (m->list.size() == 1 && m->list[0]->kind.range.Length() == 0) {
          // TODO: improve parser in this aspect
          seq += "all";
        } else {
          seq += "{";
          seq += PrintDirective::kSpace;
          seq += NewSequence(Sequence::Attribute::kGrouped, [&](auto& fseq) {
            Join(fseq, m->list, NewSequence(Sequence::Attribute::kNone, [](auto& sseq) {
                   sseq += PrintDirective::kSemicolon;
                   sseq += PrintDirective::kSpaceOrLine;
                 }));
          });
          seq += PrintDirective::kSpace;
          seq += "}";
        }
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::GroupDecl: {
      const auto* m = n->As<ast::nodes::GroupDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "group";
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& fseq) {
          Join(fseq, m->defs, NewSequence(Sequence::Attribute::kNone, [](auto& sseq) {
                 sseq += PrintDirective::kSemicolon;
                 sseq += PrintDirective::kHardLine;
               }));
        });
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::FriendDecl: {
      const auto* m = n->As<ast::nodes::FriendDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "friend module";
        seq += PrintDirective::kSpace;
        seq += S(m->module);
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::ClassTypeDecl: {
      const auto* m = n->As<ast::nodes::ClassTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->external) {
          seq += "external";
          seq += PrintDirective::kSpace;
        }
        seq += "type";
        seq += PrintDirective::kSpace;
        seq += "class";
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        if (!m->extends.empty()) {
          seq += PrintDirective::kSpace;
          seq += "extends";
          seq += PrintDirective::kSpace;
          Join(seq, m->extends, ", ");
        }
        if (m->runs_on) {
          seq += PrintDirective::kSpace;
          seq += S(m->runs_on);
        }
        if (m->mtc) {
          seq += PrintDirective::kSpace;
          seq += S(m->mtc);
        }
        if (m->system) {
          seq += PrintDirective::kSpace;
          seq += S(m->system);
        }
        seq += PrintDirective::kHardLine;
        seq += "{";
        seq += PrintDirective::kHardLine;
        seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& iseq) {
          Join(iseq, m->defs, PrintDirective::kHardLine);
        });
        seq += PrintDirective::kHardLine;
        seq += "}";
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::MapTypeDecl: {
      const auto* m = n->As<ast::nodes::MapTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "map";
        seq += PrintDirective::kSpace;
        seq += S(m->spec);
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::EnumTypeDecl: {
      const auto* m = n->As<ast::nodes::EnumTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "type";
        seq += PrintDirective::kSpace;
        seq += "enumerated";
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        seq += PrintDirective::kHardLine;
        seq += "{";
        seq += PrintDirective::kHardLine;
        seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& vseq) {
          const auto count = std::size(m->values);
          std::size_t i{0};
          for (const auto& ev : m->values) {
            switch (ev->nkind) {
              case ast::NodeKind::CallExpr: {
                const auto* ce = ev->As<ast::nodes::CallExpr>();
                vseq += S(ce->fun);
                vseq += PrintDirective::kSpace;
                vseq += S(ce->args);
                break;
              }
              default: {
                vseq += S(ev);
                break;
              }
            }
            if (i + 1 != count) {
              vseq += ",";
              vseq += PrintDirective::kHardLine;
            }
            ++i;
          }
        });
        seq += PrintDirective::kHardLine;
        seq += "}";
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::EnumSpec: {
      const auto* m = n->As<ast::nodes::EnumSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "enumerated";
        seq += PrintDirective::kSpace;
        seq += PrintDirective::kSpace;
        seq += "{";
        seq += PrintDirective::kHardLine;
        seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& vseq) {
          const auto count = std::size(m->values);
          std::size_t i{0};
          for (const auto& ev : m->values) {
            switch (ev->nkind) {
              case ast::NodeKind::CallExpr: {
                const auto* ce = ev->As<ast::nodes::CallExpr>();
                vseq += S(ce->fun);
                vseq += PrintDirective::kSpace;
                vseq += S(ce->args);
                break;
              }
              default: {
                vseq += S(ev);
                break;
              }
            }
            if (i + 1 != count) {
              vseq += ",";
              vseq += PrintDirective::kHardLine;
            }
            ++i;
          }
        });
        seq += PrintDirective::kHardLine;
        seq += "}";
      });
      break;
    }
    case ast::NodeKind::BehaviourTypeDecl: {
      const auto* m = n->As<ast::nodes::BehaviourTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "type";
        seq += PrintDirective::kSpace;
        seq += S(m->kind);
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        seq += S(m->params);
        if (m->runs_on) {
          seq += PrintDirective::kSpace;
          seq += S(m->runs_on);
        }
        if (m->system) {
          seq += PrintDirective::kSpace;
          seq += S(m->system);
        }
        if (m->ret) {
          seq += PrintDirective::kSpace;
          seq += S(m->ret);
        }
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::PortTypeDecl: {
      const auto* m = n->As<ast::nodes::PortTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "type";
        seq += PrintDirective::kSpace;
        seq += "port";
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        seq += PrintDirective::kSpace;
        seq += S(m->kind);
        if (m->realtime) {
          seq += PrintDirective::kSpace;
          seq += "realtime";
        }
        if (!m->attrs.empty()) {
          seq += NewSequence(Sequence::Attribute::kGrouped, [&](auto& dseq) {
            dseq += PrintDirective::kSpaceOrLine;
            Join(dseq, m->attrs, NewSequence(Sequence::Attribute::kNone, [](auto& sseq) {
                   sseq += PrintDirective::kSemicolon;
                   sseq += PrintDirective::kHardLine;
                 }));
            dseq += PrintDirective::kSemicolon;
            dseq += PrintDirective::kSpaceOrLine;
          });
        }
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::PortAttribute: {
      const auto* m = n->As<ast::nodes::PortAttribute>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        seq += PrintDirective::kSpace;
        Join(seq, m->types, ", ");
      });
      break;
    }
    case ast::NodeKind::PortMapAttribute: {
      const auto* m = n->As<ast::nodes::PortMapAttribute>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        seq += PrintDirective::kSpace;
        seq += "param";
        seq += PrintDirective::kSpace;
        seq += S(m->params);
      });
      break;
    }
    case ast::NodeKind::ComponentTypeDecl: {
      const auto* m = n->As<ast::nodes::ComponentTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "type";
        seq += PrintDirective::kSpace;
        seq += "component";
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        if (!m->extends.empty()) {
          seq += PrintDirective::kSpace;
          seq += "extends";
          seq += PrintDirective::kSpace;
          Join(seq, m->extends, ", ");
        }
        seq += PrintDirective::kSpace;
        seq += S(m->body);
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::FormalPar: {
      const auto* m = n->As<ast::nodes::FormalPar>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->direction) {
          seq += S(m->direction);
          seq += PrintDirective::kSpace;
        }
        if (m->restriction) {
          seq += S(m->restriction);
          seq += PrintDirective::kSpace;
        }
        if (m->modif) {
          seq += S(m->modif);
          seq += PrintDirective::kSpace;
        }
        seq += S(m->type);
        seq += PrintDirective::kSpace;
        seq += S(m->name);
        for (const auto* pe : m->arraydef) {
          seq += "[";
          // TODO: check why it is being parsed as a list
          for (const auto* expr : pe->list) {
            seq += S(expr);
          }
          seq += "]";
        }
        if (m->value) {
          seq += PrintDirective::kSpace;
          seq += ":=";
          seq += PrintDirective::kSpace;
          seq += S(m->value);
        }
      });
      break;
    }
    case ast::NodeKind::LengthExpr: {
      const auto* m = n->As<ast::nodes::LengthExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->x) {
          seq += S(m->x);
          seq += PrintDirective::kSpace;
        }
        seq += "length";
        seq += PrintDirective::kSpace;
        seq += S(m->size);
      });
      break;
    }
    case ast::NodeKind::RunsOnSpec: {
      const auto* m = n->As<ast::nodes::RunsOnSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "runs on";
        seq += PrintDirective::kSpace;
        seq += S(m->comp);
      });
      break;
    }
    case ast::NodeKind::SystemSpec: {
      const auto* m = n->As<ast::nodes::SystemSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "system";
        seq += PrintDirective::kSpace;
        seq += S(m->comp);
      });
      break;
    }
    case ast::NodeKind::MtcSpec: {
      const auto* m = n->As<ast::nodes::MtcSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "mtc";
        seq += PrintDirective::kSpace;
        seq += S(m->comp);
      });
      break;
    }
    case ast::NodeKind::ReturnSpec: {
      const auto* m = n->As<ast::nodes::ReturnSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "return";
        if (m->restriction) {
          seq += PrintDirective::kSpace;
          seq += S(m->restriction);
        }
        if (m->modif) {
          seq += PrintDirective::kSpace;
          seq += S(m->modif);
        }
        seq += PrintDirective::kSpace;
        seq += S(m->type);
      });
      break;
    }
    case ast::NodeKind::RestrictionSpec: {
      const auto* m = n->As<ast::nodes::RestrictionSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->is_template) {
          seq += "template";
        }
        if (m->type.kind != ast::TokenKind::kSentinel) {
          if (m->is_template) {
            seq += PrintDirective::kSpace;
            seq += "(";
          }
          seq += S(m->type);
          if (m->is_template) {
            seq += ")";
          }
        }
      });
      break;
    }
    case ast::NodeKind::IndexExpr: {
      const auto* m = n->As<ast::nodes::IndexExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->x);
        seq += "[";
        seq += S(m->index);
        seq += "]";
      });
      break;
    }
    case ast::NodeKind::CallExpr: {
      const auto* m = n->As<ast::nodes::CallExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->fun);
        seq += S(m->args);
      });
      break;
    }
    case ast::NodeKind::RedirectExpr: {
      const auto* m = n->As<ast::nodes::RedirectExpr>();
      return NewSequence(Sequence::Attribute::kGrouped, [&](auto& seq) {
        seq += S(m->x);
        seq += PrintDirective::kSpaceOrLine;
        seq += "->";
        if (!m->value.empty()) {
          seq += PrintDirective::kSpace;
          seq += "value";
          seq += PrintDirective::kSpace;
          Join(seq, m->value, ", ");
        }
        if (!m->param.empty()) {
          seq += PrintDirective::kSpace;
          seq += "param";
          seq += PrintDirective::kSpace;
          Join(seq, m->param, ", ");
        }
        if (m->sender) {
          seq += PrintDirective::kSpace;
          seq += "sender";
          seq += PrintDirective::kSpace;
          seq += S(m->sender);
        }
        if (m->to_index) {
          seq += PrintDirective::kSpace;
          seq += "@index";
          seq += PrintDirective::kSpace;
          seq += S(m->to_index);
        }
        if (m->timestamp) {
          seq += PrintDirective::kSpace;
          seq += "timestamp";
          seq += PrintDirective::kSpace;
          seq += S(m->timestamp);
        }
      });
      break;
    }
    case ast::NodeKind::RedirectToIndex: {
      const auto* m = n->As<ast::nodes::RedirectToIndex>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->value) {
          seq += "value";
          seq += PrintDirective::kSpace;
        }
        seq += S(m->index);
      });
      break;
    }
    case ast::NodeKind::ParametrizedIdent: {
      const auto* m = n->As<ast::nodes::ParametrizedIdent>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->ident);
        seq += S(m->params);
      });
      break;
    }
    case ast::NodeKind::RegexpExpr: {
      const auto* m = n->As<ast::nodes::RegexpExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "regexp";
        if (m->nocase) {
          seq += PrintDirective::kSpace;
          seq += S(m->nocase);
        }
        seq += PrintDirective::kSpace;
        seq += S(m->x);
      });
      break;
    }
    case ast::NodeKind::PatternExpr: {
      const auto* m = n->As<ast::nodes::PatternExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "pattern";
        if (m->nocase) {
          seq += PrintDirective::kSpace;
          seq += S(m->nocase);
        }
        seq += PrintDirective::kSpace;
        seq += S(m->x);
      });
      break;
    }
    case ast::NodeKind::DecodedExpr: {
      const auto* m = n->As<ast::nodes::DecodedExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->params);
        seq += S(m->x);
      });
      break;
    }
    case ast::NodeKind::DynamicExpr: {
      const auto* m = n->As<ast::nodes::DynamicExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->body);
      });
      break;
    }
    case ast::NodeKind::DecmatchExpr: {
      const auto* m = n->As<ast::nodes::DecmatchExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "decmatch";
        if (m->params) {
          seq += PrintDirective::kSpace;
          seq += S(m->params);
        }
        seq += PrintDirective::kSpace;
        seq += S(m->x);
      });
      break;
    }
    case ast::NodeKind::ControlPart: {
      const auto* m = n->As<ast::nodes::ControlPart>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "control";
        seq += PrintDirective::kSpace;
        seq += S(m->body);
        if (m->with) {
          seq += PrintDirective::kSpace;
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::AssignmentExpr: {
      const auto* m = n->As<ast::nodes::AssignmentExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->property);
        seq += PrintDirective::kSpace;
        seq += ":=";
        seq += PrintDirective::kSpace;
        seq += S(m->value);
      });
      break;
    }

    default: {
      VANADIUM_DEBUG_ASSERT("Unhandled node '{}'", magic_enum::enum_name(n->nkind));
      return n->nrange.String(src_);
    }
  }
}

}  // namespace

Unit SerializeAst(std::string_view src, const ast::Node* n, lib::Arena& arena) {
  return AstSerializer(src, arena).Serialize(n);
}

}  // namespace vanadium::format
