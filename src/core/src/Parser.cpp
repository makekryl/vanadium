#include "Parser.h"

#include <cctype>
#include <cstring>
#include <format>
#include <magic_enum/magic_enum.hpp>
#include <set>
#include <sstream>
#include <utility>

#include "AST.h"
#include "ASTNodes.h"
#include "Scanner.h"

namespace vanadium::core::ast {
namespace parser {

// clang-format off
const std::set<TokenKind> kTokStmtStart = {
  TokenKind::ALT,
  TokenKind::ALTSTEP,
  TokenKind::BREAK,
  TokenKind::CASE,
  TokenKind::CONST,
  TokenKind::CONTINUE,
  TokenKind::CONTROL,
  TokenKind::DISPLAY,
  TokenKind::DO,
  TokenKind::ELSE,
  TokenKind::ENCODE,
  TokenKind::EXTENSION,
  TokenKind::FOR,
  TokenKind::FRIEND,
  TokenKind::FUNCTION,
  TokenKind::GOTO,
  TokenKind::GROUP,
  TokenKind::IF,
  TokenKind::IMPORT,
  TokenKind::INTERLEAVE,
  TokenKind::LABEL,
  TokenKind::MAP,
  TokenKind::MODULE,
  TokenKind::MODULEPAR,
  TokenKind::PORT,
  TokenKind::PRIVATE,
  TokenKind::PUBLIC,
  TokenKind::RBRACE,
  TokenKind::REPEAT,
  TokenKind::RETURN,
  TokenKind::SELECT,
  TokenKind::SEMICOLON,
  TokenKind::SIGNATURE,
  TokenKind::TEMPLATE,
  TokenKind::TESTCASE,
  TokenKind::TIMER,
  TokenKind::TYPE,
  TokenKind::UNMAP,
  TokenKind::VAR,
  TokenKind::VARIANT,
  TokenKind::WHILE,
};
const std::set<TokenKind> kTokOperandStart = { // +IDENT? see ntt todo
  TokenKind::ADDRESS,
  TokenKind::ALL,
  TokenKind::ANY,
  TokenKind::ANYKW,
  TokenKind::BITSTRING,
  TokenKind::CHARSTRING,
  TokenKind::ERROR,
  TokenKind::FAIL,
  TokenKind::FALSE,
  TokenKind::FLOAT,
  TokenKind::INCONC,
  TokenKind::INT,
  TokenKind::MAP,
  TokenKind::MTC,
  TokenKind::MUL,
  TokenKind::kNaN,
  TokenKind::NONE,
  TokenKind::kNULL,
  TokenKind::OMIT,
  TokenKind::PASS,
  TokenKind::STRING,
  TokenKind::SYSTEM,
  TokenKind::TESTCASE,
  TokenKind::TIMER,
  TokenKind::TRUE,
  TokenKind::UNIVERSAL,
  TokenKind::UNMAP,
};
const std::set<TokenKind> kTokTopLevel = {
  TokenKind::COMMA,
  TokenKind::SEMICOLON,
  TokenKind::MODULE,
  TokenKind::CONTROL,
  TokenKind::EXTERNAL,
  TokenKind::FRIEND,
  TokenKind::FUNCTION,
  TokenKind::GROUP,
  TokenKind::IMPORT,
  TokenKind::MODULEPAR,
  TokenKind::SIGNATURE,
  TokenKind::TEMPLATE,
  TokenKind::TYPE,
  TokenKind::VAR,
  TokenKind::ALTSTEP,
  TokenKind::CONST,
  TokenKind::PRIVATE,
  TokenKind::PUBLIC,
  TokenKind::TIMER,
  TokenKind::PORT,
  TokenKind::REPEAT,
  TokenKind::BREAK,
  TokenKind::CONTINUE,
  TokenKind::LABEL,
  TokenKind::GOTO,
  TokenKind::RETURN,
  TokenKind::SELECT,
  TokenKind::ALT,
  TokenKind::INTERLEAVE,
  TokenKind::LBRACK,
  TokenKind::FOR,
  TokenKind::WHILE,
  TokenKind::DO,
  TokenKind::IF,
  TokenKind::LBRACE,
  TokenKind::IDENT,
  TokenKind::ANYKW,
  TokenKind::ALL,
  TokenKind::MAP,
  TokenKind::UNMAP,
  TokenKind::MTC,
  TokenKind::TESTCASE,
};
// clang-format on

constexpr int kLowestPrec = 0;
namespace {
int PrecedenceOf(TokenKind kind) {
  switch (kind) {
    case TokenKind::ASSIGN:
      return 1;
    case TokenKind::COLON:
      return 2;
    case TokenKind::DECODE:
      return 3;
    case TokenKind::RANGE:
      return 4;
    case TokenKind::EXCL:
      return 5;
    case TokenKind::OR:
      return 6;
    case TokenKind::XOR:
      return 7;
    case TokenKind::AND:
      return 8;
    case TokenKind::NOT:
      return 9;
    case TokenKind::EQ:
    case TokenKind::NE:
      return 10;
    case TokenKind::LT:
    case TokenKind::LE:
    case TokenKind::GT:
    case TokenKind::GE:
      return 11;
    case TokenKind::SHR:
    case TokenKind::SHL:
    case TokenKind::ROR:
    case TokenKind::ROL:
      return 12;
    case TokenKind::OR4B:
      return 13;
    case TokenKind::XOR4B:
      return 14;
    case TokenKind::AND4B:
      return 15;
    case TokenKind::NOT4B:
      return 16;
    case TokenKind::ADD:
    case TokenKind::SUB:
    case TokenKind::CONCAT:
      return 17;
    case TokenKind::MUL:
    case TokenKind::DIV:
    case TokenKind::REM:
    case TokenKind::MOD:
      return 18;
    case TokenKind::COLONCOLON:
      return 19;
    case TokenKind::OF:
      return 20;
    default:
      return kLowestPrec;
  }
}

inline void ExtendByIncorporatedNode(Node* n, Node* x) {
  n->parent = std::exchange(x->parent, n);
  n->nrange.begin = x->nrange.begin;
}
}  // namespace

Parser::Parser(lib::Arena& arena, std::string_view src) : scanner_(src), src_(src), arena_(&arena) {}

RootNode* Parser::ParseRoot() {
  return NewNode<RootNode>([&](auto& root) {
    tok_ = Peek(1).kind;
    while (tok_ != TokenKind::kEOF) {
      auto* node = Parse();
      node->parent = &root;
      root.nodes.push_back(node);
      if (tok_ != TokenKind::kEOF && !kTokTopLevel.contains(tok_)) {
        EmitError(Peek(1).range, std::format("unexpected '{}' token", magic_enum::enum_name(tok_)));
        break;
      }
      if (tok_ == TokenKind::COMMA || tok_ == TokenKind::SEMICOLON) {
        Consume();
      }
    }
  });
}

std::vector<SyntaxError>&& Parser::GetErrors() noexcept {
  return std::move(errors_);
}

std::vector<pos_t>&& Parser::ExtractLineMapping() noexcept {
  return scanner_.ExtractLineMapping();
}

Node* Parser::Parse() {
  switch (tok_) {
    case TokenKind::MODULE:
      return ParseModule();

    case TokenKind::CONTROL:
    case TokenKind::EXTERNAL:
    case TokenKind::FRIEND:
    case TokenKind::FUNCTION:
    case TokenKind::GROUP:
    case TokenKind::IMPORT:
    case TokenKind::MODULEPAR:
    case TokenKind::SIGNATURE:
    case TokenKind::TEMPLATE:
    case TokenKind::TYPE:
    case TokenKind::VAR:
    case TokenKind::ALTSTEP:
    case TokenKind::CONST:
    case TokenKind::PRIVATE:
    case TokenKind::PUBLIC:
      return ParseModuleDef();

    case TokenKind::TIMER:
    case TokenKind::PORT:
    case TokenKind::REPEAT:
    case TokenKind::BREAK:
    case TokenKind::CONTINUE:
    case TokenKind::LABEL:
    case TokenKind::GOTO:
    case TokenKind::RETURN:
    case TokenKind::SELECT:
    case TokenKind::ALT:
    case TokenKind::INTERLEAVE:
    case TokenKind::LBRACK:
    case TokenKind::FOR:
    case TokenKind::WHILE:
    case TokenKind::DO:
    case TokenKind::IF:
    case TokenKind::LBRACE:
    case TokenKind::IDENT:
    case TokenKind::ANYKW:
    case TokenKind::ALL:
    case TokenKind::MAP:
    case TokenKind::UNMAP:
    case TokenKind::MTC:
      return ParseStmt();

    case TokenKind::TESTCASE:
      if (Peek(1).kind == TokenKind::DOT) {
        return ParseStmt();
      }
      return ParseModuleDef();

    default:
      return ParseExpr();
  }
}

nodes::Module* Parser::ParseModule() {
  return NewNode<nodes::Module>([&](auto& m) {
    ConsumeInvariant(TokenKind::MODULE);
    ParseName(m.name);
    if (tok_ == TokenKind::LANGUAGE) {
      // TODO
      m.language = ParseLanguageSpec();
    }
    Expect(TokenKind::LBRACE);
    while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
      m.defs.emplace_back(ParseModuleDef());
      ExpectSemi();
    }
    Expect(TokenKind::RBRACE);
    m.with = ParseWith();
  });
}

nodes::ModuleDef* Parser::ParseModuleDef() {
  return NewNode<nodes::ModuleDef>([&](auto& md) {
    switch (tok_) {
      case TokenKind::PRIVATE:
      case TokenKind::PUBLIC:
        md.visibility = TokAlloc(Consume());
        break;
      case TokenKind::FRIEND:
        if (Peek(2).kind != TokenKind::MODULE) {
          md.visibility = TokAlloc(Consume());
        }
        break;
      default:
        break;
    }

    // const auto& etok = Peek(1); // todo: errornode
    md.def = [&] -> Node* {
      switch (tok_) {
        case TokenKind::IMPORT:
          return ParseImport();
        case TokenKind::GROUP:
          return ParseGroup();
        case TokenKind::FRIEND:
          return ParseFriend();
        case TokenKind::TYPE:
          return ParseTypeDecl();
        case TokenKind::TEMPLATE:
          return ParseTemplateDecl();
        case TokenKind::MODULEPAR:
          return ParseModulePar();
        case TokenKind::VAR:
        case TokenKind::CONST:
          return ParseValueDecl();
        case TokenKind::SIGNATURE:
          return ParseSignatureDecl();
        case TokenKind::FUNCTION:
        case TokenKind::TESTCASE:
        case TokenKind::ALTSTEP:
          return ParseFuncDecl();
        case TokenKind::CREATE:
          return ParseConstructorDecl();
        case TokenKind::CONTROL:
          return ParseControlPart();
        case TokenKind::EXTERNAL:
          switch (Peek(2).kind) {
            case TokenKind::FUNCTION:
              return ParseExtFuncDecl();
            case TokenKind::CONST:
              EmitError(Peek(1).range, "external constants are not supported anymore");
              Consume();
              return ParseValueDecl();
            default:
              EmitErrorExpected("function");
              Advance(kTokStmtStart);
              return nullptr;  // todo: errornode?
          }
        default:
          EmitErrorExpected("module definition");
          Advance(kTokStmtStart);
          return nullptr;  // todo: errornode?
      }
    }();
  });
}

nodes::Decl* Parser::ParseModulePar() {
  Consume();
  if (tok_ == TokenKind::LBRACE) {
    return NewNode<nodes::ModuleParameterGroup>([&](auto& mpg) {
      Consume();  // LBrace
      while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
        mpg.decls.push_back(NewNode<nodes::ValueDecl>([&](auto& vd) {
          vd.template_restriction = ParseRestrictionSpec();
          vd.type = ParseTypeRef();
          vd.decls = ParseDeclList();
          ExpectSemi();
        }));
      }
      Expect(TokenKind::RBRACE);
      mpg.with = ParseWith();
    });
  }
  return NewNode<nodes::ValueDecl>([&](auto& vd) {
    vd.template_restriction = ParseRestrictionSpec();
    vd.type = ParseTypeRef();
    vd.decls = ParseDeclList();
    vd.with = ParseWith();
  });
}

