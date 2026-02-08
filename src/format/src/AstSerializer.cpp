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

Unit AstSerializer::S(const ast::Node* n) {
  assert(n);
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
        seq += " ";
        seq += PrintDirective::kHardLine;
        seq += "{";
        seq += PrintDirective::kHardLine;
        seq += PrintDirective::kHardLine;
        Join(seq, m->defs, PrintDirective::kHardLine);
        seq += PrintDirective::kHardLine;
        seq += PrintDirective::kHardLine;
        seq += "}";
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
          seq += "{";
          seq += PrintDirective::kHardLine;
          seq += NewSequence(Sequence::Attribute::kIndented, [&](auto& bseq) {
            bseq += S(m->body);
          });
          seq += PrintDirective::kHardLine;
          seq += "}";
        }
      });
    }

    case ast::NodeKind::FormalPars: {
      const auto* m = n->As<ast::nodes::FormalPars>();
      return NewSequence(Sequence::Attribute::kGrouped, [&](auto& seq) {
        Join(seq, m->list, NewSequence(Sequence::Attribute::kNone, [&](auto& sseq) {
               sseq += ",";
               sseq += PrintDirective::kHardLine;
             }));
      });
    }

    case ast::NodeKind::BlockStmt: {
      const auto* m = n->As<ast::nodes::BlockStmt>();
      return NewSequence(Sequence::Attribute::kNone, [&](auto& seq) {
        Join(seq, m->stmts, NewSequence(Sequence::Attribute::kNone, [&](auto& sseq) {
               sseq += ";";
               sseq += PrintDirective::kHardLine;
             }));
        if (!m->stmts.empty()) {
          seq += ";";
        }
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
