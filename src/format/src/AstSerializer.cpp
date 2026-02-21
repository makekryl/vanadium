#include "vanadium/format/AstSerializer.h"

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <string_view>
#include <type_traits>
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

const ast::Range& RangeOf(const ast::Token& tok) {
  return tok.range;
}
const ast::Range& RangeOf(const ast::IsNode auto* n) {
  return n->nrange;
}

class TokenWindow {
 public:
  TokenWindow(std::string_view src, ast::pos_t off) : scanner_(src, off) {
    next_ = scanner_.Scan();
  }

  ast::Token Advance() {
    cur_ = next_;
    next_ = scanner_.Scan();
    return cur_;
  }

  [[nodiscard]] const ast::Token& Current() const {
    return cur_;
  }
  [[nodiscard]] const ast::Token& Next() const {
    return next_;
  }

 private:
  ast::parser::Scanner scanner_;
  ast::Token cur_;
  ast::Token next_;
};

class AstSerializer {
 public:
  AstSerializer(const ast::AST& ast, lib::Arena& arena) : ast_(ast), arena_(arena) {}

  Unit Serialize(const ast::Node* n) {
    tokens_.emplace(ast_.src, n->nrange.begin);
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
    return tok.On(ast_.src);
  }
  Unit S(const ast::Token* tok) {
    return S(*tok);
  }

  Sequence* NewSequence(Sequence::Attribute attrs, mp::Consumer<Sequence&> auto f) {
    auto* seq = arena_.Alloc<Sequence>(Sequence{.attributes = attrs, .units = {}});
    f(*seq);
    return seq;
  }
  Sequence* NewSequence(mp::Consumer<Sequence&> auto f) {
    return NewSequence(Sequence::Attribute::kNone, f);
  }

  template <typename T, typename Serializer = std::nullptr_t>
  void Join(Sequence& target, const std::vector<T>& items, std::initializer_list<Unit> separators,
            Serializer f_serialize = nullptr) {
    const auto serialize = [&](const T& item) {
      if constexpr (std::is_same_v<Serializer, std::nullptr_t>) {
        A(target, S(item));
      } else {
        f_serialize(item);
      }
    };
    if (!items.empty()) {
      serialize(items.front());
      for (std::size_t i = 1; i < items.size(); ++i) {
        for (const auto& su : separators) {
          A(target, su);
        }

        // if there's a leading comment, PreferredNewlines would be inserted by the comment scanner
        if (tokens_->Next().kind != ast::TokenKind::COMMENT) {
          const ast::pos_t prev_line = std::max(ast_.lines.LineOf(RangeOf(items[i - 1]).end), last_comment_line_);
          const ast::pos_t current_line = ast_.lines.LineOf(RangeOf(items[i]).begin);
          if (current_line != prev_line) {
            A(target, PreferredNewlines{current_line - prev_line});
          }
        }

        serialize(items[i]);
      }
    }
  }
  template <typename T, typename Serializer = std::nullptr_t>
  void Join(Sequence& target, const std::vector<T>& items, Unit separator, Serializer f_serialize = nullptr) {
    Join(target, items, {separator}, f_serialize);
  }

  //

  template <bool TrailingOnly = false>
  void ScanComments(Sequence& tgt) {
    ast::Token comment_tok;
    while ((comment_tok = tokens_->Next()).kind == ast::TokenKind::COMMENT) {
      const auto ctok = tokens_->Current();
      const ast::pos_t prev_line = ast_.lines.LineOf(ctok.range.end);
      const ast::pos_t comment_line = ast_.lines.LineOf(comment_tok.range.begin);

      if constexpr (TrailingOnly) {
        if (prev_line != comment_line) {
          return;
        }
      }
      tokens_->Advance();  // consume comment_tok

      if (comment_line != prev_line) {
        const ast::pos_t dy = ctok.kind == ast::TokenKind::COMMENT ? 0 : 1;
        for (ast::pos_t i = 0; i < (comment_line - prev_line - dy); ++i) {
          tgt.units.emplace_back(PrintDirective::kHardLine);
        }
      }
      last_comment_line_ = comment_line;

      tgt.units.emplace_back(Comment{comment_tok.On(ast_.src)});
    }
  }