nodes::ControlPart* Parser::ParseControlPart() {
  return NewNode<nodes::ControlPart>([&](auto& c) {
    ConsumeInvariant(TokenKind::CONTROL);
    c.body = ParseBlockStmt();
    c.with = ParseWith();
  });
}

nodes::ImportDecl* Parser::ParseImport() {
  return NewNode<nodes::ImportDecl>([&](auto& d) {
    ConsumeInvariant(TokenKind::IMPORT);
    Expect(TokenKind::FROM);
    ParseName(d.module);
    if (tok_ == TokenKind::LANGUAGE) {
      d.language = ParseLanguageSpec();
    }
    switch (tok_) {
      case TokenKind::ALL: {
        auto* y = NewNode<nodes::DefKindExpr>([&](auto& dk) {
          // TODO: rework imports parsing according to the specs
          dk.kind = {};
          nodes::Expr* z = ParseAnyIdent();
          if (tok_ == TokenKind::EXCEPT) {
            z = NewNode<nodes::ExceptExpr>([&, pz = z](auto& ee) {
              ee.x = pz;
              ConsumeInvariant(TokenKind::EXCEPT);
              Expect(TokenKind::LBRACE);
              ee.list = ParseExceptStmts();
              Expect(TokenKind::RBRACE);
            });
          }
          dk.list = {z};
        });
        d.list = {y};
        break;
      }
      case TokenKind::LBRACE:
        ConsumeInvariant(TokenKind::LBRACE);
        while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
          d.list.push_back(ParseImportStmt());
          if (tok_ == TokenKind::RBRACE) {
            MaybeExpectSemi();
          } else {
            ExpectSemi();
          }
        }
        Expect(TokenKind::RBRACE);
        break;
      default:
        EmitErrorExpected("'all' or import spec");
        break;
    }
    d.with = ParseWith();
  });
}

nodes::DefKindExpr* Parser::ParseImportStmt() {
  return NewNode<nodes::DefKindExpr>([&](auto& dk) {
    switch (tok_) {
      case TokenKind::ALTSTEP:
      case TokenKind::CONST:
      case TokenKind::FUNCTION:
      case TokenKind::MODULEPAR:
      case TokenKind::SIGNATURE:
      case TokenKind::TEMPLATE:
      case TokenKind::TESTCASE:
      case TokenKind::TYPE: {
        dk.kind = Consume();
        if (tok_ == TokenKind::ALL) {
          nodes::Expr* y = ParseAnyIdent();
          if (tok_ == TokenKind::EXCEPT) {
            y = NewNode<nodes::ExceptExpr>([&, py = y](auto& ee) {
              ee.x = py;
              ConsumeInvariant(TokenKind::EXCEPT);
              ee.list = ParseRefList();
            });
          }
          dk.list = {y};
        } else {
          dk.list = ParseRefList();
        }
        break;
      }
      case TokenKind::GROUP: {
        dk.kind = Consume();
        while (true) {
          nodes::Expr* y = ParseTypeRef();
          if (tok_ == TokenKind::EXCEPT) {
            y = NewNode<nodes::ExceptExpr>([&, py = y](auto& ee) {
              ee.x = py;
              ConsumeInvariant(TokenKind::EXCEPT);
              Expect(TokenKind::LBRACE);
              ee.list = ParseExceptStmts();
              Expect(TokenKind::RBRACE);
            });
          }
          dk.list.push_back(y);
          if (tok_ != TokenKind::COMMA) {
            break;
          }
          Consume();
        }
        break;
      }
      case TokenKind::IMPORT:
        dk.kind = Consume();
        dk.list.push_back(ParseExpectedIdent(TokenKind::ALL));
        break;
      default:
        EmitErrorExpected("import definition qualifier");
        Advance(kTokStmtStart);
        break;
    }
  });
}

std::vector<nodes::Expr*> Parser::ParseExceptStmts() {
  std::vector<nodes::Expr*> v;
  while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
    v.push_back(ParseExceptStmt());
    ExpectSemi();
  }
  return v;
}

nodes::DefKindExpr* Parser::ParseExceptStmt() {
  return NewNode<nodes::DefKindExpr>([&](auto& dk) {
    switch (tok_) {
      case TokenKind::ALTSTEP:
      case TokenKind::CONST:
      case TokenKind::FUNCTION:
      case TokenKind::GROUP:
      case TokenKind::IMPORT:
      case TokenKind::MODULEPAR:
      case TokenKind::SIGNATURE:
      case TokenKind::TEMPLATE:
      case TokenKind::TESTCASE:
      case TokenKind::TYPE:
        dk.kind = Consume();
        break;
      default:
        EmitErrorExpected("definition qualifier");
        break;
    }

    if (tok_ == TokenKind::ALL) {
      dk.list.push_back(ParseAnyIdent());
    } else {
      dk.list = ParseRefList();
    }
  });
}

nodes::GroupDecl* Parser::ParseGroup() {
  return NewNode<nodes::GroupDecl>([&](auto& g) {
    ConsumeInvariant(TokenKind::GROUP);
    ParseName(g.name);
    Expect(TokenKind::LBRACE);
    while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
      g.defs.emplace_back(ParseModuleDef());
      ExpectSemi();
    }
    Expect(TokenKind::RBRACE);
    g.with = ParseWith();
  });
}

nodes::FriendDecl* Parser::ParseFriend() {
  return NewNode<nodes::FriendDecl>([&](auto& f) {
    ConsumeInvariant(TokenKind::FRIEND);
    Expect(TokenKind::MODULE);
    ParseName(f.module);
    f.with = ParseWith();
  });
}

nodes::LanguageSpec* Parser::ParseLanguageSpec() {
  return NewNode<nodes::LanguageSpec>([&](auto& s) {
    ConsumeInvariant(TokenKind::LANGUAGE);
    while (true) {
      s.list.push_back(Expect(TokenKind::STRING));
      if (tok_ != TokenKind::COMMA) {
        break;
      }
      Consume();
    }
  });
}

nodes::Decl* Parser::ParseTypeDecl() {
  // const auto& etok = Peek(1); // TODO: ErrorNode:start
  switch (Peek(2).kind) {
    case TokenKind::IDENT:
    case TokenKind::ADDRESS:
    case TokenKind::CHARSTRING:
    case TokenKind::kNULL:
    case TokenKind::UNIVERSAL:
      return ParseSubTypeDecl();
    case TokenKind::PORT:
      return ParsePortTypeDecl();
    case TokenKind::COMPONENT:
      return ParseComponentTypeDecl();
    case TokenKind::CLASS:
      return ParseClassTypeDecl();
    case TokenKind::UNION:
      return ParseStructTypeDecl();
    case TokenKind::MAP:
      return ParseMapTypeDecl();
    case TokenKind::SET:
    case TokenKind::RECORD: {
      const auto& p3k = Peek(3).kind;
      if (p3k == TokenKind::IDENT || p3k == TokenKind::ADDRESS) {
        return ParseStructTypeDecl();
      }
      // lists are also parsed by parseSubTypeDecl
      return ParseSubTypeDecl();
    }
    case TokenKind::ENUMERATED:
      return ParseEnumTypeDecl();
    case TokenKind::FUNCTION:
    case TokenKind::ALTSTEP:
    case TokenKind::TESTCASE:
      return ParseBehaviourTypeDecl();
    default:
      EmitErrorExpected("type definition");
      Advance(kTokStmtStart);
      return nullptr;
  }
}

