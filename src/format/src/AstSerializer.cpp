#include "vanadium/format/AstSerializer.h"

#include <initializer_list>
#include <optional>
#include <print>
#include <string_view>
#include <variant>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/Scanner.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/Assert.h>
#include <vanadium/lib/Metaprogramming.h>

namespace vanadium::format {

namespace {

class AstSerializer {
 public:
  AstSerializer(std::string_view src, lib::Arena& arena) : src_(src), arena_(arena) {}

  Unit Serialize(const ast::Node* n) {
    scanner_.emplace(src_, n->nrange.begin);
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
  void Join(Sequence& target, const std::vector<T>& items, std::initializer_list<Unit> separators) {
    for (std::size_t i = 0; i + 1 < items.size(); ++i) {
      A(target, S(items[i]));
      for (const auto& su : separators) {
        A(target, su);
      }
    }
    if (!items.empty()) {
      A(target, S(items.back()));
    }
  }
  template <typename T>
  void Join(Sequence& target, const std::vector<T>& items, Unit separator) {
    Join(target, items, {separator});
  }

  //

  void A(Sequence& seq, Unit u) {
    // todo: optimize
    if (std::holds_alternative<EmptyUnit>(u)) {
      return;
    }
    if (const auto* const s = std::get_if<std::string_view>(&u)) {
      auto tok = scanner_->Scan();
      if (tok.kind == ast::TokenKind::SEMICOLON) {
        tok = scanner_->Scan();
      }
      if (tok.kind == ast::TokenKind::COMMENT) {
        tok = scanner_->Scan();
      }
      std::println("s='{}' <-> tok='{}'", *s, tok.On(src_));
      seq.units.emplace_back(tok.On(src_));
      if (*s != tok.On(src_)) {
        std::println("mismatch {} <-> {}", *s, tok.On(src_));
        std::exit(2);
      }
    }
    seq.units.push_back(std::move(u));
  }

  //

  std::string_view src_;
  std::optional<ast::parser::Scanner> scanner_;
  lib::Arena& arena_;
};

Unit AstSerializer::S(const ast::Node* n) {  // NOLINT(readability-function-size)
  // NOLINTNEXTLINE(readability-identifier-naming)
  const auto S_pars = [this](Sequence& seq, const ast::nodes::FormalPars* pars) -> void {
    A(seq, "<");
    A(seq, S(pars));
    A(seq, ">");
  };

  switch (n->nkind) {
    case ast::NodeKind::RootNode: {
      const auto* m = n->As<ast::RootNode>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        m->Accept([&](const auto* cn) {
          A(seq, S(cn));
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
        A(seq, S(m->tok1));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->tok2));
      });
    }

