#include <gtest/gtest.h>

#include <fstream>
#include <sstream>

#include "Asn1AST.h"
#include "Asn1Scanner.h"
#include "magic_enum/magic_enum.hpp"

TEST(Asn1Scanner, test) {
  std::ifstream f("/workspaces/vanadium/src/ast_asn1/test/EUTRA-RRC-Definitions.asn");
  std::stringstream buf;
  buf << f.rdbuf();
  const std::string s = buf.str();

  vanadium::asn1::ast::parser::Scanner scanner(s);

  vanadium::asn1::ast::Token tok;
  while ((tok = scanner.Scan()).kind != vanadium::asn1::ast::TokenKind::kEOF) {
    std::cout << magic_enum::enum_name(tok.kind) << std::endl;
  }
}