nodes::SubTypeDecl* Parser::ParseSubTypeDecl() {
  return NewNode<nodes::SubTypeDecl>([&](auto& d) {
    Consume();
    d.field = ParseField();
    d.with = ParseWith();
  });
}

nodes::PortTypeDecl* Parser::ParsePortTypeDecl() {
  return NewNode<nodes::PortTypeDecl>([&](auto& ptd) {
    Consume();  // TypeTok
    Consume();  // PortTok
    ParseName(ptd.name);
    if (tok_ == TokenKind::LT) {
      ptd.pars = ParseTypeFormalPars();
    }

    switch (tok_) {
      case TokenKind::MIXED:
      case TokenKind::MESSAGE:
      case TokenKind::PROCEDURE:
        Consume();  // KindTok
        break;
      default:
        EmitErrorExpected("'message' or 'procedure'");
        break;
    }

    if (tok_ == TokenKind::REALTIME) {
      Consume();
      ptd.realtime = true;
    }

    Expect(TokenKind::LBRACE);
    while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
      ptd.attrs.push_back(ParsePortAttribute());
      ExpectSemi();
    }
    Expect(TokenKind::RBRACE);
    ptd.with = ParseWith();
  });
}

Node* Parser::ParsePortAttribute() {
  // const auto& etok = Peek(1);  // todo: errornode
  switch (tok_) {
    case TokenKind::IN:
    case TokenKind::OUT:
    case TokenKind::INOUT:
    case TokenKind::ADDRESS:
      return NewNode<nodes::PortAttribute>([&](auto& pa) {
        pa.kind = Consume();
        pa.types = ParseRefList();
      });
    case TokenKind::MAP:
    case TokenKind::UNMAP:
      return NewNode<nodes::PortMapAttribute>([&](auto& pma) {
        Consume();  // MapTok
        Expect(TokenKind::PARAM);
        pma.params = ParseFormalPars();
      });
    default:
      EmitErrorExpected("port attribute");
      Advance(kTokStmtStart);
      return NewErrorNode<Node>();
  }
}

nodes::ComponentTypeDecl* Parser::ParseComponentTypeDecl() {
  return NewNode<nodes::ComponentTypeDecl>([&](auto& ctd) {
    Consume();  // TypeTok
    Consume();  // CompTok
    ParseName(ctd.name);
    if (tok_ == TokenKind::LT) {
      ctd.pars = ParseTypeFormalPars();
    }
    if (tok_ == TokenKind::EXTENDS) {
      Consume();  // ExtendsTok
      ctd.extends = ParseRefList();
    }
    ctd.body = ParseBlockStmt();
    ctd.with = ParseWith();
  });
}

nodes::StructTypeDecl* Parser::ParseStructTypeDecl() {
  return NewNode<nodes::StructTypeDecl>([&](auto& s) {
    Consume();           // TypeTok
    s.kind = Consume();  // KindTok
    ParseName(s.name);
    if (tok_ == TokenKind::LT) {
      s.pars = ParseTypeFormalPars();
    }
    Expect(TokenKind::LBRACE);
    while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
      s.fields.push_back(ParseField());
      if (tok_ != TokenKind::COMMA) {
        break;
      }
      Consume();
    }
    Expect(TokenKind::RBRACE);
    s.with = ParseWith();
  });
}

nodes::ClassTypeDecl* Parser::ParseClassTypeDecl() {
  return NewNode<nodes::ClassTypeDecl>([&](auto& ctd) {
    Consume();             // TypeTok
    ctd.kind = Consume();  // KindTok
    if (tok_ == TokenKind::MODIF) {
      ctd.modif = TokAlloc(Consume());
    }
    ParseName(ctd.name);
    if (tok_ == TokenKind::EXTENDS) {
      Consume();  // ExtendsTok
      ctd.extends = ParseRefList();
    }
    if (tok_ == TokenKind::RUNS) {
      ctd.runs_on = ParseRunsOn();
    }
    if (tok_ == TokenKind::MTC) {
      ctd.mtc = ParseMtc();
    }
    if (tok_ == TokenKind::SYSTEM) {
      ctd.system = ParseSystem();
    }
    Expect(TokenKind::LBRACE);
    while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
      ctd.defs.push_back(ParseModuleDef());
      ExpectSemi();
    }
    Expect(TokenKind::RBRACE);
    ctd.with = ParseWith();
  });
}

nodes::MapTypeDecl* Parser::ParseMapTypeDecl() {
  return NewNode<nodes::MapTypeDecl>([&](auto& mtd) {
    Consume();  // TypeTok
    mtd.spec = ParseMapSpec();
    ParseName(mtd.name);
    if (tok_ == TokenKind::LT) {
      mtd.pars = ParseTypeFormalPars();
    }
    mtd.with = ParseWith();
  });
}

nodes::EnumTypeDecl* Parser::ParseEnumTypeDecl() {
  return NewNode<nodes::EnumTypeDecl>([&](auto& etd) {
    Consume();  // TypeTok
    Consume();  // EnumTok
    ParseName(etd.name);
    if (tok_ == TokenKind::LT) {
      etd.pars = ParseTypeFormalPars();
    }
    Expect(TokenKind::LBRACE);
    while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
      etd.enums.push_back(ParseEnum());
      if (tok_ != TokenKind::COMMA) {
        break;
      }
      Consume();
    }
    Expect(TokenKind::RBRACE);
    etd.with = ParseWith();
  });
}

nodes::Expr* Parser::ParseEnum() {
  // const auto firstIdent = [](this auto& self, nodes::Expr* e) -> nodes::Expr* {
  // switch (e->nkind) {
  //   case NodeKind::CallExpr:
  //     return self(e->As<nodes::CallExpr>()->fun);
  //   case NodeKind::SelectorExpr:
  //     return self(e->As<nodes::SelectorExpr>()->x);
  //   case NodeKind::Ident:
  //     return e;
  //   default:
  //     return nullptr;
  // }

  // auto* x = ParseExpr();
  return ParseExpr();
}

nodes::BehaviourTypeDecl* Parser::ParseBehaviourTypeDecl() {
  return NewNode<nodes::BehaviourTypeDecl>([&](auto& btd) {
    Consume();  // TypeTok
    Consume();  // KindTok
    ParseName(btd.name);
    if (tok_ == TokenKind::LT) {
      btd.pars = ParseTypeFormalPars();
    }
    btd.params = ParseFormalPars();
    if (tok_ == TokenKind::RUNS) {
      btd.runs_on = ParseRunsOn();
    }
    if (tok_ == TokenKind::SYSTEM) {
      btd.system = ParseSystem();
    }
    if (tok_ == TokenKind::RETURN) {
      btd.ret = ParseReturn();
    }
    btd.with = ParseWith();
  });
}

nodes::Field* Parser::ParseField() {
  return NewNode<nodes::Field>([&](auto& f) {
    if (tok_ == TokenKind::MODIF) {
      if (Lit(1) != "@default") {
        EmitErrorExpected("@default");
      }
      f.default_tok = TokAlloc(Consume());  // DefaultTok
    }
    f.type = ParseTypeSpec();
    ParseName(f.name);
    if (tok_ == TokenKind::LT) {
      f.pars = ParseTypeFormalPars();
    }
    if (tok_ == TokenKind::LBRACK) {
      f.arraydef = ParseArrayDefs();
    }
    if (tok_ == TokenKind::LPAREN) {
      f.value_constraint = ParseParenExpr();
    }
    if (tok_ == TokenKind::OPTIONAL) {
      Consume();
      f.optional = true;
    }
  });
}

nodes::TypeSpec* Parser::ParseTypeSpec() {
  // const auto& etok = Peek(1); // todo: errornode
  switch (tok_) {
    case TokenKind::ADDRESS:
    case TokenKind::CHARSTRING:
    case TokenKind::IDENT:
    case TokenKind::kNULL:
    case TokenKind::UNIVERSAL:
      return NewNode<nodes::RefSpec>([&](auto& rs) {
        rs.x = ParseTypeRef();
      });
    case TokenKind::UNION:
      return ParseStructSpec();
    case TokenKind::SET:
    case TokenKind::RECORD:
      if (Peek(2).kind == TokenKind::LBRACE) {
        return ParseStructSpec();
      }
      return ParseListSpec();
    case TokenKind::MAP:
      return ParseMapSpec();
    case TokenKind::ENUMERATED:
      return ParseEnumSpec();
    case TokenKind::FUNCTION:
    case TokenKind::ALTSTEP:
    case TokenKind::TESTCASE:
      return ParseBehaviourSpec();
    default:
      EmitErrorExpected("type definition");
      return NewErrorNode<nodes::TypeSpec>();
  }
}

nodes::StructSpec* Parser::ParseStructSpec() {
  return NewNode<nodes::StructSpec>([&](auto& ss) {
    Consume();  // KindTok
    Expect(TokenKind::LBRACE);
    while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
      ss.fields.push_back(ParseField());
      if (tok_ != TokenKind::COMMA) {
        break;
      }
      Consume();
    }
    Expect(TokenKind::RBRACE);
  });
}

nodes::MapSpec* Parser::ParseMapSpec() {
  return NewNode<nodes::MapSpec>([&](auto& ms) {
    Consume();  // MapTok
    Expect(TokenKind::FROM);
    ms.from = ParseTypeSpec();
    Expect(TokenKind::TO);
    ms.to = ParseTypeSpec();
  });
}