  void A(Sequence& seq, Unit u) {
    // todo: optimize
    if (std::holds_alternative<EmptyUnit>(u)) {
      return;
    }

    const auto chk_trailing_comments = [&] {
      ScanComments<true>(seq);
    };
    const auto chk_leading_comments = [&] {
      if (tokens_->Next().kind != ast::TokenKind::COMMENT) {
        return;
      }
      ScanComments(seq);

      const ast::pos_t current_line = ast_.lines.LineOf(RangeOf(tokens_->Next()).begin);
      if (current_line > last_comment_line_) {
        A(seq, PreferredNewlines{current_line - last_comment_line_});
      }
    };

    if (const auto* pd = std::get_if<PrintDirective>(&u); pd && *pd == PrintDirective::kSemicolon) {
      if (tokens_->Next().kind == ast::TokenKind::SEMICOLON) {
        chk_leading_comments();
        //
        seq.units.push_back(std::move(u));
        tokens_->Advance();
        //
        chk_trailing_comments();
      } else {
        seq.units.push_back(std::move(u));
        if (tokens_->Next().kind == ast::TokenKind::SEMICOLON) {
          tokens_->Advance();
          chk_trailing_comments();
        }
      }
      return;
    }

    if (!std::holds_alternative<std::string_view>(u)) {
      seq.units.push_back(std::move(u));
      return;
    }

    chk_leading_comments();
    {
      [[maybe_unused]] const auto& s = std::get<std::string_view>(u);
      [[maybe_unused]] const auto next_tok = tokens_->Next();
      VANADIUM_DEBUG_ASSERT(s == next_tok.On(ast_.src), "fmt('{}') <-> actual('{}', {}-{})", s,  //
                            next_tok.On(ast_.src), next_tok.range.begin, next_tok.range.end);
    }
    tokens_->Advance();
    //
    seq.units.push_back(std::move(u));
    //
    chk_trailing_comments();
  }

  //

  const ast::AST& ast_;
  std::optional<TokenWindow> tokens_;
  ast::pos_t last_comment_line_{0};
  lib::Arena& arena_;
};

