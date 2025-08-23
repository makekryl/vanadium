#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Asn1Transparser.h"
#include "Parser.h"
#include "TextDumper.h"

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

  const auto parse_fn = filename.ends_with(".asn") ? vanadium::asn1::ast::Parse : vanadium::core::ast::Parse;

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