nodes::EnumSpec* Parser::ParseEnumSpec() {
  return NewNode<nodes::EnumSpec>([&](auto& es) {
    Consume();  // Tok
    Expect(TokenKind::LBRACE);
    while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
      es.enums.push_back(ParseEnum());
      if (tok_ != TokenKind::COMMA) {
        break;
      }
      Consume();
    }
    Expect(TokenKind::RBRACE);
  });
}

nodes::ListSpec* Parser::ParseListSpec() {
  return NewNode<nodes::ListSpec>([&](auto& ls) {
    Consume();  // KindTok
    if (tok_ == TokenKind::LENGTH) {
      ls.length = ParseLengthExpr(nullptr);
    }
    Expect(TokenKind::OF);
    ls.elemtype = ParseTypeSpec();
  });
}

nodes::BehaviourSpec* Parser::ParseBehaviourSpec() {
  return NewNode<nodes::BehaviourSpec>([&](auto& bs) {
    Consume();  // KindTok
    bs.params = ParseFormalPars();
    if (tok_ == TokenKind::RUNS) {
      bs.runs_on = ParseRunsOn();
    }
    if (tok_ == TokenKind::SYSTEM) {
      bs.system = ParseSystem();
    }
    if (tok_ == TokenKind::RETURN) {
      bs.ret = ParseReturn();
    }
  });
}

nodes::TemplateDecl* Parser::ParseTemplateDecl() {
  return NewNode<nodes::TemplateDecl>([&](auto& td) {
    Consume();  // TemplateTok
    if (tok_ == TokenKind::LPAREN) {
      Consume();  // LParen
      td.restriction = NewNode<nodes::RestrictionSpec>([&](auto& n) {
        n.type = Consume();  // omit/value/...
      });
      Expect(TokenKind::RPAREN);
    }
    if (tok_ == TokenKind::MODIF) {
      td.modif = TokAlloc(Consume());
    }
    td.type = ParseTypeRef();
    if (td.type == nullptr) {
      // ^ check for errornode instead
      return;
    }
    ParseName(td.name);
    if (tok_ == TokenKind::LT) {
      td.pars = ParseTypeFormalPars();
    }
    if (tok_ == TokenKind::LPAREN) {
      td.params = ParseFormalPars();
    }
    if (tok_ == TokenKind::MODIFIES) {
      Consume();
      td.base = ParsePrimaryExpr();
    }
    Expect(TokenKind::ASSIGN);
    td.value = ParseExpr();
    td.with = ParseWith();
  });
}

nodes::ValueDecl* Parser::ParseValueDecl() {
  return NewNode<nodes::ValueDecl>([&](auto& vd) {
    if (tok_ != TokenKind::TIMER) {
      vd.kind = TokAlloc(Consume());
      vd.template_restriction = ParseRestrictionSpec();
      if (tok_ == TokenKind::MODIF) {
        vd.modif = TokAlloc(Consume());
      }
    }
    vd.type = ParseTypeRef();
    vd.decls = ParseDeclList();
    vd.with = ParseWith();
  });
}

nodes::RestrictionSpec* Parser::ParseRestrictionSpec() {
  switch (tok_) {
    case TokenKind::TEMPLATE:
      return NewNode<nodes::RestrictionSpec>([&](auto& rs) {
        Consume();  // TemplateTok
        rs.is_template = true;
        if (tok_ == TokenKind::LPAREN) {
          Consume();
          rs.type = Consume();
          Expect(TokenKind::RPAREN);
        } else {
          rs.type = {};
        }
      });
    case TokenKind::OMIT:
    case TokenKind::VALUE:
    case TokenKind::PRESENT:
      return NewNode<nodes::RestrictionSpec>([&](auto& rs) {
        rs.type = Consume();
      });
    default:
      return nullptr;
  }
}

std::vector<nodes::Declarator*> Parser::ParseDeclList() {
  std::vector<nodes::Declarator*> v;
  v.push_back(ParseDeclarator());
  while (tok_ == TokenKind::COMMA) {
    Consume();
    v.push_back(ParseDeclarator());
  }
  return v;
}

nodes::Declarator* Parser::ParseDeclarator() {
  return NewNode<nodes::Declarator>([&](auto& d) {
    ParseName(d.name);
    if (tok_ == TokenKind::LBRACK) {
      d.arraydef = ParseArrayDefs();
    }
    if (tok_ == TokenKind::ASSIGN) {
      Consume();
      d.value = ParseExpr();
    }
  });
}

nodes::FuncDecl* Parser::ParseFuncDecl() {
  return NewNode<nodes::FuncDecl>([&](auto& d) {
    d.kind = Consume();
    if (tok_ == TokenKind::MODIF) {
      d.modif = TokAlloc(Consume());
    }
    ParseName(d.name);
    if (tok_ == TokenKind::LT) {
      d.pars = ParseTypeFormalPars();
    }
    d.params = ParseFormalPars();
    if (tok_ == TokenKind::RUNS) {
      d.runs_on = ParseRunsOn();
    }
    if (tok_ == TokenKind::MTC) {
      d.mtc = ParseMtc();
    }
    if (tok_ == TokenKind::SYSTEM) {
      d.system = ParseSystem();
    }
    if (tok_ == TokenKind::RETURN) {
      d.ret = ParseReturn();
    }
    if (tok_ == TokenKind::LBRACE) {
      d.body = ParseBlockStmt();
    }
    d.with = ParseWith();
  });
}

nodes::FuncDecl* Parser::ParseExtFuncDecl() {
  return NewNode<nodes::FuncDecl>([&](auto& d) {
    Consume();  // ExternalTok
    d.external = true;

    d.kind = Consume();
    if (tok_ == TokenKind::MODIF) {
      d.modif = TokAlloc(Consume());
    }
    ParseName(d.name);
    d.params = ParseFormalPars();
    if (tok_ == TokenKind::RUNS) {
      d.runs_on = ParseRunsOn();
    }
    if (tok_ == TokenKind::MTC) {
      d.mtc = ParseMtc();
    }
    if (tok_ == TokenKind::SYSTEM) {
      d.system = ParseSystem();
    }
    if (tok_ == TokenKind::RETURN) {
      d.ret = ParseReturn();
    }
    d.with = ParseWith();
  });
}

nodes::ConstructorDecl* Parser::ParseConstructorDecl() {
  return NewNode<nodes::ConstructorDecl>([&](auto& d) {
    Consume();  // CreateTok
    d.params = ParseFormalPars();
    d.body = ParseBlockStmt();
  });
}

nodes::SignatureDecl* Parser::ParseSignatureDecl() {
  return NewNode<nodes::SignatureDecl>([&](auto& d) {
    Consume();  // Tok
    ParseName(d.name);
    if (tok_ == TokenKind::LT) {
      d.pars = ParseTypeFormalPars();
    }
    d.params = ParseFormalPars();
    if (tok_ == TokenKind::NOBLOCK) {
      Consume();
      d.noblock = true;
    }
    if (tok_ == TokenKind::RETURN) {
      d.ret = ParseReturn();
    }
    if (tok_ == TokenKind::EXCEPTION) {
      Consume();
      d.exception = ParseParenExpr();
    }
    d.with = ParseWith();
  });
}

nodes::ReturnSpec* Parser::ParseReturn() {
  return NewNode<nodes::ReturnSpec>([&](auto& r) {
    Consume();  // Tok
    r.restriction = ParseRestrictionSpec();
    if (tok_ == TokenKind::MODIF) {
      r.modif = TokAlloc(Consume());
    }
    r.type = ParseTypeRef();
  });
}

nodes::FormalPars* Parser::ParseFormalPars() {
  return NewNode<nodes::FormalPars>([&](auto& fp) {
    Expect(TokenKind::LPAREN);
    while (tok_ != TokenKind::RPAREN) {
      fp.list.push_back(ParseFormalPar());
      if (tok_ != TokenKind::COMMA) {
        break;
      }
      Consume();
    }
    Expect(TokenKind::RPAREN);
  });
}

nodes::FormalPar* Parser::ParseFormalPar() {
  return NewNode<nodes::FormalPar>([&](auto& fp) {
    switch (tok_) {
      case TokenKind::IN:
      case TokenKind::OUT:
      case TokenKind::INOUT:  // TODO
        fp.direction = TokAlloc(Consume());
        break;
      default:
        break;
    }
    fp.restriction = ParseRestrictionSpec();
    if (tok_ == TokenKind::MODIF) {
      fp.modif = TokAlloc(Consume());
    }
    fp.type = ParseTypeRef();
    ParseName(fp.name);
    if (tok_ == TokenKind::LBRACK) {
      fp.arraydef = ParseArrayDefs();
    }
    if (tok_ == TokenKind::ASSIGN) {
      Consume();
      fp.value = ParseExpr();
    }
  });
}

nodes::FormalPars* Parser::ParseTypeFormalPars() {
  return NewNode<nodes::FormalPars>([&](auto& p) {
    Expect(TokenKind::GT);
    while (tok_ != TokenKind::GT) {
      p.list.push_back(ParseTypeFormalPar());
      if (tok_ != TokenKind::COMMA) {
        break;
      }
      Consume();
    }
    Expect(TokenKind::GT);
  });
}

nodes::FormalPar* Parser::ParseTypeFormalPar() {
  return NewNode<nodes::FormalPar>([&](auto& p) {
    if (tok_ == TokenKind::IN) {
      p.direction = TokAlloc(Consume());
    }

    switch (tok_) {
      case TokenKind::TYPE:
      case TokenKind::SIGNATURE:
        p.type = ParseAnyIdent();
        break;
      default:
        p.type = ParseTypeRef();
        break;
    }

    if (tok_ == TokenKind::ASSIGN) {
      Consume();
      p.value = ParseTypeRef();
    }
  });
}

