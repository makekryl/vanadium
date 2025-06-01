#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

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

  const auto src = ReadFile(argv[1]);
  if (!src) {
    std::cerr << std::format("Failed to open '{}'", argv[1]) << std::endl;
    return 1;
  }

  vanadium::lib::Arena arena;
  auto ast = vanadium::core::ast::Parse(arena, *src);

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
