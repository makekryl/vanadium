#include <vanadium/ast/Parser.h>

#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Asn1Transparser.h"
#include "TextDumper.h"
#include "vanadium/asn1/ast/Asn1ModuleBasket.h"
#include "vanadium/ast/AST.h"
#include "vanadium/lib/Arena.h"

namespace {
std::optional<std::string> ReadFile(const char* path) {
  // TODO: err
  if (auto f = std::ifstream(path)) {
    std::stringstream buf;
    buf << f.rdbuf();
    return buf.str();
  }
  return std::nullopt;
}

vanadium::ast::AST ParseUsingAsn1c(vanadium::lib::Arena& arena, std::string_view src) {
  vanadium::ast::AST ast;

  vanadium::asn1::ast::Asn1ModuleBasket basket;
  basket.Update<void>(nullptr, src);
  return basket.Transform<void>(nullptr, arena);
}
}  // namespace

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << std::format("{} [filename]", argv[0]) << std::endl;
    return 1;
  }

  const std::string_view filename{argv[1]};

  const auto src = ReadFile(filename.data());
  if (!src) {
    std::cerr << std::format("Failed to open '{}'", filename) << std::endl;
    return 1;
  }

  const auto parse_fn =
      filename.ends_with(".asn") ? /*vanadium::asn1::ast::Transparse*/ ParseUsingAsn1c : vanadium::ast::Parse;

  vanadium::lib::Arena arena;
  auto ast = parse_fn(arena, *src);

  // TODO: display errors
  if (!ast.errors.empty()) {
    std::cout << std::format(" (!) {} errors\n", ast.errors.size());
    for (const auto& err : ast.errors) {
      std::cout << std::format("{}:{} - {}", err.range.begin, err.range.end, err.description) << std::endl;
    }
  }

  TextASTDumper::Create(ast, std::cout).Apply([](auto& d) {
    d.Dump();
  });
}