std::vector<nodes::ParenExpr*> Parser::ParseArrayDefs() {
  std::vector<nodes::ParenExpr*> v;
  while (tok_ == TokenKind::LBRACK) {
    v.push_back(ParseArrayDef());
  }
  return v;
}

nodes::ParenExpr* Parser::ParseArrayDef() {
  return NewNode<nodes::ParenExpr>([&](auto& e) {
    Expect(TokenKind::LBRACK);
    e.list = ParseExprList();
    Expect(TokenKind::RBRACK);
  });
}

nodes::RunsOnSpec* Parser::ParseRunsOn() {
  return NewNode<nodes::RunsOnSpec>([&](auto& s) {
    Expect(TokenKind::RUNS);
    Expect(TokenKind::ON);
    s.comp = ParseAnyIdent();
  });
}

nodes::SystemSpec* Parser::ParseSystem() {
  return NewNode<nodes::SystemSpec>([&](auto& s) {
    Expect(TokenKind::SYSTEM);
    s.comp = ParseAnyIdent();
  });
}

nodes::MtcSpec* Parser::ParseMtc() {
  return NewNode<nodes::MtcSpec>([&](auto& s) {
    Expect(TokenKind::MTC);
    s.comp = ParseAnyIdent();
  });
}

nodes::Ident* Parser::ParseName() {
  switch (tok_) {
    case TokenKind::IDENT:
    case TokenKind::CREATE:
    case TokenKind::ADDRESS:
    case TokenKind::CONTROL:
    case TokenKind::CLASS:
      return NewNode<nodes::Ident>([&](auto&) {
        Consume();
      });
    default:
      Expect(TokenKind::IDENT);
      return nullptr;
  }
}

void Parser::ParseName(std::optional<nodes::Ident>& ident) {
  switch (tok_) {
    case TokenKind::IDENT:
    case TokenKind::ADDRESS:
    case TokenKind::CONTROL:
    case TokenKind::CLASS:
      ident.emplace();
      ident->parent = last_node_;
      ident->nrange = Consume().range;
      break;
    default:
      Expect(TokenKind::IDENT);
      break;
  }
}

nodes::Ident* Parser::ParseAnyIdent() {
  return NewNode<nodes::Ident>([&](auto&) {
    Consume();
  });
}

nodes::Ident* Parser::ParseExpectedIdent(TokenKind kind) {
  return NewNode<nodes::Ident>([&](auto&) {
    Expect(kind);
  });
}

nodes::WithSpec* Parser::ParseWith() {
  if (tok_ != TokenKind::WITH) {
    return nullptr;
  }
  return NewNode<nodes::WithSpec>([&](auto& w) {
    Consume();  // tok
    Expect(TokenKind::LBRACE);
    while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
      w.list.emplace_back(ParseWithStmt());
      MaybeExpectSemi();
    }
    Expect(TokenKind::RBRACE);
  });
}

nodes::WithStmt* Parser::ParseWithStmt() {
  return NewNode<nodes::WithStmt>([&](auto& s) {
    switch (tok_) {
      case TokenKind::ENCODE:
      case TokenKind::VARIANT:
      case TokenKind::DISPLAY:
      case TokenKind::EXTENSION:
      case TokenKind::OPTIONAL:
      case TokenKind::STEPSIZE:
      case TokenKind::OVERRIDE:
        s.kind = Consume();
        break;
      default:
        EmitErrorExpected("with-attribute");
        Advance(kTokStmtStart);
        break;
    }

    switch (tok_) {
      case TokenKind::OVERRIDE:
        s.overrides = true;
        break;
      case TokenKind::MODIF:
        // TODO: CHECK THIS CODE BLOCK
        if (Lit(1) != "@local") {
          EmitErrorExpected("@local");
        }
        s.overrides = true;
        break;
      default:
        break;
    }

    if (tok_ == TokenKind::LPAREN) {
      Consume();
      while (true) {
        s.list.push_back(ParseWithQualifier());
        if (tok_ != TokenKind::COMMA) {
          break;
        }
        Consume();
      }
      Expect(TokenKind::RPAREN);
    }

    auto* v = NewNode<nodes::ValueLiteral>([&](auto& p) {
      p.tok = Expect(TokenKind::STRING);
    });
    if (tok_ == TokenKind::DOT) {
      s.value = NewNode<nodes::SelectorExpr>([&](auto& se) {
        se.x = v;
        ConsumeInvariant(TokenKind::DOT);
        // TODO: check whether this is really possible
        // se.sel = NewNode<nodes::ValueLiteral>([&](auto& sel) {
        //   sel.tok = Expect(TokenKind::STRING);
        // });
        se.sel = NewNode<nodes::Ident>([&](auto&) {
          Expect(TokenKind::STRING);
        });
      });
    } else {
      s.value = v;
    }
  });
}

nodes::Expr* Parser::ParseWithQualifier() {
  // const auto& etok = Peek(1);  // todo: errornode
  switch (tok_) {
    case TokenKind::IDENT:
      return ParseTypeRef();
    case TokenKind::LBRACK:
      return ParseIndexExpr(nullptr);
    case TokenKind::TYPE:
    case TokenKind::TEMPLATE:
    case TokenKind::CONST:
    case TokenKind::ALTSTEP:
    case TokenKind::TESTCASE:
    case TokenKind::FUNCTION:
    case TokenKind::SIGNATURE:
    case TokenKind::MODULEPAR:
    case TokenKind::GROUP: {
      return NewNode<nodes::DefKindExpr>([&](auto& p) {
        p.kind = Consume();
        nodes::Expr* y = ParseExpectedIdent(TokenKind::ALL);
        if (tok_ == TokenKind::EXCEPT) {
          y = NewNode<nodes::ExceptExpr>([&](auto& ee) {
            ee.x = y;
            Consume();  // except
            Expect(TokenKind::LBRACE);
            ee.list = ParseRefList();
            Expect(TokenKind::RBRACE);
          });
        }
        p.list.push_back(y);
      });
    }
    default:
      EmitErrorExpected("with-qualifier");
      Advance(kTokStmtStart);
      return NewErrorNode<nodes::Expr>();
  }
}

std::vector<nodes::Expr*> Parser::ParseRefList() {
  std::vector<nodes::Expr*> v;
  while (true) {
    v.push_back(ParseTypeRef());
    if (tok_ != TokenKind::COMMA) {
      break;
    }
    Consume();
  }
  return v;
}

nodes::Expr* Parser::ParseTypeRef() {
  return ParsePrimaryExpr();
}

nodes::Stmt* Parser::ParseStmt() {
  switch (Peek(1).kind) {
    case TokenKind::TEMPLATE:
      return NewNode<nodes::DeclStmt>([&](auto& s) {
        s.decl = ParseTemplateDecl();
      });

    case TokenKind::VAR:
    case TokenKind::CONST:
    case TokenKind::TIMER:
    case TokenKind::PORT:
      return NewNode<nodes::DeclStmt>([&](auto& s) {
        s.decl = ParseValueDecl();
      });

    case TokenKind::REPEAT:
    case TokenKind::BREAK:
    case TokenKind::CONTINUE:
      return NewNode<nodes::BranchStmt>([&](auto& s) {
        s.kind = Consume();
      });

    case TokenKind::LABEL:
    case TokenKind::GOTO:
      return NewNode<nodes::BranchStmt>([&](auto& s) {
        s.kind = Consume();
        s.label = ParseName();
      });

    case TokenKind::RETURN:
      return NewNode<nodes::ReturnStmt>([&](auto& s) {
        Consume();
        if (!kTokStmtStart.contains(tok_) && tok_ != TokenKind::SEMICOLON && tok_ != TokenKind::RBRACE) {
          s.result = ParseExpr();
        }
      });

    case TokenKind::SELECT:
      return ParseSelect();

    case TokenKind::ALT:
    case TokenKind::INTERLEAVE:
      return NewNode<nodes::AltStmt>([&](auto& s) {
        s.kind = Consume();  // alt/interleave TODO: enum
        if (tok_ == TokenKind::MODIF) {
          s.no_default = TokAlloc(Consume());
        }
        s.body = ParseBlockStmt();
      });

    case TokenKind::LBRACK:
      return ParseAltGuard();

    case TokenKind::FOR:
      return ParseForLoop();

    case TokenKind::WHILE:
      return ParseWhileLoop();

    case TokenKind::DO:
      return ParseDoWhileLoop();

    case TokenKind::IF:
      return ParseIfStmt();

    case TokenKind::LBRACE:
      return ParseBlockStmt();

    case TokenKind::IDENT:
    case TokenKind::TESTCASE:
    case TokenKind::ANYKW:
    case TokenKind::ALL:
    case TokenKind::MAP:
    case TokenKind::UNMAP:
    case TokenKind::MTC: {
      auto* x = ParseSimpleStmt();

      if (tok_ == TokenKind::LBRACE) {
        if (x->nkind != NodeKind::CallExpr) {
          return x;
        }
        auto* cef = x->As<nodes::CallExpr>()->fun;
        if (cef->nkind != NodeKind::SelectorExpr) {
          return x;
        }
        auto* cefi = cef->As<nodes::SelectorExpr>()->sel;
        if (cefi->nkind != NodeKind::Ident) {
          return x;
        }
        if (cefi->As<nodes::Ident>()->On(src_) != "call") {
          return x;
        }
        return NewNode<nodes::CallStmt>([&](auto& cs) {
          cs.stmt = x;
          cs.body = ParseBlockStmt();
        });
      }

      return x;
    }

    case TokenKind::INT:
    case TokenKind::FLOAT:
    case TokenKind::STRING:
    case TokenKind::BITSTRING:
    case TokenKind::TRUE:
    case TokenKind::FALSE:
    case TokenKind::PASS:
    case TokenKind::FAIL:
    case TokenKind::NONE:
    case TokenKind::INCONC:
    case TokenKind::ERROR:
      return ParseSimpleStmt();

    default:
      EmitErrorExpected("statement");
      Advance(kTokStmtStart);
      return NewErrorNode<nodes::Stmt>();
  }
}