Unit AstSerializer::S(const ast::Node* n) {  // NOLINT(readability-function-size)
  switch (n->nkind) {
    case ast::NodeKind::RootNode: {
      const auto* m = n->As<ast::RootNode>();
      return NewSequence([&](auto& seq) {
        m->Accept([&](const auto* cn) {
          A(seq, S(cn));
          return false;
        });
        ScanComments(seq);
      });
    }

    case ast::NodeKind::Ident: {
      const auto* m = n->As<ast::nodes::Ident>();
      return m->On(ast_.src);
    }

    case ast::NodeKind::CompositeIdent: {
      return NewSequence([&](auto& seq) {
        const auto* m = n->As<ast::nodes::CompositeIdent>();
        A(seq, S(m->tok1));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->tok2));
      });
    }

    case ast::NodeKind::Module: {
      const auto* m = n->As<ast::nodes::Module>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        if (m->visibility) {
          A(seq, S(m->visibility));
          A(seq, PrintDirective::kSpace);
        }
        A(seq, S(m->def));
        switch (m->def->nkind) {
          case ast::NodeKind::ImportDecl:
          case ast::NodeKind::SubTypeDecl:
          case ast::NodeKind::TemplateDecl:
          case ast::NodeKind::ValueDecl:
            A(seq, PrintDirective::kSemicolon);
            break;
          case ast::NodeKind::FuncDecl:
            if (m->def->As<ast::nodes::FuncDecl>()->external) {
              A(seq, PrintDirective::kSemicolon);
            }
            break;
          default:
            break;
        }
      });
    }

    case ast::NodeKind::StructTypeDecl: {
      const auto* m = n->As<ast::nodes::StructTypeDecl>();
      return NewSequence([&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->kind));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        A(seq, PrintDirective::kSpace);
        A(seq, PrintDirective::kHardLine);
        A(seq, "{");
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& fseq) {
            A(fseq, PrintDirective::kHardLine);
            Join(fseq, m->fields, {",", PrintDirective::kHardLine});
          }));
        A(seq, PrintDirective::kHardLine);
        A(seq, "}");
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
    }

    case ast::NodeKind::Field: {
      const auto* m = n->As<ast::nodes::Field>();
      return NewSequence([&](auto& seq) {
        A(seq, S(m->type));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          A(seq, S(m->pars));
        }
        if (m->arraydef) {
          A(seq, S(m->arraydef));
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
          A(seq, S(m->pars));
        }
        if (m->params) {
          A(seq, S(m->params));
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

        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
    }

    case ast::NodeKind::BlockStmt: {
      const auto* m = n->As<ast::nodes::BlockStmt>();
      return NewSequence([&](auto& seq) {
        A(seq, "{");
        if (!m->stmts.empty()) {
          A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& iseq) {
              A(iseq, PrintDirective::kHardLine);
              Join(iseq, m->stmts, PrintDirective::kHardLine, [&](const ast::nodes::Stmt* stmt) {
                A(iseq, S(stmt));
                switch (stmt->nkind) {
                  case ast::NodeKind::ExprStmt:
                  case ast::NodeKind::DeclStmt:
                  case ast::NodeKind::ReturnStmt:
                  case ast::NodeKind::BranchStmt:
                    A(iseq, PrintDirective::kSemicolon);
                    break;
                  default:
                    break;
                }
              });
            }));
          A(seq, PrintDirective::kHardLine);
        } else {
          A(seq, PrintDirective::kSpace);
        }
        A(seq, "}");

        if (tokens_->Next().kind == ast::TokenKind::SEMICOLON) {
          tokens_->Advance();
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        A(seq, S(m->name));
        if (m->arraydef) {
          A(seq, S(m->arraydef));
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
          A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& iseq) {
              A(iseq, PrintDirective::kSpaceOrLine);
              Join(iseq, m->list, {",", PrintDirective::kSpaceOrLine});
            }));
          A(seq, PrintDirective::kSpaceOrLine);
        }
        A(seq, "}");
      });
    }

    case ast::NodeKind::ValueLiteral: {
      const auto* m = n->As<ast::nodes::ValueLiteral>();
      return m->tok.On(ast_.src);
    }

    case ast::NodeKind::StructSpec: {
      const auto* m = n->As<ast::nodes::StructSpec>();
      return NewSequence([&](auto& seq) {
        A(seq, S(m->kind));
        A(seq, "{");
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& fseq) {
            A(fseq, PrintDirective::kHardLine);
            Join(fseq, m->fields, {",", PrintDirective::kHardLine});
          }));
        A(seq, PrintDirective::kHardLine);
        A(seq, "}");
      });
      break;
    }
    case ast::NodeKind::MapSpec: {
      const auto* m = n->As<ast::nodes::MapSpec>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
          A(seq, S(m->pars));
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
      return NewSequence([&](auto& seq) {
        A(seq, "modulepar");
        A(seq, PrintDirective::kSpace);
        A(seq, "{");
        if (!m->decls.empty()) {
          A(seq, NewSequence(Sequence::Attribute::kGrouped, [&](auto& dseq) {
              A(dseq, PrintDirective::kSpaceOrLine);
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
      return NewSequence([&](auto& seq) {
        A(seq, "create");
        A(seq, S(m->params));
        if (m->body) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->body));
        }
      });
      break;
    }
    case ast::NodeKind::SignatureDecl: {
      const auto* m = n->As<ast::nodes::SignatureDecl>();
      return NewSequence([&](auto& seq) {
        A(seq, "signature");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          A(seq, S(m->pars));
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        A(seq, S(m->stmt));
        A(seq, S(m->body));
      });
      break;
    }
    case ast::NodeKind::ForStmt: {
      const auto* m = n->As<ast::nodes::ForStmt>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        A(seq, "while");
        A(seq, PrintDirective::kSpace);
        A(seq, "(");
        A(seq, S(m->cond));
        A(seq, ")");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
      });
      break;
    }
    case ast::NodeKind::DoWhileStmt: {
      const auto* m = n->As<ast::nodes::DoWhileStmt>();
      return NewSequence([&](auto& seq) {
        A(seq, "do");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
        A(seq, PrintDirective::kSpace);
        A(seq, "while");
        A(seq, PrintDirective::kSpace);
        A(seq, "(");
        A(seq, S(m->cond));
        A(seq, ")");
      });
      break;
    }
    case ast::NodeKind::IfStmt: {
      const auto* m = n->As<ast::nodes::IfStmt>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& cseq) {
            A(cseq, PrintDirective::kHardLine);
            Join(cseq, m->clauses, PrintDirective::kHardLine);
          }));
        A(seq, PrintDirective::kHardLine);
        A(seq, "}");
      });
      break;
    }
    case ast::NodeKind::CaseClause: {
      const auto* m = n->As<ast::nodes::CaseClause>();
      return NewSequence([&](auto& seq) {
        A(seq, "case");
        A(seq, PrintDirective::kSpace);
        // todo: "case else" proper chk
        if (m->cond.empty()) {
          A(seq, "else");
        } else {
          A(seq, "(");
          Join(seq, m->cond, {",", PrintDirective::kSpace});
          A(seq, ")");
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
      });
      break;
    }
    case ast::NodeKind::CommClause: {
      const auto* m = n->As<ast::nodes::CommClause>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        A(seq, "language");
        A(seq, PrintDirective::kSpace);
        Join(seq, m->list, {",", PrintDirective::kSpace});
      });
      break;
    }
    case ast::NodeKind::WithSpec: {
      const auto* m = n->As<ast::nodes::WithSpec>();
      return NewSequence([&](auto& seq) {
        A(seq, "with");
        A(seq, PrintDirective::kSpace);
        A(seq, "{");
        if (!m->list.empty()) {
          A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& cseq) {
              A(cseq, PrintDirective::kHardLine);
              Join(cseq, m->list, {PrintDirective::kSemicolon, PrintDirective::kHardLine});
              if (m->list.size() > 1) {
                A(cseq, PrintDirective::kSemicolon);
              }
            }));
        }
        if (m->list.back()->value->On(ast_.src).ends_with("\n\"")) {
          A(seq, PrintDirective::kSpace);
        } else {
          A(seq, PrintDirective::kHardLine);
        }
        A(seq, "}");
      });
      break;
    }
    case ast::NodeKind::WithStmt: {
      const auto* m = n->As<ast::nodes::WithStmt>();
      return NewSequence([&](auto& seq) {
        A(seq, S(m->kind));
        if (m->overrides) {
          // TODO: CHECK PARSER
          A(seq, PrintDirective::kSpace);
          A(seq, "override");
        }
        if (!m->list.empty()) {
          A(seq, PrintDirective::kSpace);
          A(seq, "(");
          Join(seq, m->list, {",", PrintDirective::kSpace});
          A(seq, ")");
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->value));
      });
      break;
    }
    case ast::NodeKind::SelectorExpr: {
      const auto* m = n->As<ast::nodes::SelectorExpr>();
      return NewSequence([&](auto& seq) {
        A(seq, S(m->x));
        A(seq, ".");
        A(seq, S(m->sel));
      });
      break;
    }
    case ast::NodeKind::DefKindExpr: {
      const auto* m = n->As<ast::nodes::DefKindExpr>();
      return NewSequence([&](auto& seq) {
        A(seq, S(m->kind));
        A(seq, PrintDirective::kSpace);
        Join(seq, m->list, {",", PrintDirective::kSpace});
      });
      break;
    }
    case ast::NodeKind::ExceptExpr: {
      const auto* m = n->As<ast::nodes::ExceptExpr>();
      return NewSequence([&](auto& seq) {
        A(seq, S(m->x));
        Join(seq, m->list, {",", PrintDirective::kSpace});
      });
      break;
    }
    case ast::NodeKind::FromExpr: {
      const auto* m = n->As<ast::nodes::FromExpr>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence(Sequence::Attribute::kGrouped, [&](auto& seq) {
        A(seq, "(");
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& eseq) {
            A(eseq, PrintDirective::kSoftLine);
            Join(eseq, m->list, {",", PrintDirective::kSpaceOrLine});
          }));
        A(seq, PrintDirective::kSoftLine);
        A(seq, ")");
      });
      break;
    }
    case ast::NodeKind::ArrayExpr: {
      const auto* m = n->As<ast::nodes::ArrayExpr>();
      return NewSequence([&](auto& seq) {
        for (const auto* dim : m->list) {
          A(seq, "[");
          A(seq, S(dim));
          A(seq, "]");
        }
      });
    }
    case ast::NodeKind::PostExpr: {
      const auto* m = n->As<ast::nodes::PostExpr>();
      return NewSequence([&](auto& seq) {
        A(seq, S(m->x));
        A(seq, S(m->op));
      });
      break;
    }
    case ast::NodeKind::BinaryExpr: {
      const auto* m = n->As<ast::nodes::BinaryExpr>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        A(seq, S(m->x));
        A(seq, S(m->y));
      });
      break;
    }
    case ast::NodeKind::ParamExpr: {
      const auto* m = n->As<ast::nodes::ParamExpr>();
      return NewSequence([&](auto& seq) {
        A(seq, S(m->x));
        A(seq, PrintDirective::kSpace);
        A(seq, "param");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->y));
      });
      break;
    }
    case ast::NodeKind::ImportDecl: {
      const auto* m = n->As<ast::nodes::ImportDecl>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        A(seq, "friend");
        A(seq, PrintDirective::kSpace);
        A(seq, "module");
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
      return NewSequence([&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        if (m->external) {
          A(seq, "external");
          A(seq, PrintDirective::kSpace);
        }
        A(seq, "class");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (!m->extends.empty()) {
          A(seq, PrintDirective::kSpace);
          A(seq, "extends");
          A(seq, PrintDirective::kSpace);
          Join(seq, m->extends, {",", PrintDirective::kSpace});
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
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& iseq) {
            A(iseq, PrintDirective::kHardLine);
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
      return NewSequence([&](auto& seq) {
        A(seq, "map");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->spec));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          A(seq, S(m->pars));
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
      return NewSequence([&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, "enumerated");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          A(seq, S(m->pars));
        }
        A(seq, PrintDirective::kHardLine);
        A(seq, "{");
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& vseq) {
            A(vseq, PrintDirective::kHardLine);
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
      return NewSequence([&](auto& seq) {
        A(seq, "enumerated");
        A(seq, PrintDirective::kSpace);
        A(seq, PrintDirective::kSpace);
        A(seq, "{");
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& vseq) {
            A(vseq, PrintDirective::kHardLine);
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
      return NewSequence([&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->kind));
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          A(seq, S(m->pars));
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
      return NewSequence([&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, "port");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          A(seq, S(m->pars));
        }
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->kind));
        if (m->realtime) {
          A(seq, PrintDirective::kSpace);
          A(seq, "realtime");
        }
        A(seq, PrintDirective::kSpace);
        A(seq, "{");
        if (!m->attrs.empty()) {
          A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& dseq) {
              A(dseq, PrintDirective::kHardLine);
              Join(dseq, m->attrs, {PrintDirective::kSemicolon, PrintDirective::kHardLine});
              A(dseq, PrintDirective::kSemicolon);
            }));
        }
        A(seq, PrintDirective::kHardLine);
        A(seq, "}");
        if (m->with) {
          A(seq, PrintDirective::kSpace);
          A(seq, S(m->with));
        }
      });
      break;
    }
    case ast::NodeKind::PortAttribute: {
      const auto* m = n->As<ast::nodes::PortAttribute>();
      return NewSequence([&](auto& seq) {
        A(seq, S(m->kind));
        A(seq, PrintDirective::kSpace);
        Join(seq, m->types, {",", PrintDirective::kSpace});
      });
      break;
    }
    case ast::NodeKind::PortMapAttribute: {
      const auto* m = n->As<ast::nodes::PortMapAttribute>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        A(seq, "type");
        A(seq, PrintDirective::kSpace);
        A(seq, "component");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->name));
        if (m->pars) {
          A(seq, S(m->pars));
        }
        if (!m->extends.empty()) {
          A(seq, PrintDirective::kSpace);
          A(seq, "extends");
          A(seq, PrintDirective::kSpace);
          Join(seq, m->extends, {",", PrintDirective::kSpace});
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

    case ast::NodeKind::FormalPars: {
      const auto* m = n->As<ast::nodes::FormalPars>();
      return NewSequence(Sequence::Attribute::kGrouped, [&](auto& seq) {
        A(seq, "(");
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& eseq) {
            A(eseq, PrintDirective::kSoftLine);
            Join(eseq, m->list, {",", PrintDirective::kSpaceOrLine});
          }));
        A(seq, PrintDirective::kSoftLine);
        A(seq, ")");
      });
    }
    case ast::NodeKind::FormalPar: {
      const auto* m = n->As<ast::nodes::FormalPar>();
      return NewSequence([&](auto& seq) {
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
        if (m->arraydef) {
          A(seq, S(m->arraydef));
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

    case ast::NodeKind::TypePars: {
      const auto* m = n->As<ast::nodes::TypePars>();
      return NewSequence(Sequence::Attribute::kGrouped, [&](auto& seq) {
        A(seq, "<");
        A(seq, NewSequence(Sequence::Attribute::kIndented, [&](auto& eseq) {
            A(eseq, PrintDirective::kSoftLine);
            Join(eseq, m->list, {",", PrintDirective::kSpaceOrLine});
          }));
        A(seq, PrintDirective::kSoftLine);
        A(seq, ">");
      });
    }
    case ast::NodeKind::TypePar: {
      const auto* m = n->As<ast::nodes::TypePar>();
      return NewSequence([&](auto& seq) {
        if (m->direction) {
          A(seq, S(m->direction));
          A(seq, PrintDirective::kSpace);
        }
        A(seq, S(m->type));
        A(seq, PrintDirective::kSpace);
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        A(seq, "runs");
        A(seq, PrintDirective::kSpace);
        A(seq, "on");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->comp));
      });
      break;
    }
    case ast::NodeKind::SystemSpec: {
      const auto* m = n->As<ast::nodes::SystemSpec>();
      return NewSequence([&](auto& seq) {
        A(seq, "system");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->comp));
      });
      break;
    }
    case ast::NodeKind::MtcSpec: {
      const auto* m = n->As<ast::nodes::MtcSpec>();
      return NewSequence([&](auto& seq) {
        A(seq, "mtc");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->comp));
      });
      break;
    }
    case ast::NodeKind::ReturnSpec: {
      const auto* m = n->As<ast::nodes::ReturnSpec>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        if (m->x) {
          A(seq, S(m->x));
        }
        A(seq, "[");
        A(seq, S(m->index));
        A(seq, "]");
      });
      break;
    }
    case ast::NodeKind::CallExpr: {
      const auto* m = n->As<ast::nodes::CallExpr>();
      return NewSequence([&](auto& seq) {
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
          Join(seq, m->value, {",", PrintDirective::kSpace});
        }
        if (!m->param.empty()) {
          A(seq, PrintDirective::kSpace);
          A(seq, "param");
          A(seq, PrintDirective::kSpace);
          Join(seq, m->param, {",", PrintDirective::kSpace});
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        A(seq, S(m->ident));
        A(seq, S(m->params));
      });
      break;
    }
    case ast::NodeKind::RegexpExpr: {
      const auto* m = n->As<ast::nodes::RegexpExpr>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        A(seq, "@decoded");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->params));
        A(seq, S(m->x));
      });
      break;
    }
    case ast::NodeKind::DynamicExpr: {
      const auto* m = n->As<ast::nodes::DynamicExpr>();
      return NewSequence([&](auto& seq) {
        A(seq, "@dynamic");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->body));
      });
      break;
    }
    case ast::NodeKind::DecmatchExpr: {
      const auto* m = n->As<ast::nodes::DecmatchExpr>();
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
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
      return NewSequence([&](auto& seq) {
        A(seq, S(m->property));
        A(seq, PrintDirective::kSpace);
        A(seq, ":=");
        A(seq, PrintDirective::kSpace);
        A(seq, S(m->value));
      });
      break;
    }

    default: {
      VANADIUM_DEBUG_ASSERT(false, "Unhandled node '{}'", magic_enum::enum_name(n->nkind));
      while (tokens_->Current().range.end != n->nrange.end) {
        tokens_->Advance();
      }
      return NewSequence([&](auto& seq) {
        // push directly to seq.units - avoid token matching against this giant blob
        seq.units.push_back(n->nrange.String(ast_.src));
      });
    }
  }
}

}  // namespace

Unit SerializeAst(const ast::AST& ast, const ast::Node* n, lib::Arena& arena) {
  return AstSerializer(ast, arena).Serialize(n);
}

}  // namespace vanadium::format