    case ast::NodeKind::Module: {
      const auto* m = n->As<ast::nodes::Module>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "module");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->language) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->language));
        }
        A(seq, PrintDirective::kSpace);
        A(seq, PrintDirective::kHardLine);
        A(seq, "{");
        A(seq, PrintDirective::kHardLine);
        if (!m->defs.empty()) {
          if (m->defs.front()->def->nkind != ast::NodeKind::ImportDecl) {
            A(seq, PrintDirective::kHardLine);
          }
          Join(seq, m->defs, PrintDirective::kHardLine);
          A(seq, PrintDirective::kHardLine);
          A(seq, PrintDirective::kHardLine);
        }
        A(seq, "}");
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
    }

    case ast::NodeKind::Definition: {
      const auto* m = n->As<ast::nodes::Definition>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->visibility) {
          A(seq, S(m->visibility));
          A(seq, PrintDirective::kSpace);
        }
        A(seq, S(m->def));
        switch (m->def->nkind) {
          case ast::NodeKind::ImportDecl:
          case ast::NodeKind::SubTypeDecl:
          case ast::NodeKind::ValueDecl:
            A(seq, PrintDirective::kSemicolon);
            break;
          default:
            break;
        }
      });
    }

    case ast::NodeKind::StructTypeDecl: {
      const auto* m = n->As<ast::nodes::StructTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->kind));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        A(seq, PrintDirective::kSpace);
        A(seq, PrintDirective::kHardLine);
        A(seq, "{");
        A(seq, PrintDirective::kHardLine);
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& fseq) {
            Join(fseq, m->fields, {",", PrintDirective::kHardLine});
          }));
        A(seq, PrintDirective::kHardLine);
        A(seq, "}");
      });
    }

    case ast::NodeKind::Field: {
      const auto* m = n->As<ast::nodes::Field>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->type));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        for (const auto* pe : m->arraydef) {
          A(seq, "[");
          // TODO: check why it is being parsed as a list
          for (const auto* expr : pe->list) {
            A(seq, S(expr));
          }
          A(seq, "]");
        }
        if (m->length) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->length));
        }
        if (m->value_constraint) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->value_constraint));
        }
        if (m->optional) {
          A(seq, PrintDirective::kSpace);
          A(seq, "optional");
        }
      });
    }

    case ast::NodeKind::SubTypeDecl: {
      const auto* m = n->As<ast::nodes::SubTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->field));
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
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
        A(seq, S(m->kind));
        if (m->length) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->length));
        }
        A(seq, PrintDirective::kSpace);
        A(seq, "of");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->elemtype));
      });
    }

    case ast::NodeKind::FuncDecl: {
      const auto* m = n->As<ast::nodes::FuncDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->external) {
          A(seq, "external");
          A(seq, PrintDirective::kSpace);
        }
        A(seq, S(m->kind));
        if (m->modif) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->modif));
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        if (m->params) {
          A(seq, "(");
          A(seq, S(m->params));
          A(seq, ")");
        }
        if (m->runs_on) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->runs_on));
        }
        if (m->mtc) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->runs_on));
        }
        if (m->system) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->system));
        }
        if (m->ret) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->ret));
        }

        if (!m->external) {
          A(seq, PrintDirective::kHardLine);
          A(seq, S(m->body));
        }
      });
    }

    case ast::NodeKind::FormalPars: {
      const auto* m = n->As<ast::nodes::FormalPars>();
      return NewSequence(Sequence::Attribute::kGrouped, [&](auto& seq) {
        A(seq, PrintDirective::kSoftLine);
        Join(seq, m->list, {",", PrintDirective::kSpaceOrLine});
        A(seq, PrintDirective::kSoftLine);
      });
    }

    case ast::NodeKind::BlockStmt: {
      const auto* m = n->As<ast::nodes::BlockStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "{");
        if (!m->stmts.empty()) {
          A(seq, PrintDirective::kHardLine);
          A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& iseq) {
              const auto t_stmt = [&](const ast::nodes::Stmt* stmt) {
                A(iseq, S(stmt));
                switch (stmt->nkind) {
                  case ast::NodeKind::ReturnStmt:
                  case ast::NodeKind::BranchStmt:
                    A(iseq, PrintDirective::kSemicolon);
                    break;
                  default:
                    break;
                }
              };
              for (std::size_t i = 0; i + 1 < m->stmts.size(); ++i) {
                t_stmt(m->stmts[i]);
                A(iseq, PrintDirective::kHardLine);
                // todo: chk [i+1]
              }
              if (!m->stmts.empty()) {
                t_stmt(m->stmts.back());
              }
            }));
          A(seq, PrintDirective::kHardLine);
        } else {
          A(seq, PrintDirective::kSpace);
        }
        A(seq, "}");
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
          A(seq, S(m->kind));
          A(seq, PrintDirective::kSpace);
        }
        if (m->template_restriction) {
          A(seq, S(m->template_restriction));
          A(seq, PrintDirective::kSpace);
        }
        if (m->modif) {
          A(seq, S(m->modif));
          A(seq, PrintDirective::kSpace);
        }
        A(seq, S(m->type));
        A(seq, PrintDirective::kSpace);
        A(seq, NewSequence(Sequence::Attribute::kGrouped, [&](auto& dseq) {
            Join(dseq, m->decls, {",", PrintDirective::kSpaceOrLine});
          }));
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
    }

    case ast::NodeKind::Declarator: {
      const auto* m = n->As<ast::nodes::Declarator>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->name));
        for (const auto* pe : m->arraydef) {
          A(seq, "[");
          // TODO: check why it is being parsed as a list
          for (const auto* expr : pe->list) {
            A(seq, S(expr));
          }
          A(seq, "]");
        }
        if (m->value) {
          A(seq, PrintDirective::kSpace);
          A(seq, ":=");
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->value));
        }
      });
    }

    case ast::NodeKind::CompositeLiteral: {
      const auto* m = n->As<ast::nodes::CompositeLiteral>();
      return NewSequence(Sequence::Attribute::kGrouped, [&](auto& seq) {
        A(seq, "{");
        if (!m->list.empty()) {
          A(seq, PrintDirective::kSpaceOrLine);
          Join(seq, m->list, {",", PrintDirective::kSpaceOrLine});
          A(seq, PrintDirective::kSpaceOrLine);
        }
        A(seq, "}");
      });
    }

    case ast::NodeKind::ValueLiteral: {
      const auto* m = n->As<ast::nodes::ValueLiteral>();
      return m->tok.On(src_);
    }

    case ast::NodeKind::StructSpec: {
      const auto* m = n->As<ast::nodes::StructSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->kind));
        A(seq, "{");
        A(seq, PrintDirective::kHardLine);
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& fseq) {
            Join(fseq, m->fields, {",", PrintDirective::kHardLine});
          }));
        A(seq, PrintDirective::kHardLine);
        A(seq, "}");
      });
      break;
    }
    case ast::NodeKind::MapSpec: {
      const auto* m = n->As<ast::nodes::MapSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "map");
        A(seq, PrintDirective::kSpace);
        A(seq, "from");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->from));
        A(seq, PrintDirective::kSpace);
        A(seq, "to");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->to));
      });
      break;
    }
    case ast::NodeKind::BehaviourSpec: {
      const auto* m = n->As<ast::nodes::BehaviourSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->kind));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->params));
        if (m->runs_on) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->runs_on));
        }
        if (m->system) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->system));
        }
        if (m->ret) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->ret));
        }
      });
      break;
    }
    case ast::NodeKind::TemplateDecl: {
      const auto* m = n->As<ast::nodes::TemplateDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "template");
        if (m->restriction) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->restriction));
        }
        if (m->modif) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->modif));
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->type));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        if (m->params) {
          A(seq, S(m->params));
        }
        if (m->base) {
          A(seq, PrintDirective::kSpace);
          A(seq, "modifies");
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->base));
        }
        A(seq, PrintDirective::kSpace);
        A(seq, ":=");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->value));
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::ModuleParameterGroup: {
      const auto* m = n->As<ast::nodes::ModuleParameterGroup>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "modulepar");
        A(seq, PrintDirective::kSpace);
        A(seq, "{");
        if (!m->decls.empty()) {
          A(seq, PrintDirective::kSpaceOrLine);
          A(seq, NewSequence(Sequence::Attribute::kGrouped, [&](auto& dseq) {
              Join(dseq, m->decls, {PrintDirective::kSemicolon, PrintDirective::kHardLine});
              A(dseq, PrintDirective::kSemicolon);
              A(dseq, PrintDirective::kSpaceOrLine);
            }));
        }
        A(seq, "}");
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::ConstructorDecl: {
      const auto* m = n->As<ast::nodes::ConstructorDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "create");
        A(seq, "(");
        A(seq, S(m->params));
        A(seq, ")");
        if (m->body) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->body));
        }
      });
      break;
    }
    case ast::NodeKind::SignatureDecl: {
      const auto* m = n->As<ast::nodes::SignatureDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "signature");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        A(seq, S(m->params));
        if (m->noblock) {
          A(seq, PrintDirective::kSpace);
          A(seq, "noblock");
        }
        if (m->ret) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->ret));
        }
        if (m->exception) {
          A(seq, PrintDirective::kSpace);
          A(seq, "exception");
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->exception));
        }
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::BranchStmt: {
      const auto* m = n->As<ast::nodes::BranchStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->kind));
        if (m->label) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->label));
        }
      });
      break;
    }
    case ast::NodeKind::ReturnStmt: {
      const auto* m = n->As<ast::nodes::ReturnStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "return");
        if (m->result) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->result));
        }
      });
      break;
    }
    case ast::NodeKind::AltStmt: {
      const auto* m = n->As<ast::nodes::AltStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->kind));
        if (m->no_default) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->no_default));
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
      });
      break;
    }
    case ast::NodeKind::CallStmt: {
      const auto* m = n->As<ast::nodes::CallStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->stmt));
        A(seq, S(m->body));
      });
      break;
    }
    case ast::NodeKind::ForStmt: {
      const auto* m = n->As<ast::nodes::ForStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "for");
        A(seq, PrintDirective::kSpace);
        A(seq, "(");
        A(seq, S(m->init));
        A(seq, PrintDirective::kSemicolon);
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->cond));
        A(seq, PrintDirective::kSemicolon);
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->post->expr));
        A(seq, ")");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
      });
      break;
    }
    case ast::NodeKind::ForRangeStmt: {
      const auto* m = n->As<ast::nodes::ForRangeStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "for");
        A(seq, PrintDirective::kSpace);
        A(seq, "(");
        A(seq, S(m->init));
        A(seq, PrintDirective::kSpace);
        A(seq, "in");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->range));
        A(seq, ")");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
      });
      break;
    }
    case ast::NodeKind::WhileStmt: {
      const auto* m = n->As<ast::nodes::WhileStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "while");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->cond));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
      });
      break;
    }
    case ast::NodeKind::DoWhileStmt: {
      const auto* m = n->As<ast::nodes::DoWhileStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "do");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
        A(seq, PrintDirective::kSpace);
        A(seq, "while");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->cond));
      });
      break;
    }
    case ast::NodeKind::IfStmt: {
      const auto* m = n->As<ast::nodes::IfStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "if");
        A(seq, PrintDirective::kSpace);
        A(seq, "(");
        A(seq, S(m->cond));
        A(seq, ")");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->consequent));
        if (m->alternate) {
          A(seq, PrintDirective::kSpace);
          A(seq, "else");
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->alternate));
        }
      });
      break;
    }
    case ast::NodeKind::SelectStmt: {
      const auto* m = n->As<ast::nodes::SelectStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "select");
        if (m->is_union) {
          A(seq, PrintDirective::kSpace);
          A(seq, "union");
        }
        A(seq, PrintDirective::kSpace);
        A(seq, "(");
        A(seq, S(m->tag));
        A(seq, ")");
        A(seq, PrintDirective::kHardLine);
        A(seq, "{");
        A(seq, PrintDirective::kHardLine);
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& cseq) {
            Join(cseq, m->clauses, PrintDirective::kHardLine);
          }));
        A(seq, PrintDirective::kHardLine);
        A(seq, "}");
      });
      break;
    }
    case ast::NodeKind::CaseClause: {
      const auto* m = n->As<ast::nodes::CaseClause>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "case");
        A(seq, PrintDirective::kSpace);
        // todo: "case else" proper chk
        if (m->cond.empty()) {
          A(seq, "else");
        } else {
          A(seq, "(");
          Join(seq, m->cond, ", ");
          A(seq, ")");
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
      });
      break;
    }
    case ast::NodeKind::CommClause: {
      const auto* m = n->As<ast::nodes::CommClause>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "[");
        if (m->x) {
          A(seq, S(m->x));
        }
        A(seq, "]");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->comm->expr));
        if (m->body) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->body));
        } else {
          A(seq, PrintDirective::kSemicolon);
        }
      });
      break;
    }
    case ast::NodeKind::LanguageSpec: {
      const auto* m = n->As<ast::nodes::LanguageSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "language");
        A(seq, PrintDirective::kSpace);
        Join(seq, m->list, ", ");
      });
      break;
    }
    case ast::NodeKind::WithSpec: {
      const auto* m = n->As<ast::nodes::WithSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "with");
        A(seq, PrintDirective::kSpace);
        A(seq, "{");
        if (!m->list.empty()) {
          A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& cseq) {
              A(cseq, PrintDirective::kHardLine);
              Join(cseq, m->list, PrintDirective::kHardLine);
              if (m->list.back()->value->On(src_).ends_with("\n\"")) {
                A(cseq, PrintDirective::kSpace);
              } else {
                A(cseq, PrintDirective::kHardLine);
              }
            }));
        }
        A(seq, "}");
      });
      break;
    }
    case ast::NodeKind::WithStmt: {
      const auto* m = n->As<ast::nodes::WithStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->kind));
        if (m->overrides) {
          // TODO: CHECK PARSER
          A(seq, PrintDirective::kSpace);
          A(seq, "override");
        }
        if (!m->list.empty()) {
          A(seq, PrintDirective::kSpace);
          A(seq, "(");
          Join(seq, m->list, ", ");
          A(seq, ")");
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->value));
      });
      break;
    }
    case ast::NodeKind::SelectorExpr: {
      const auto* m = n->As<ast::nodes::SelectorExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->x));
        A(seq, ".");
        A(seq, S(m->sel));
      });
      break;
    }
    case ast::NodeKind::DefKindExpr: {
      const auto* m = n->As<ast::nodes::DefKindExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->kind));
        A(seq, PrintDirective::kSpace);
        Join(seq, m->list, ", ");
      });
      break;
    }
    case ast::NodeKind::ExceptExpr: {
      const auto* m = n->As<ast::nodes::ExceptExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->x));
        Join(seq, m->list, ", ");
      });
      break;
    }
    case ast::NodeKind::FromExpr: {
      const auto* m = n->As<ast::nodes::FromExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->kind));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->from));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->x));
      });
      break;
    }
    case ast::NodeKind::ModifiesExpr: {
      const auto* m = n->As<ast::nodes::ModifiesExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "modifies");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->x));
        A(seq, PrintDirective::kSpace);
        A(seq, ":=");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->y));
      });
      break;
    }
    case ast::NodeKind::ParenExpr: {
      const auto* m = n->As<ast::nodes::ParenExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "(");
        A(seq, NewSequence(Sequence::Attribute::kGrouped, [&](auto& eseq) {
            Join(eseq, m->list, ", ");
          }));
        A(seq, ")");
      });
      break;
    }
    case ast::NodeKind::PostExpr: {
      const auto* m = n->As<ast::nodes::PostExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->x));
        A(seq, S(m->op));
      });
      break;
    }
    case ast::NodeKind::BinaryExpr: {
      const auto* m = n->As<ast::nodes::BinaryExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        const bool spacing = m->op.kind != ast::TokenKind::COLON && m->op.kind != ast::TokenKind::RANGE;
        A(seq, S(m->x));
        if (spacing) {
          A(seq, PrintDirective::kSpace);
        }
        A(seq, S(m->op));
        if (spacing) {
          A(seq, PrintDirective::kSpace);
        }
        A(seq, S(m->y));
      });
      break;
    }
    case ast::NodeKind::UnaryExpr: {
      const auto* m = n->As<ast::nodes::UnaryExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->op));
        if (m->op.range.Length() > 1) {
          A(seq, PrintDirective::kSpace);
        }
        A(seq, S(m->x));
      });
      break;
    }
    case ast::NodeKind::ValueExpr: {
      const auto* m = n->As<ast::nodes::ValueExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->x));
        A(seq, S(m->y));
      });
      break;
    }
    case ast::NodeKind::ParamExpr: {
      const auto* m = n->As<ast::nodes::ParamExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->x));
        A(seq, S(m->y));
      });
      break;
    }
    case ast::NodeKind::ImportDecl: {
      const auto* m = n->As<ast::nodes::ImportDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "import");
        A(seq, PrintDirective::kSpace);
        A(seq, "from");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->module));
        if (m->language) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->language));
        }
        A(seq, PrintDirective::kSpace);
        if (m->list.size() == 1 && m->list[0]->kind.range.Length() == 0) {
          // TODO: improve parser in this aspect
          A(seq, "all");
        } else {
          A(seq, "{");
          A(seq, PrintDirective::kSpace);
          A(seq, NewSequence(Sequence::Attribute::kGrouped, [&](auto& fseq) {
              Join(fseq, m->list, {PrintDirective::kSemicolon, PrintDirective::kSpaceOrLine});
            }));
          A(seq, PrintDirective::kSpace);
          A(seq, "}");
        }
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::GroupDecl: {
      const auto* m = n->As<ast::nodes::GroupDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "group");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& fseq) {
            Join(fseq, m->defs, {PrintDirective::kSemicolon, PrintDirective::kHardLine});
          }));
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::FriendDecl: {
      const auto* m = n->As<ast::nodes::FriendDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "friend module");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->module));
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::ClassTypeDecl: {
      const auto* m = n->As<ast::nodes::ClassTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->external) {
          A(seq, "external");
          A(seq, PrintDirective::kSpace);
        }
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, "class");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (!m->extends.empty()) {
          A(seq, PrintDirective::kSpace);
          A(seq, "extends");
          A(seq, PrintDirective::kSpace);
          Join(seq, m->extends, ", ");
        }
        if (m->runs_on) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->runs_on));
        }
        if (m->mtc) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->mtc));
        }
        if (m->system) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->system));
        }
        A(seq, PrintDirective::kHardLine);
        A(seq, "{");
        A(seq, PrintDirective::kHardLine);
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& iseq) {
            Join(iseq, m->defs, PrintDirective::kHardLine);
          }));
        A(seq, PrintDirective::kHardLine);
        A(seq, "}");
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::MapTypeDecl: {
      const auto* m = n->As<ast::nodes::MapTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "map");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->spec));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::EnumTypeDecl: {
      const auto* m = n->As<ast::nodes::EnumTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, "enumerated");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        A(seq, PrintDirective::kHardLine);
        A(seq, "{");
        A(seq, PrintDirective::kHardLine);
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& vseq) {
            const auto count = std::size(m->values);
            std::size_t i{0};
            for (const auto& ev : m->values) {
              switch (ev->nkind) {
                case ast::NodeKind::CallExpr: {
                  const auto* ce = ev->As<ast::nodes::CallExpr>();
                  A(vseq, S(ce->fun));
                  A(vseq, PrintDirective::kSpace);
                  A(vseq, S(ce->args));
                  break;
                }
                default: {
                  A(vseq, S(ev));
                  break;
                }
              }
              if (i + 1 != count) {
                A(vseq, ",");
                A(vseq, PrintDirective::kHardLine);
              }
              ++i;
            }
          }));
        A(seq, PrintDirective::kHardLine);
        A(seq, "}");
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::EnumSpec: {
      const auto* m = n->As<ast::nodes::EnumSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "enumerated");
        A(seq, PrintDirective::kSpace);
        A(seq, PrintDirective::kSpace);
        A(seq, "{");
        A(seq, PrintDirective::kHardLine);
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& vseq) {
            const auto count = std::size(m->values);
            std::size_t i{0};
            for (const auto& ev : m->values) {
              switch (ev->nkind) {
                case ast::NodeKind::CallExpr: {
                  const auto* ce = ev->As<ast::nodes::CallExpr>();
                  A(vseq, S(ce->fun));
                  A(vseq, PrintDirective::kSpace);
                  A(vseq, S(ce->args));
                  break;
                }
                default: {
                  A(vseq, S(ev));
                  break;
                }
              }
              if (i + 1 != count) {
                A(vseq, ",");
                A(vseq, PrintDirective::kHardLine);
              }
              ++i;
            }
          }));
        A(seq, PrintDirective::kHardLine);
        A(seq, "}");
      });
      break;
    }
    case ast::NodeKind::BehaviourTypeDecl: {
      const auto* m = n->As<ast::nodes::BehaviourTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->kind));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        A(seq, S(m->params));
        if (m->runs_on) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->runs_on));
        }
        if (m->system) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->system));
        }
        if (m->ret) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->ret));
        }
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::PortTypeDecl: {
      const auto* m = n->As<ast::nodes::PortTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, "port");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->kind));
        if (m->realtime) {
          A(seq, PrintDirective::kSpace);
          A(seq, "realtime");
        }
        if (!m->attrs.empty()) {
          A(seq, NewSequence(Sequence::Attribute::kGrouped, [&](auto& dseq) {
              A(dseq, PrintDirective::kSpaceOrLine);
              Join(dseq, m->attrs, {PrintDirective::kSemicolon, PrintDirective::kHardLine});
              A(dseq, PrintDirective::kSemicolon);
              A(dseq, PrintDirective::kSpaceOrLine);
            }));
        }
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::PortAttribute: {
      const auto* m = n->As<ast::nodes::PortAttribute>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->kind));
        A(seq, PrintDirective::kSpace);
        Join(seq, m->types, ", ");
      });
      break;
    }
    case ast::NodeKind::PortMapAttribute: {
      const auto* m = n->As<ast::nodes::PortMapAttribute>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->kind));
        A(seq, PrintDirective::kSpace);
        A(seq, "param");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->params));
      });
      break;
    }
    case ast::NodeKind::ComponentTypeDecl: {
      const auto* m = n->As<ast::nodes::ComponentTypeDecl>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, "component");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          S_pars(seq, m->pars);
        }
        if (!m->extends.empty()) {
          A(seq, PrintDirective::kSpace);
          A(seq, "extends");
          A(seq, PrintDirective::kSpace);
          Join(seq, m->extends, ", ");
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::FormalPar: {
      const auto* m = n->As<ast::nodes::FormalPar>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->direction) {
          A(seq, S(m->direction));
          A(seq, PrintDirective::kSpace);
        }
        if (m->restriction) {
          A(seq, S(m->restriction));
          A(seq, PrintDirective::kSpace);
        }
        if (m->modif) {
          A(seq, S(m->modif));
          A(seq, PrintDirective::kSpace);
        }
        A(seq, S(m->type));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        for (const auto* pe : m->arraydef) {
          A(seq, "[");
          // TODO: check why it is being parsed as a list
          for (const auto* expr : pe->list) {
            A(seq, S(expr));
          }
          A(seq, "]");
        }
        if (m->value) {
          A(seq, PrintDirective::kSpace);
          A(seq, ":=");
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->value));
        }
      });
      break;
    }
    case ast::NodeKind::LengthExpr: {
      const auto* m = n->As<ast::nodes::LengthExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->x) {
          A(seq, S(m->x));
          A(seq, PrintDirective::kSpace);
        }
        A(seq, "length");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->size));
      });
      break;
    }
    case ast::NodeKind::RunsOnSpec: {
      const auto* m = n->As<ast::nodes::RunsOnSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "runs on");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->comp));
      });
      break;
    }
    case ast::NodeKind::SystemSpec: {
      const auto* m = n->As<ast::nodes::SystemSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "system");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->comp));
      });
      break;
    }
    case ast::NodeKind::MtcSpec: {
      const auto* m = n->As<ast::nodes::MtcSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "mtc");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->comp));
      });
      break;
    }
    case ast::NodeKind::ReturnSpec: {
      const auto* m = n->As<ast::nodes::ReturnSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "return");
        if (m->restriction) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->restriction));
        }
        if (m->modif) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->modif));
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->type));
      });
      break;
    }
    case ast::NodeKind::RestrictionSpec: {
      const auto* m = n->As<ast::nodes::RestrictionSpec>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->is_template) {
          A(seq, "template");
        }
        if (m->type.kind != ast::TokenKind::kSentinel) {
          if (m->is_template) {
            A(seq, PrintDirective::kSpace);
            A(seq, "(");
          }
          A(seq, S(m->type));
          if (m->is_template) {
            A(seq, ")");
          }
        }
      });
      break;
    }
    case ast::NodeKind::IndexExpr: {
      const auto* m = n->As<ast::nodes::IndexExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->x));
        A(seq, "[");
        A(seq, S(m->index));
        A(seq, "]");
      });
      break;
    }
    case ast::NodeKind::CallExpr: {
      const auto* m = n->As<ast::nodes::CallExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->fun));
        A(seq, S(m->args));
      });
      break;
    }
    case ast::NodeKind::RedirectExpr: {
      const auto* m = n->As<ast::nodes::RedirectExpr>();
      return NewSequence(Sequence::Attribute::kGrouped, [&](auto& seq) {
        A(seq, S(m->x));
        A(seq, PrintDirective::kSpaceOrLine);
        A(seq, "->");
        if (!m->value.empty()) {
          A(seq, PrintDirective::kSpace);
          A(seq, "value");
          A(seq, PrintDirective::kSpace);
          Join(seq, m->value, ", ");
        }
        if (!m->param.empty()) {
          A(seq, PrintDirective::kSpace);
          A(seq, "param");
          A(seq, PrintDirective::kSpace);
          Join(seq, m->param, ", ");
        }
        if (m->sender) {
          A(seq, PrintDirective::kSpace);
          A(seq, "sender");
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->sender));
        }
        if (m->to_index) {
          A(seq, PrintDirective::kSpace);
          A(seq, "@index");
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->to_index));
        }
        if (m->timestamp) {
          A(seq, PrintDirective::kSpace);
          A(seq, "timestamp");
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->timestamp));
        }
      });
      break;
    }
    case ast::NodeKind::RedirectToIndex: {
      const auto* m = n->As<ast::nodes::RedirectToIndex>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        if (m->value) {
          A(seq, "value");
          A(seq, PrintDirective::kSpace);
        }
        A(seq, S(m->index));
      });
      break;
    }
    case ast::NodeKind::ParametrizedIdent: {
      const auto* m = n->As<ast::nodes::ParametrizedIdent>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->ident));
        A(seq, S(m->params));
      });
      break;
    }
    case ast::NodeKind::RegexpExpr: {
      const auto* m = n->As<ast::nodes::RegexpExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "regexp");
        if (m->nocase) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->nocase));
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->x));
      });
      break;
    }
    case ast::NodeKind::PatternExpr: {
      const auto* m = n->As<ast::nodes::PatternExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "pattern");
        if (m->nocase) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->nocase));
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->x));
      });
      break;
    }
    case ast::NodeKind::DecodedExpr: {
      const auto* m = n->As<ast::nodes::DecodedExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->params));
        A(seq, S(m->x));
      });
      break;
    }
    case ast::NodeKind::DynamicExpr: {
      const auto* m = n->As<ast::nodes::DynamicExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->body));
      });
      break;
    }
    case ast::NodeKind::DecmatchExpr: {
      const auto* m = n->As<ast::nodes::DecmatchExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "decmatch");
        if (m->params) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->params));
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->x));
      });
      break;
    }
    case ast::NodeKind::ControlPart: {
      const auto* m = n->As<ast::nodes::ControlPart>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, "control");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::AssignmentExpr: {
      const auto* m = n->As<ast::nodes::AssignmentExpr>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        A(seq, S(m->property));
        A(seq, PrintDirective::kSpace);
        A(seq, ":=");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->value));
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