nodes::BlockStmt* Parser::ParseBlockStmt() {
  return NewNode<nodes::BlockStmt>([&](auto& s) {
    Expect(TokenKind::LBRACE);
    while (tok_ != TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
      s.stmts.push_back(ParseStmt());
      ExpectSemi();
    }
    Expect(TokenKind::RBRACE);
  });
}

nodes::Stmt* Parser::ParseForLoop() {  // CRITICAL TODO : for loop
  const auto begin_pos = ConsumeInvariant(TokenKind::FOR).range.begin;
  Expect(TokenKind::LPAREN);

  nodes::Stmt* init;
  if (tok_ == TokenKind::VAR) {
    init = NewNode<nodes::DeclStmt>([&](auto& s) {
      s.decl = ParseValueDecl();
    });
  } else {
    init = NewNode<nodes::ExprStmt>([&](auto& s) {
      s.expr = ParseExpr();
    });
  }

  nodes::Stmt* n;
  if (tok_ == TokenKind::IN) {
    n = NewNode<nodes::ForRangeStmt>([&](auto& s) {
      s.init = init;
      ConsumeInvariant(TokenKind::IN);
      s.range = ParseExpr();
      Expect(TokenKind::RPAREN);
      s.body = ParseBlockStmt();
    });
  } else {
    n = NewNode<nodes::ForStmt>([&](auto& s) {
      s.init = init;
      Expect(TokenKind::SEMICOLON);
      s.cond = ParseExpr();
      Expect(TokenKind::SEMICOLON);
      s.post = ParseSimpleStmt();
      Expect(TokenKind::RPAREN);
      s.body = ParseBlockStmt();
    });
  }

  n->nrange.begin = begin_pos;
  return n;
}

nodes::WhileStmt* Parser::ParseWhileLoop() {
  return NewNode<nodes::WhileStmt>([&](auto& s) {
    ConsumeInvariant(TokenKind::WHILE);
    Expect(TokenKind::LPAREN);
    s.cond = ParseExpr();
    Expect(TokenKind::RPAREN);
    s.body = ParseBlockStmt();
  });
}

nodes::DoWhileStmt* Parser::ParseDoWhileLoop() {
  return NewNode<nodes::DoWhileStmt>([&](auto& s) {
    ConsumeInvariant(TokenKind::DO);
    s.body = ParseBlockStmt();
    Expect(TokenKind::WHILE);
    Expect(TokenKind::LPAREN);
    s.cond = ParseExpr();
    Expect(TokenKind::RPAREN);
  });
}

nodes::IfStmt* Parser::ParseIfStmt() {
  return NewNode<nodes::IfStmt>([&](auto& s) {
    ConsumeInvariant(TokenKind::IF);
    Expect(TokenKind::LPAREN);
    s.cond = ParseExpr();
    Expect(TokenKind::RPAREN);
    s.consequent = ParseBlockStmt();

    if (tok_ == TokenKind::ELSE) {
      Consume();
      if (tok_ == TokenKind::IF) {
        s.alternate = ParseIfStmt();
      } else {
        s.alternate = ParseBlockStmt();
      }
    }
  });
}

nodes::SelectStmt* Parser::ParseSelect() {
  return NewNode<nodes::SelectStmt>([&](auto& s) {
    Expect(TokenKind::SELECT);
    if (tok_ == TokenKind::UNION) {
      Consume();
      s.is_union = true;
    }
    s.tag = ParseParenExpr();
    Expect(TokenKind::LBRACE);
    while (tok_ == TokenKind::CASE) {
      s.clauses.push_back(ParseCaseClause());
    }
    Expect(TokenKind::RBRACE);
  });
}

nodes::CaseClause* Parser::ParseCaseClause() {
  return NewNode<nodes::CaseClause>([&](auto& c) {
    Expect(TokenKind::CASE);
    if (tok_ == TokenKind::ELSE) {
      Consume();
    } else {
      c.cond = ParseParenExpr();
    }
    c.body = ParseBlockStmt();
    MaybeExpectSemi();  // TODO REMOVE
  });
}

nodes::CommClause* Parser::ParseAltGuard() {
  return NewNode<nodes::CommClause>([&](auto& c) {
    Expect(TokenKind::LBRACK);
    if (tok_ == TokenKind::ELSE) {
      Consume();
      c.is_else = true;
      Expect(TokenKind::RBRACK);
      c.body = ParseBlockStmt();
      return;
    }

    if (tok_ != TokenKind::RBRACK) {
      c.x = ParseExpr();
    }
    Expect(TokenKind::RBRACK);
    c.comm = ParseSimpleStmt();
    if (tok_ == TokenKind::LBRACE) {
      c.body = ParseBlockStmt();
    }
  });
}

nodes::Stmt* Parser::ParseSimpleStmt() {
  return NewNode<nodes::ExprStmt>([&](auto& s) {
    s.expr = ParseExpr();
  });
}

// Expr ::= BinaryExpr
nodes::Expr* Parser::ParseExpr() {
  return ParseBinaryExpr(kLowestPrec + 1);
}

// BinaryExpr ::= UnaryExpr
//
//	| BinaryExpr OP BinaryExpr
nodes::Expr* Parser::ParseBinaryExpr(int prec) {
  auto* x = ParsePostfixExpr();
  while (true) {
    const auto tprec = PrecedenceOf(tok_);
    if (tprec < prec) {
      return x;
    }

    if (tok_ == TokenKind::ASSIGN) {
      if (x->nkind != NodeKind::Ident && x->parent->nkind == NodeKind::CompositeLiteral &&
          x->nkind != NodeKind::IndexExpr) {
        EmitError(x->nrange, "identifier expected");
      }
      x = NewNode<nodes::AssignmentExpr>([&, px = x](auto& be) {
        be.property = px;
        ConsumeInvariant(TokenKind::ASSIGN);
        be.value = ParseBinaryExpr(prec + 1);
      });
      ExtendByIncorporatedNode(x, x->As<nodes::AssignmentExpr>()->property);
      continue;
    }

    x = NewNode<nodes::BinaryExpr>([&, px = x](auto& be) {
      be.x = px;
      be.op = Consume();
      be.y = ParseBinaryExpr(prec + 1);
    });
    ExtendByIncorporatedNode(x, x->As<nodes::BinaryExpr>()->x);
  }
}

nodes::Expr* Parser::ParsePostfixExpr() {
  auto* x = ParseUnaryExpr();

  if (tok_ == TokenKind::INC || tok_ == TokenKind::DEC) {
    x = NewNode<nodes::PostExpr>([&, px = x](auto& pe) {
      pe.x = px;
      pe.op = Consume();
    });
  }

  if (tok_ == TokenKind::LENGTH) {
    x = ParseLengthExpr(x);
  }

  if (tok_ == TokenKind::IFPRESENT) {
    x = NewNode<nodes::UnaryExpr>([&, px = x](auto& ue) {
      ue.op = Consume();
      ue.x = px;
    });
  }

  if (tok_ == TokenKind::TO || tok_ == TokenKind::FROM) {
    x = NewNode<nodes::BinaryExpr>([&, px = x](auto& be) {
      be.x = px;
      be.op = Consume();
      be.y = ParseExpr();
    });
  }

  if (tok_ == TokenKind::REDIR) {
    x = ParseRedirect(x);
  }

  if (tok_ == TokenKind::VALUE) {
    x = NewNode<nodes::ValueExpr>([&, px = x](auto& ve) {
      ve.x = px;
      Consume();
      ve.y = ParseExpr();
    });
  }

  if (tok_ == TokenKind::PARAM) {
    x = NewNode<nodes::ParamExpr>([&, px = x](auto& pe) {
      pe.x = px;
      Consume();
      pe.y = ParseParenExpr();
    });
  }

  if (tok_ == TokenKind::ALIVE) {
    x = NewNode<nodes::UnaryExpr>([&, px = x](auto& ue) {
      ue.op = Consume();
      ue.x = px;
    });
  }

  return x;
}

// UnaryExpr ::= "-"
//
//	| ("-"|"+"|"!"|"not"|"not4b") UnaryExpr
//	| PrimaryExpr
nodes::Expr* Parser::ParseUnaryExpr() {
  switch (tok_) {
    case TokenKind::ADD:
    case TokenKind::EXCL:
    case TokenKind::NOT:
    case TokenKind::NOT4B:
    case TokenKind::SUB:
    case TokenKind::INC:
    case TokenKind::DEC: {
      const auto& tok = Consume();
      // handle unused '-'
      if (tok.kind == TokenKind::SUB) {
        switch (tok_) {
          case TokenKind::COMMA:
          case TokenKind::SEMICOLON:
          case TokenKind::RBRACE:
          case TokenKind::RBRACK:
          case TokenKind::RPAREN:
          case TokenKind::kEOF:
            return NewNode<nodes::ValueLiteral>([&](auto& v) {
              v.tok = tok;
            });
          default:
            break;
        }
      }
      return NewNode<nodes::UnaryExpr>([&](auto& ue) {
        ue.op = tok;
        ue.x = ParseUnaryExpr();
      });
    }

    case TokenKind::COLONCOLON: {
      const auto& tok = Consume();
      return NewNode<nodes::BinaryExpr>([&](auto& be) {
        be.op = tok;
        be.x = ParseExpr();
      });
    }

    default:
      return ParsePrimaryExpr();
  }
}

