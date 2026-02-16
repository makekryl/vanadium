#include <format>
#include <fstream>
#include <print>
#include <sstream>

#include <argparse/argparse.hpp>

#include <vanadium/ast/Parser.h>
#include <vanadium/bin/Bootstrap.h>
#include <vanadium/format/AstPrinter.h>
#include <vanadium/lib/Arena.h>

#include "vanadium/bin/fmt/FmtTreeDumper.h"

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
  ap.add_description("TTCN-3 source code pretty-printer");
  //
  std::string filepath;
  ap.add_argument("path").store_into(filepath).help("file path");
  //
  bool just_dump_tree{false};
  ap.add_argument("--dump-tree").store_into(just_dump_tree).help("dump serialized AST tree");

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

  if (just_dump_tree) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    vanadium::bin::fmt::DumpSerializedTree(std::cout, ast, ast.root);
    return 0;
  }

  std::println("{}", format::PrintAst(ast, ast.root,
                                      format::PrintOptions{
                                          .tab_width = 4,
                                          .print_width = 80,
                                      }));

  return 0;
}
}  // namespace

DEFINE_VANADIUM_ENTRYPOINT(main);
