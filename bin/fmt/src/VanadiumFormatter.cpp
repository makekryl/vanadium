#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <sstream>

#include <argparse/argparse.hpp>

#include <vanadium/ast/Parser.h>
#include <vanadium/bin/Bootstrap.h>
#include <vanadium/format/AstPrinter.h>
#include <vanadium/lib/Arena.h>

using namespace vanadium;

namespace {
std::optional<std::string> ReadFile(const std::string& path) {
  // TODO: check & raise errors
  if (auto f = std::ifstream(path)) {
    std::stringstream buf;
    buf << f.rdbuf();
    return buf.str();
  }
  return std::nullopt;
}
}  // namespace

namespace {
int main(int argc, char* argv[]) {
  argparse::ArgumentParser ap("vanadium_fmt");
  ap.add_description("TTCN-3 source code pretty printer");
  //
  std::string filepath;
  ap.add_argument("path").store_into(filepath).help("file path");

  //
  PARSE_CLI_ARGS_OR_EXIT(ap, argc, argv, 1);
  //

  auto src = ReadFile(filepath);
  if (!src) {
    std::println(stderr, "Failed to open '{}'", filepath);
    return 1;
  }

  lib::Arena arena;
  auto ast = ast::Parse(arena, *src);

  std::println("{}", format::PrintAst(ast.src, ast.root,
                                      format::PrinterOptions{
                                          .tab_width = 2,
                                          .print_width = 80,
                                      }));

  return 0;
}
}  // namespace

DEFINE_VANADIUM_ENTRYPOINT(main);