// PrimaryExpr ::= Operand [{ExtFieldRef}] [Stuff]
//
// ExtFieldRef ::= "." ID
//               | "[" Expr "]"
//               | "(" ExprList ")"
//
// Stuff       ::= ["length"      "(" ExprList ")"]
//                 ["ifpresent"                   ]
//                 [("to"|"from") Expr            ]
//                 ["->"          Redirect        ]

// Redirect    ::= ["value"            ExprList   ]
//
//	["param"            ExprList   ]
//	["sender"           PrimaryExpr]
//	["@index" ["value"] PrimaryExpr]
//	["timestamp"        PrimaryExpr]
nodes::Expr* Parser::ParsePrimaryExpr() {
  auto* x = ParseOperand();
  while (true) {
    switch (tok_) {
      case TokenKind::DOT:
        x = ParseSelectorExpr(x);
        break;
      case TokenKind::LBRACK:
        x = ParseIndexExpr(x);
        break;
      case TokenKind::LPAREN:
        x = ParseCallExpr(x);
        // Not supporting chained function calls like 'get().x'
        // eliminates conflicts with alt-guards.
        break;
      default:
        goto exit_loop;
    }
  }

exit_loop:
  return x;
}

// Operand ::= ("any"|"all") ("component"|"port"|"timer"|"from" PrimaryExpr)
//
//	| Literal
//	| Reference
//
// Literal ::= INT | STRING | BITSTRING | FLOAT
//
//	| "?" | "*"
//	| "none" | "inconc" | "pass" | "fail" | "error"
//	| "true" | "false"
//	| "not_a_number"
//
// Reference ::= ID
//
//	| "address" | ["unviersal"] "charstring" | "timer"
//	| "null" | "omit"
//	| "mtc" | "system" | "testcase"
//	| "map" | "unmap"
nodes::Expr* Parser::ParseOperand() {
  // const auto& etok = Peek(1);// todo: errornode
  switch (tok_) {
    case TokenKind::ANYKW:
    case TokenKind::ALL: {
      const auto& tok = Consume();
      switch (tok_) {
        case TokenKind::COMPONENT:
        case TokenKind::PORT:
        case TokenKind::TIMER:
          return NewNode<nodes::CompositeIdent>([&](auto& ident) {
            ident.tok1 = tok;
            ident.tok2 = Consume();
          });
        case TokenKind::FROM:
          return NewNode<nodes::FromExpr>([&](auto& fe) {
            fe.kind = tok;
            fe.from = Consume();
            fe.x = ParsePrimaryExpr();
          });
        default:
          return ParseAnyIdent();
      }
    }

    case TokenKind::UNIVERSAL:
      return ParseUniversalCharstring();

    case TokenKind::ADDRESS:
    case TokenKind::CHARSTRING:
    case TokenKind::CLASS:
    case TokenKind::MAP:
    case TokenKind::MTC:
    case TokenKind::SYSTEM:
    case TokenKind::TESTCASE:
    case TokenKind::TIMER:
    case TokenKind::UNMAP:
    case TokenKind::VALUE:
      return ParseAnyIdent();

    case TokenKind::IDENT:
      return ParseRef();

    case TokenKind::INT:
    case TokenKind::ANY:
    case TokenKind::BITSTRING:
    case TokenKind::ERROR:
    case TokenKind::kNULL:
    case TokenKind::OMIT:
    case TokenKind::FAIL:
    case TokenKind::FALSE:
    case TokenKind::FLOAT:
    case TokenKind::INCONC:
    case TokenKind::MUL:
    case TokenKind::kNaN:
    case TokenKind::NONE:
    case TokenKind::PASS:
    case TokenKind::STRING:
    case TokenKind::TRUE:
      return NewNode<nodes::ValueLiteral>([&](auto& v) {
        v.tok = Consume();
      });

    case TokenKind::LPAREN:
      // can be template `x := (1,2,3)`, but also artihmetic expression: `1*(2+3)`
      return ParseParenExpr();

    case TokenKind::LBRACK:
      return ParseIndexExpr(nullptr);

    case TokenKind::LBRACE:
      return ParseCompositeLiteral();

    case TokenKind::MODIFIES:
      return NewNode<nodes::ModifiesExpr>([&](auto& m) {
        Consume();  // 'modfiies' tok
        m.x = ParsePrimaryExpr();
        Expect(TokenKind::ASSIGN);
        m.y = ParseExpr();
      });

    case TokenKind::REGEXP:
      return ParseCallRegexp();

    case TokenKind::PATTERN:
      return ParseCallPattern();

    case TokenKind::DECMATCH:
      return ParseCallDecmatch();

    case TokenKind::MODIF:
      return ParseModifier();

    default:
      EmitErrorExpected("operand");
      return NewErrorNode<nodes::Expr>();
  }
}

nodes::SelectorExpr* Parser::ParseSelectorExpr(nodes::Expr* x) {
  auto* se = NewNode<nodes::SelectorExpr>([&](auto& se) {
    se.x = x;
    ConsumeInvariant(TokenKind::DOT);
    se.sel = ParseRef();
  });
  ExtendByIncorporatedNode(se, se->x);
  return se;
}

nodes::IndexExpr* Parser::ParseIndexExpr(nodes::Expr* x) {
  auto* r = NewNode<nodes::IndexExpr>([&](auto& ie) {
    ie.x = x;
    Expect(TokenKind::LBRACK);
    ie.index = ParseExpr();
    Expect(TokenKind::RBRACK);
  });
  if (x) [[likely]] {
    ExtendByIncorporatedNode(r, r->x);
  }
  return r;
}

nodes::CallExpr* Parser::ParseCallExpr(nodes::Expr* x) {
  auto* r = NewNode<nodes::CallExpr>([&](auto& ce) {
    ce.fun = x;
    ce.args = NewNode<nodes::ParenExpr>([&](auto& pe) {
      Expect(TokenKind::LPAREN);
      switch (tok_) {
        case TokenKind::RPAREN:
          break;
        case TokenKind::TO:
        case TokenKind::FROM:
          // TODO(ntt): Shouldn't this be a FromExpr?
          pe.list.push_back(NewNode<nodes::BinaryExpr>([&](auto& be) {
            be.x = nullptr;
            be.op = Consume();
            be.y = ParseExpr();
          }));
          break;
        case TokenKind::REDIR:
          pe.list.push_back(ParseRedirect(nullptr));
          break;
        default:
          pe.list = ParseExprList();
          break;
      }
      Expect(TokenKind::RPAREN);
    });
  });
  if (x) [[likely]] {
    ExtendByIncorporatedNode(r, r->fun);
  }
  return r;
}

nodes::LengthExpr* Parser::ParseLengthExpr(nodes::Expr* x) {
  auto* r = NewNode<nodes::LengthExpr>([&](auto& le) {
    le.x = x;
    Expect(TokenKind::LENGTH);
    le.size = ParseParenExpr();
  });
  if (x) [[likely]] {
    ExtendByIncorporatedNode(r, r->x);
  }
  return r;
}

nodes::RedirectExpr* Parser::ParseRedirect(nodes::Expr* x) {
  auto* r = NewNode<nodes::RedirectExpr>([&](auto& re) {
    re.x = x;
    Expect(TokenKind::REDIR);

    if (tok_ == TokenKind::VALUE) {
      Consume();
      re.value = ParseExprList();
    }

    if (tok_ == TokenKind::PARAM) {
      Consume();
      re.param = ParseExprList();
    }

    if (tok_ == TokenKind::SENDER) {
      Consume();
      re.sender = ParsePrimaryExpr();
    }

    if (tok_ == TokenKind::MODIF) {
      const auto& ctok = Peek(1);
      if (ctok.On(src_) != "@index") {
        EmitError(ctok.range, std::format("expected '@index', found {}", ctok.On(src_)));
      }

      re.to_index = NewNode<nodes::RedirectToIndex>([&](auto& rti) {
        Consume();  // IndexTok
        if (tok_ == TokenKind::VALUE) {
          Consume();
          rti.value = true;
        }
        rti.index = ParsePrimaryExpr();
      });
    }

    if (tok_ == TokenKind::TIMESTAMP) {
      ConsumeInvariant(TokenKind::TIMESTAMP);
      re.timestamp = ParsePrimaryExpr();
    }
  });
  if (x) [[likely]] {
    ExtendByIncorporatedNode(r, r->x);
  }
  return r;
}

// ExprList ::= Expr { "," Expr }
std::vector<nodes::Expr*> Parser::ParseExprList() {
  std::vector<nodes::Expr*> v;
  v.push_back(ParseExpr());
  while (tok_ == TokenKind::COMMA) {
    Consume();
    v.push_back(ParseExpr());
  }
  return v;
}

nodes::Ident* Parser::ParseRef() {
  auto* ident = ParseName();
  if (ident == nullptr) {
    return nullptr;
  }

  if (tok_ != TokenKind::LT) [[likely]] {
    return ident;
  }

  Mark();
  //
  auto* x = TryParseTypeParameters();
  if (x != nullptr && !kTokOperandStart.contains(tok_)) {
    Commit();
    return NewNode<nodes::ParametrizedIdent>([&](auto& p) {
      p.ident = ident;
      p.params = x;
    });
  }
  //
  Reset();
  return ident;
}

nodes::ParenExpr* Parser::ParseParenExpr() {
  return NewNode<nodes::ParenExpr>([&](auto& pe) {
    Expect(TokenKind::LPAREN);
    pe.list = ParseExprList();
    Expect(TokenKind::RPAREN);
  });
}

nodes::CompositeIdent* Parser::ParseUniversalCharstring() {
  return NewNode<nodes::CompositeIdent>([&](auto& ci) {
    ci.tok1 = Expect(TokenKind::UNIVERSAL);
    ci.tok2 = Expect(TokenKind::CHARSTRING);
  });
}

