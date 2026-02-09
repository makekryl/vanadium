#include "vanadium/format/AstSerializer.h"

#include <optional>
#include <ranges>
#include <string_view>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/lib/Arena.h>
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

  void Join(Sequence& target, std::ranges::range auto items, Unit separator) {
    const auto count = std::size(items);
    for (auto [i, n] : std::views::enumerate(items)) {
      target += S(n);
      if (i + 1 != count) {
        target += separator;
      }
    }
  }

  std::string_view src_;
  lib::Arena& arena_;
};

Unit AstSerializer::S(const ast::Node* n) {  // NOLINT(readability-function-size)
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

    case ast::NodeKind::Module: {
      const auto* m = n->As<ast::nodes::Module>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "module";
        seq += " ";
        seq += S(m->name);
        if (m->language) {
          seq += " ";
          seq += S(m->language);
        }
        seq += " ";
        seq += PrintDirective::kHardLine;
        seq += "{";
        seq += PrintDirective::kHardLine;
        seq += PrintDirective::kHardLine;
        Join(seq, m->defs, PrintDirective::kHardLine);
        seq += PrintDirective::kHardLine;
        seq += PrintDirective::kHardLine;
        seq += "}";
        if (m->with) {
          seq += " ";
          seq += S(m->with);
        }
      });
    }

    case ast::NodeKind::Definition: {
      const auto* m = n->As<ast::nodes::Definition>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->visibility) {
          seq += S(m->visibility);
          seq += " ";
        }
        seq += S(m->def);
        switch (m->def->nkind) {
          case ast::NodeKind::StructTypeDecl:
          case ast::NodeKind::PortTypeDecl:
          case ast::NodeKind::FuncDecl:
            break;
          default:
            seq += ";";
        }
      });
    }

    case ast::NodeKind::StructTypeDecl: {
      const auto* m = n->As<ast::nodes::StructTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "type";
        seq += " ";
        seq += S(m->kind);
        seq += " ";
        seq += S(m->name);
        seq += " ";
        seq += PrintDirective::kHardLine;
        seq += "{";
        seq += PrintDirective::kHardLine;
        seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& fseq) {
          Join(fseq, m->fields, NewSequence(Sequence::Attribute::kNone, [&](auto& sseq) {
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
        seq += " ";
        seq += S(m->name);
        if (m->pars) {
          seq += S(m->pars);
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
          seq += " ";
          seq += S(m->length);
        }
        if (m->value_constraint) {
          seq += " ";
          seq += S(m->value_constraint);
        }
        if (m->optional) {
          seq += " ";
          seq += "optional";
        }
      });
    }

    case ast::NodeKind::SubTypeDecl: {
      const auto* m = n->As<ast::nodes::SubTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "type";
        seq += " ";
        seq += S(m->field);
        if (m->with) {
          seq += " ";
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
        seq += " ";
        seq += "of";
        if (m->length) {
          seq += " ";
          seq += S(m->length);
        }
        seq += " ";
        seq += S(m->elemtype);
      });
    }

    case ast::NodeKind::FuncDecl: {
      const auto* m = n->As<ast::nodes::FuncDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->external) {
          seq += "external";
          seq += " ";
        }
        seq += S(m->kind);
        if (m->modif) {
          seq += " ";
          seq += S(m->modif);
        }
        seq += " ";
        seq += S(m->name);
        if (m->pars) {
          seq += "<";
          seq += S(m->pars);
          seq += ">";
        }
        if (m->params) {
          seq += "(";
          seq += S(m->params);
          seq += ")";
        }
        if (m->runs_on) {
          seq += " ";
          seq += S(m->runs_on);
        }
        if (m->mtc) {
          seq += " ";
          seq += S(m->runs_on);
        }
        if (m->system) {
          seq += " ";
          seq += S(m->system);
        }
        if (m->ret) {
          seq += " ";
          seq += S(m->ret);
        }

        if (!m->external) {
          seq += PrintDirective::kHardLine;
          seq += S(m->body);
          seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& bseq) {});
        }
      });
    }

    case ast::NodeKind::FormalPars: {
      const auto* m = n->As<ast::nodes::FormalPars>();
      return NewSequence(Sequence::Attribute::kGrouped, [&](auto& seq) {
        seq += PrintDirective::kSoftLine;
        Join(seq, m->list, NewSequence(Sequence::Attribute::kNone, [&](auto& sseq) {
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
        seq += PrintDirective::kHardLine;
        seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& iseq) {
          Join(iseq, m->stmts, NewSequence(Sequence::Attribute::kNone, [&](auto& sseq) {
                 sseq += ";";
                 sseq += PrintDirective::kHardLine;
               }));
          if (!m->stmts.empty()) {
            iseq += ";";
          }
        });
        seq += PrintDirective::kHardLine;
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
          seq += " ";
        }
        if (m->template_restriction) {
          seq += S(m->template_restriction);
          seq += " ";
        }
        if (m->modif) {
          seq += S(m->modif);
          seq += " ";
        }
        seq += S(m->type);
        seq += " ";
        seq += NewSequence(Sequence::Attribute::kGrouped, [&](auto& dseq) {
          Join(dseq, m->decls, NewSequence(Sequence::Attribute::kNone, [&](auto& sseq) {
                 sseq += ",";
                 sseq += PrintDirective::kSpaceOrLine;
               }));
        });
        if (m->with) {
          seq += " ";
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
          seq += " ";
          seq += ":=";
          seq += " ";
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
          Join(seq, m->list, NewSequence(Sequence::Attribute::kNone, [&](auto& sseq) {
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
          Join(fseq, m->fields, NewSequence(Sequence::Attribute::kNone, [&](auto& sseq) {
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
        seq += S(m->from);
        seq += S(m->to);
      });
      break;
    }
    // case ast::NodeKind::EnumSpec: {
    //   const auto* m = n->As<ast::nodes::EnumSpec>();
    //   return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
    //     seq += S(m->values);
    //   });
    //   break;
    // }
    case ast::NodeKind::BehaviourSpec: {
      const auto* m = n->As<ast::nodes::BehaviourSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        seq += S(m->params);
        seq += S(m->runs_on);
        seq += S(m->system);
        seq += S(m->ret);
      });
      break;
    }
    case ast::NodeKind::TemplateDecl: {
      const auto* m = n->As<ast::nodes::TemplateDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "template";
        if (m->restriction) {
          seq += " ";
          seq += S(m->restriction);
        }
        if (m->modif) {
          seq += " ";
          seq += S(m->modif);
        }
        seq += " ";
        seq += S(m->type);
        seq += " ";
        seq += S(m->name);
        if (m->pars) {
          seq += S(m->pars);
        }
        if (m->params) {
          seq += S(m->params);
        }
        if (m->base) {
          seq += " ";
          seq += "modifies";
          seq += " ";
          seq += S(m->base);
        }
        seq += " ";
        seq += ":=";
        seq += " ";
        seq += S(m->value);
        if (m->with) {
          seq += " ";
          seq += S(m->with);
        }
      });
      break;
    }
    // case ast::NodeKind::ModuleParameterGroup: {
    //   const auto* m = n->As<ast::nodes::ModuleParameterGroup>();
    //   return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
    //     seq += S(m->decls);
    //     seq += S(m->with);
    //   });
    //   break;
    // }
    case ast::NodeKind::ConstructorDecl: {
      const auto* m = n->As<ast::nodes::ConstructorDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->params);
        seq += S(m->body);
      });
      break;
    }
    // case ast::NodeKind::SignatureDecl: {
    //   const auto* m = n->As<ast::nodes::SignatureDecl>();
    //   return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
    //     seq += S(m->name);
    //     seq += S(m->pars);
    //     seq += S(m->params);
    //     seq += S(m->noblock);
    //     seq += S(m->ret);
    //     seq += S(m->exception);
    //     seq += S(m->with);
    //   });
    //   break;
    // }
    case ast::NodeKind::BranchStmt: {
      const auto* m = n->As<ast::nodes::BranchStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        if (m->label) {
          seq += " ";
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
          seq += " ";
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
          seq += " ";
          seq += S(m->no_default);
        }
        seq += " ";
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
        seq += " ";
        seq += "(";
        seq += S(m->init);
        seq += ";";
        seq += " ";
        seq += S(m->cond);
        seq += ";";
        seq += " ";
        seq += S(m->post);
        seq += ")";
        seq += " ";
        seq += S(m->body);
      });
      break;
    }
    case ast::NodeKind::ForRangeStmt: {
      const auto* m = n->As<ast::nodes::ForRangeStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->init);
        seq += S(m->range);
        seq += S(m->body);
      });
      break;
    }
    case ast::NodeKind::WhileStmt: {
      const auto* m = n->As<ast::nodes::WhileStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->cond);
        seq += S(m->body);
      });
      break;
    }
    case ast::NodeKind::DoWhileStmt: {
      const auto* m = n->As<ast::nodes::DoWhileStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->body);
        seq += S(m->cond);
      });
      break;
    }
    case ast::NodeKind::IfStmt: {
      const auto* m = n->As<ast::nodes::IfStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "if";
        seq += " ";
        seq += "(";
        seq += S(m->cond);
        seq += ")";
        seq += " ";
        seq += S(m->consequent);
        if (m->alternate) {
          seq += " ";
          seq += "else";
          seq += " ";
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
          seq += " ";
          seq += "union";
        }
        seq += " ";
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
        seq += " ";
        // todo: "case else" proper chk
        if (m->cond.empty()) {
          seq += "else";
        } else {
          seq += "(";
          Join(seq, m->cond, ", ");
          seq += ")";
        }
        seq += " ";
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
        seq += " ";
        seq += S(m->comm);
        if (m->body) {
          seq += " ";
          seq += S(m->body);
        }
      });
      break;
    }
    case ast::NodeKind::LanguageSpec: {
      const auto* m = n->As<ast::nodes::LanguageSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "language";
        seq += " ";
        Join(seq, m->list, ", ");
      });
      break;
    }
    case ast::NodeKind::WithSpec: {
      const auto* m = n->As<ast::nodes::WithSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "with";
        seq += " ";
        seq += "{";
        if (!m->list.empty()) {
          seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& cseq) {
            cseq += PrintDirective::kHardLine;
            Join(cseq, m->list, PrintDirective::kHardLine);
            if (m->list.back()->value->On(src_).ends_with("\n\"")) {
              cseq += " ";
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
          seq += " ";
          seq += "override";
        }
        if (!m->list.empty()) {
          seq += " ";
          seq += "(";
          Join(seq, m->list, ", ");
          seq += ")";
        }
        seq += " ";
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
        seq += " ";
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
        seq += " ";
        seq += S(m->from);
        seq += " ";
        seq += S(m->x);
      });
      break;
    }
    case ast::NodeKind::ModifiesExpr: {
      const auto* m = n->As<ast::nodes::ModifiesExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->x);
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
          seq += " ";
        }
        seq += S(m->op);
        if (spacing) {
          seq += " ";
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
          seq += " ";
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
        seq += " ";
        seq += "from";
        seq += " ";
        seq += S(m->module);
        if (m->language) {
          seq += " ";
          seq += S(m->language);
        }
        seq += " ";
        if (m->list.size() == 1 && m->list[0]->kind.range.Length() == 0) {
          // TODO: improve parser in this aspect
          seq += "all";
        } else {
          seq += "{";
          seq += " ";
          seq += NewSequence(Sequence::Attribute::kGrouped, [&](auto& fseq) {
            Join(fseq, m->list, NewSequence(Sequence::Attribute::kNone, [&](auto& sseq) {
                   sseq += ";";
                   sseq += PrintDirective::kSpaceOrLine;
                 }));
          });
          seq += " ";
          seq += "}";
        }
        if (m->with) {
          seq += " ";
          seq += S(m->with);
        }
      });
      break;
    }
    // case ast::NodeKind::GroupDecl: {
    //   const auto* m = n->As<ast::nodes::GroupDecl>();
    //   return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
    //     seq += S(m->name);
    //     seq += S(m->defs);
    //     seq += S(m->with);
    //   });
    //   break;
    // }
    case ast::NodeKind::FriendDecl: {
      const auto* m = n->As<ast::nodes::FriendDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "friend module";
        seq += " ";
        seq += S(m->module);
        if (m->with) {
          seq += " ";
          seq += S(m->with);
        }
      });
      break;
    }
    // case ast::NodeKind::ClassTypeDecl: {
    //   const auto* m = n->As<ast::nodes::ClassTypeDecl>();
    //   return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
    //     seq += S(m->external);
    //     seq += S(m->kind);
    //     seq += S(m->modif);
    //     seq += S(m->name);
    //     seq += S(m->extends);
    //     seq += S(m->runs_on);
    //     seq += S(m->mtc);
    //     seq += S(m->system);
    //     seq += S(m->defs);
    //     seq += S(m->with);
    //   });
    //   break;
    // }
    case ast::NodeKind::MapTypeDecl: {
      const auto* m = n->As<ast::nodes::MapTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->spec);
        seq += S(m->name);
        seq += S(m->pars);
        seq += S(m->with);
      });
      break;
    }
    // case ast::NodeKind::EnumTypeDecl: {
    //   const auto* m = n->As<ast::nodes::EnumTypeDecl>();
    //   return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
    //     seq += S(m->name);
    //     seq += S(m->pars);
    //     seq += S(m->values);
    //     seq += S(m->with);
    //   });
    //   break;
    // }
    case ast::NodeKind::BehaviourTypeDecl: {
      const auto* m = n->As<ast::nodes::BehaviourTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "type";
        seq += " ";
        seq += S(m->kind);
        seq += " ";
        seq += S(m->name);
        if (m->pars) {
          seq += " ";
          seq += S(m->pars);
        }
        seq += S(m->params);
        if (m->runs_on) {
          seq += " ";
          seq += S(m->runs_on);
        }
        if (m->system) {
          seq += " ";
          seq += S(m->system);
        }
        if (m->ret) {
          seq += " ";
          seq += S(m->ret);
        }
        if (m->with) {
          seq += " ";
          seq += S(m->with);
        }
      });
      break;
    }
    // case ast::NodeKind::PortTypeDecl: {
    //   const auto* m = n->As<ast::nodes::PortTypeDecl>();
    //   return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
    //     seq += S(m->name);
    //     seq += S(m->pars);
    //     seq += S(m->realtime);
    //     seq += S(m->attrs);
    //     seq += S(m->with);
    //   });
    //   break;
    // }
    // case ast::NodeKind::PortAttribute: {
    //   const auto* m = n->As<ast::nodes::PortAttribute>();
    //   return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
    //     seq += S(m->kind);
    //     seq += S(m->types);
    //   });
    //   break;
    // }
    case ast::NodeKind::PortMapAttribute: {
      const auto* m = n->As<ast::nodes::PortMapAttribute>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->kind);
        seq += S(m->params);
      });
      break;
    }
    // case ast::NodeKind::ComponentTypeDecl: {
    //   const auto* m = n->As<ast::nodes::ComponentTypeDecl>();
    //   return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
    //     seq += S(m->name);
    //     seq += S(m->pars);
    //     seq += S(m->extends);
    //     seq += S(m->body);
    //     seq += S(m->with);
    //   });
    //   break;
    // }
    case ast::NodeKind::FormalPar: {
      const auto* m = n->As<ast::nodes::FormalPar>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->direction) {
          seq += S(m->direction);
          seq += " ";
        }
        if (m->restriction) {
          seq += S(m->restriction);
          seq += " ";
        }
        if (m->modif) {
          seq += S(m->modif);
          seq += " ";
        }
        seq += S(m->type);
        seq += " ";
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
          seq += " ";
          seq += ":=";
          seq += " ";
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
          seq += " ";
        }
        seq += "length";
        seq += " ";
        seq += S(m->size);
      });
      break;
    }
    case ast::NodeKind::RunsOnSpec: {
      const auto* m = n->As<ast::nodes::RunsOnSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "runs on";
        seq += " ";
        seq += S(m->comp);
      });
      break;
    }
    case ast::NodeKind::SystemSpec: {
      const auto* m = n->As<ast::nodes::SystemSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "system";
        seq += " ";
        seq += S(m->comp);
      });
      break;
    }
    case ast::NodeKind::MtcSpec: {
      const auto* m = n->As<ast::nodes::MtcSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "mtc";
        seq += " ";
        seq += S(m->comp);
      });
      break;
    }
    case ast::NodeKind::ReturnSpec: {
      const auto* m = n->As<ast::nodes::ReturnSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "return";
        if (m->restriction) {
          seq += " ";
          seq += S(m->restriction);
        }
        if (m->modif) {
          seq += " ";
          seq += S(m->modif);
        }
        seq += " ";
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
            seq += " ";
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
          seq += " ";
          seq += "value";
          seq += " ";
          Join(seq, m->value, ", ");
        }
        if (!m->param.empty()) {
          seq += " ";
          seq += "param";
          seq += " ";
          Join(seq, m->param, ", ");
        }
        if (m->sender) {
          seq += " ";
          seq += "sender";
          seq += " ";
          seq += S(m->sender);
        }
        if (m->to_index) {
          seq += " ";
          seq += "@index";
          seq += " ";
          seq += S(m->to_index);
        }
        if (m->timestamp) {
          seq += " ";
          seq += "timestamp";
          seq += " ";
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
          seq += " ";
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
    // case ast::NodeKind::RegexpExpr: {
    //   const auto* m = n->As<ast::nodes::RegexpExpr>();
    //   return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
    //     seq += S(m->nocase);
    //     seq += S(m->x);
    //   });
    //   break;
    // }
    // case ast::NodeKind::PatternExpr: {
    //   const auto* m = n->As<ast::nodes::PatternExpr>();
    //   return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
    //     seq += S(m->nocase);
    //     seq += S(m->x);
    //   });
    //   break;
    // }
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
          seq += " ";
          seq += S(m->params);
        }
        seq += " ";
        seq += S(m->x);
      });
      break;
    }
    case ast::NodeKind::ControlPart: {
      const auto* m = n->As<ast::nodes::ControlPart>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += "control";
        seq += " ";
        seq += S(m->body);
        if (m->with) {
          seq += " ";
          seq += S(m->with);
        }
      });
      break;
    }
    case ast::NodeKind::AssignmentExpr: {
      const auto* m = n->As<ast::nodes::AssignmentExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        seq += S(m->property);
        seq += " ";
        seq += ":=";
        seq += " ";
        seq += S(m->value);
      });
      break;
    }

    default: {
      // todo: PANIC
      return magic_enum::enum_name(n->nkind);
    }
  }
}

}  // namespace

Unit SerializeAst(std::string_view src, const ast::Node* n, lib::Arena& arena) {
  return AstSerializer(src, arena).Serialize(n);
}

}  // namespace vanadium::format
