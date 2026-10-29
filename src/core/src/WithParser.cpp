#include "vanadium/core/WithParser.h"

#include <cassert>
#include <format>
#include <string_view>

#include <vanadium/ast/ASTNodes.h>

#include "vanadium/ast/ASTTypes.h"
#include "vanadium/ast/Scanner.h"
#include "vanadium/lib/FunctionRef.h"

#include "magic_enum/magic_enum.hpp"

namespace vanadium::core {

namespace {
class WithStmtStrValueScanner {
 public:
  WithStmtStrValueScanner(const ast::AST& ast, const ast::nodes::Expr* expr,
                          const lib::Consumer<ast::Range, std::string>& emit_error)
      : emit_error_{emit_error} {
    const auto* vl = expr->As<ast::nodes::ValueLiteral>();
    assert(vl->tok.kind == ast::TokenKind::STRING);
    std::string_view str = ast.Text(vl->tok);
    str.remove_prefix(1);  // "
    str.remove_suffix(1);  // "

    base_offset_ = vl->tok.range.begin + 1;
    scanner_.emplace(str);

    Grow();
  }

  ~WithStmtStrValueScanner() {
    Expect(ast::TokenKind::kEOF);
  }

  std::optional<ast::Token> Expect(ast::TokenKind kind) {
    auto tok = Scan();
    if (tok.kind != kind) {
      emit_error_(tok.range, std::format("expected {}", magic_enum::enum_name(kind)));
      return std::nullopt;
    }
    return tok;
  }

  [[nodiscard]] ast::Token Scan() {
    ast::Token tok = next_;
    Grow();
    return tok;
  }

  void Consume() {
    Grow();
  }

  [[nodiscard]] ast::TokenKind Peek() const {
    return next_.kind;
  }

 private:
  void Grow() {
    do {
      next_ = scanner_->Scan();
    } while (next_.kind == ast::TokenKind::COMMENT);
    next_.range.begin += base_offset_;
    next_.range.end += base_offset_;
  }
  const lib::Consumer<ast::Range, std::string>& emit_error_;
  ast::pos_t base_offset_;
  std::optional<ast::parser::Scanner> scanner_;  // lateinit
  //
  ast::Token next_{};
};
}  // namespace

void ParseModuleWithSpec(const ast::AST& ast, const ast::nodes::WithSpec* ws,
                         const ModuleWithAttributesConsumer& consumer) {
  if (!ws) {
    return;
  }

  const auto lit = [&ast](const ast::Token& tok) -> std::string_view {
    return ast.Text(tok);
  };

  for (const auto* stmt : ws->list) {
    switch (stmt->kind.kind) {
      case ast::TokenKind::EXTENSION: {
        WithStmtStrValueScanner scanner{ast, stmt->value, consumer.emit_error};
        const auto extension_type_tok = scanner.Scan();
        if (extension_type_tok.kind == ast::TokenKind::kEOF) {
          consumer.emit_error(extension_type_tok.range, "expected extension name");
          break;
        }
        const auto extension_type = lit(extension_type_tok);
        if ("anytype" == extension_type) {
          consumer.accept_extension_anytype(
              extension_type_tok, [&](const ModuleWithAttributesConsumer::ExtensionAnytypeConsumer& consumer) {
                while (true) {
                  const auto ident_tok = scanner.Expect(ast::TokenKind::IDENT);
                  if (!ident_tok) {
                    break;
                  }
                  consumer.accept_member(*ident_tok);
                  if (scanner.Peek() != ast::TokenKind::COMMA) {
                    break;
                  }
                  scanner.Expect(ast::TokenKind::COMMA);
                }
                scanner.Expect(ast::TokenKind::kEOF);
              });
        } else {
          // TODO(compiler): reject unknown
          break;
        }
        break;
      }
      default:
        // TODO(compiler): reject unknown
        break;
    }
  }
}

}  // namespace vanadium::core