nodes::CompositeLiteral* Parser::ParseCompositeLiteral() {
  return NewNode<nodes::CompositeLiteral>([&](auto& cl) {
    Expect(TokenKind::LBRACE);
    if (tok_ != TokenKind::RBRACE) {
      cl.list = ParseExprList();
    }
    Expect(TokenKind::RBRACE);
  });
}

nodes::RegexpExpr* Parser::ParseCallRegexp() {
  return NewNode<nodes::RegexpExpr>([&](auto& re) {
    Expect(TokenKind::REGEXP);
    if (tok_ == TokenKind::MODIF) {
      Consume();
      re.nocase = true;
    }
    re.x = ParseParenExpr();
  });
}

nodes::PatternExpr* Parser::ParseCallPattern() {
  return NewNode<nodes::PatternExpr>([&](auto& pe) {
    Expect(TokenKind::PATTERN);
    if (tok_ == TokenKind::MODIF) {
      Consume();
      pe.nocase = true;
    }
    pe.x = ParseExpr();
  });
}

nodes::DecmatchExpr* Parser::ParseCallDecmatch() {
  return NewNode<nodes::DecmatchExpr>([&](auto& de) {
    Expect(TokenKind::DECMATCH);
    if (tok_ == TokenKind::LPAREN) {
      de.params = ParseParenExpr();
    }
    de.x = ParseExpr();
  });
}

nodes::Expr* Parser::ParseModifier() {
  const auto& name = Peek(1).On(src_);
  if (name == "@dynamic") {
    return ParseDynamicModifier();
  }
  if (name == "@decoded") {
    return ParseDecodedModifier();
  }

  const auto& mtok = Expect(TokenKind::MODIF);
  EmitError(mtok.range, "modifier expected");
  return nullptr;
}

nodes::DynamicExpr* Parser::ParseDynamicModifier() {
  return NewNode<nodes::DynamicExpr>([&](auto& de) {
    ConsumeInvariant(TokenKind::MODIF);
    de.body = ParseBlockStmt();
  });
}

nodes::DecodedExpr* Parser::ParseDecodedModifier() {
  return NewNode<nodes::DecodedExpr>([&](auto& de) {
    ConsumeInvariant(TokenKind::MODIF);
    if (tok_ == TokenKind::LPAREN) {
      de.params = ParseParenExpr();
    }
    de.x = ParsePrimaryExpr();
  });
}

nodes::ParenExpr* Parser::TryParseTypeParameters() {
  return NewNode<nodes::ParenExpr>([&](auto& pe) {
    Consume();  // LParen
    while (tok_ != TokenKind::GT) {
      auto* y = TryParseTypeParameter();
      if (y == nullptr) {
        return false;
      }
      pe.list.push_back(y);
      if (tok_ != TokenKind::COMMA) {
        break;
      }
      Consume();
    }
    if (tok_ != TokenKind::GT) {
      return false;
    }
    Consume();  // RParen
    return true;
  });
}

nodes::Expr* Parser::TryParseTypeParameter() {
  nodes::Expr* x = TryParseTypeIdent();
  while (true) {
    switch (tok_) {
      case TokenKind::DOT: {
        x = NewNode<nodes::SelectorExpr>([&, px = x](auto& se) {
          se.x = px;
          ConsumeInvariant(TokenKind::DOT);
          se.sel = TryParseTypeIdent();
          return se.sel != nullptr;
        });
        break;
      }
      case TokenKind::LBRACK: {
        Consume();  // LBrack
        const auto& dash = Consume();
        const auto& rbrack = Consume();
        if (dash.kind != TokenKind::SUB || rbrack.kind != TokenKind::RBRACK) {
          return nullptr;
        }
        x = NewNode<nodes::IndexExpr>([&, px = x](auto& ie) {
          ie.x = px;
          ie.index = NewNode<nodes::ValueLiteral>([&](auto& vl) {
            vl.tok = dash;
          });
        });
        break;
      }
      default: {
        return x;
      }
    }
  }
}

nodes::Ident* Parser::TryParseTypeIdent() {
  nodes::Ident* x;
  switch (tok_) {
    case TokenKind::IDENT:
    case TokenKind::ADDRESS:
    case TokenKind::CHARSTRING:
      x = ParseAnyIdent();
      break;
    case TokenKind::UNIVERSAL:
      x = ParseUniversalCharstring();
      break;
    default:
      return nullptr;
  }
  if (tok_ == TokenKind::LT) [[unlikely]] {
    auto* y = TryParseTypeParameters();
    if (y == nullptr) {
      return NewNode<nodes::ParametrizedIdent>([&](auto& pi) {
        pi.ident = x;
        pi.params = y;
      });
    }
  }
  return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Token Parser::Consume() {
  auto tok = queue_[cursor_];
  ++cursor_;

  seen_closing_brace_ = tok_ == TokenKind::RBRACE;

  if (cursor_ == queue_.size() && !IsSpeculating()) {
    cursor_ = 0;
    queue_.clear();
  }

  Peek(1);
  tok_ = queue_[cursor_].kind;

  last_consumed_pos_ = tok.range.end;

  return tok;
}

Token Parser::ConsumeInvariant(TokenKind) {
  return Consume();
}

void Parser::Grow(std::uint8_t n) {
  constexpr auto kShouldIgnore = [](TokenKind kind) -> bool {
    switch (kind) {
      case TokenKind::COMMENT:
      case TokenKind::PREPROC:
        return true;
      default:
        return false;
    }
  };

  while (n > 0) {
    auto token = scanner_.Scan();
    if (kShouldIgnore(token.kind)) {
      continue;
    }

    if (token.kind == TokenKind::MALFORMED || token.kind == TokenKind::UNTERMINATED) {
      EmitError(token.range, "malformed or unterminated token");
    }
    queue_.push_back(token);
    --n;
  }
}

Token& Parser::Peek(std::uint8_t i) {
  const auto idx = cursor_ + i - 1;
  const auto cached = queue_.size();
  if (idx + 1 > cached) {
    Grow(idx - cached + 1);
  }
  return queue_[idx];
}

std::string_view Parser::Lit(std::uint8_t i) {
  return Peek(i).On(src_);
}

ast::pos_t Parser::Pos(std::uint8_t i) {
  return Peek(i).range.begin;
}

Token Parser::Expect(TokenKind expected) {
  if (tok_ != expected) {
    std::stringstream ss;
    ss << magic_enum::enum_name(expected);
    EmitErrorExpected(ss.str());
  }
  return Consume();
}

inline void Parser::MaybeExpectSemi() {
  if (tok_ == TokenKind::SEMICOLON) {
    Consume();
    return;
  }
}

void Parser::ExpectSemi() {
  if (tok_ == TokenKind::SEMICOLON) {
    Consume();
    return;
  }

  constexpr bool kPedanticSemi = false;
  if (kPedanticSemi) {
    if (!seen_closing_brace_ && tok_ == TokenKind::RBRACE && tok_ != TokenKind::kEOF) {
      EmitErrorExpected(";");
      // Advance(TOK_STMT_START); // TODO: is this really needed?
    }
  }
}

void Parser::Advance(const std::set<TokenKind>& to) {
  for (; tok_ != TokenKind::kEOF; Consume()) {
    if (!to.contains(tok_)) {
      continue;
    }
    // Return only if parser made some progress since last
    // sync or if it has not reached 10 advance calls without
    // progress. Otherwise consume at least one token to
    // avoid an endless parser loop (it is possible that
    // both parseOperand and parseStmt call advance and
    // correctly do not advance, thus the need for the
    // invocation limit p.syncCnt).
    const auto pos = Pos(1);
    if (pos == sync_pos_ && sync_cnt_ < 10) {
      ++sync_cnt_;
      return;
    }
    if (pos > sync_pos_) {
      sync_pos_ = pos;
      sync_cnt_ = 0;
      return;
    }
    // Reaching here indicates a parser bug, likely an
    // incorrect token list in this function, but it only
    // leads to skipping of possibly correct code if a
    // previous error is present, and thus is preferred
    // over a non-terminating parse.
  }
}

inline void Parser::Mark() {
  markers_.push_back(cursor_);
}

inline void Parser::Commit() {
  markers_.pop_back();
}

inline void Parser::Reset() {
  const ast::pos_t marker = markers_.back();
  markers_.pop_back();
  cursor_ = marker;
  tok_ = queue_[cursor_].kind;
}

inline bool Parser::IsSpeculating() const noexcept {
  return !markers_.empty();
}

void Parser::EmitError(const ast::Range& range, std::string&& message) {
  errors_.emplace_back(range, std::move(message));
}
void Parser::EmitErrorExpected(std::string&& what) {
  errors_.emplace_back(Peek(1).range, std::format("expected '{}'", what));
}

//

Token* Parser::TokAlloc(Token&& token) {
  return arena_->Alloc<Token>(std::move(token));
}

template <typename T, typename Initializer>
  requires std::is_invocable_v<Initializer, T&>
T* Parser::NewNode(Initializer f) {
  auto* p = arena_->Alloc<T>();
  p->nrange.begin = Peek(1).range.begin;
  //
  auto* top = last_node_;
  p->parent = top;
  last_node_ = p;
  //
  if constexpr (std::is_same_v<typename std::invoke_result_t<Initializer, T&>, bool>) {
    if (!f(*p)) {
      return nullptr;
    }
  } else {
    f(*p);
  }
  //
  p->nrange.end = last_consumed_pos_;
  //
  last_node_ = top;
  return p;
}

template <IsNode ConcreteNode>
ConcreteNode* Parser::NewErrorNode() {
  return arena_->Alloc<ConcreteNode>(NodeKind::ErrorNode);
}

}  // namespace parser
}  // namespace vanadium::core::ast
