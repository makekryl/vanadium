#include <filesystem>
#include <format>
#include <fstream>
#include <print>
#include <sstream>

#include <argparse/argparse.hpp>

#include <vanadium/ast/Parser.h>
#include <vanadium/bin/Bootstrap.h>
#include <vanadium/format/AstPrinter.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/version.h>

#include "vanadium/bin/fmt/FmtTreeDumper.h"
#include "vanadium/bin/fmt/OptionsReader.h"

using namespace vanadium;

namespace {
std::string ReadStdin() {
  std::ostringstream ss;
  ss << std::cin.rdbuf();
  return ss.str();
}
std::optional<std::string> ReadFile(const std::string& path) {
  // TODO: check & raise errors
  if (auto f = std::ifstream(path)) {
    std::stringstream buf;
    buf << f.rdbuf();
    return buf.str();
  }
  return std::nullopt;
}
void WriteFile(const std::string& path, std::string_view content) {
  // TODO: check & raise errors
  if (auto f = std::ofstream(path, std::ios::out | std::ios::trunc)) {
    f << content;
  }
}
}  // namespace

namespace {
int main(int argc, char* argv[]) {
  argparse::ArgumentParser ap("vanadium_fmt", vanadium::bin::kVersion);
  ap.add_description("TTCN-3 source code pretty-printer");
  //
  std::string filepath;
  ap.add_argument("path").store_into(filepath).help("file path");
  //
  bool use_dryrun{false};
  bool use_write{false};
  {
    auto& group = ap.add_mutually_exclusive_group();
    group.add_argument("--dryrun", "-n").store_into(use_dryrun).help("dry-run mode");
    group.add_argument("--write", "-w").store_into(use_write).help("overwrite file");
  }
  //
  bool just_dump_tree{false};
  ap.add_argument("--dump-tree").store_into(just_dump_tree).help("dump serialized AST tree");

  //
  PARSE_CLI_ARGS_OR_EXIT(ap, argc, argv, 1);
  //

  const bool read_from_stdin = filepath == "-";

  auto src = read_from_stdin ? ReadStdin() : ReadFile(filepath);
  if (!src) {
    std::println(stderr, "Failed to read '{}'", filepath);
    return 3;
  }

  lib::Arena arena;
  const auto ast = ast::Parse(arena, *src);

  if (!ast.errors.empty()) {
    std::println(stderr, "File has syntax errors:", filepath);
    for (const auto& err : ast.errors) {
      std::println(stderr, " - ({}:{}): {}", err.range.begin, err.range.end, err.description);
    }
    return 2;
  }

  if (just_dump_tree) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    vanadium::bin::fmt::DumpSerializedTree(std::cout, ast, ast.root);
    return 0;
  }

  format::PrintOptions print_opts{};
  if (const auto manifest_path = std::filesystem::current_path() / ".vanadiumrc.toml";
      std::filesystem::exists(manifest_path)) {
    const auto manifest_contents = ReadFile(manifest_path.string());
    if (!manifest_contents) {
      std::println(stderr, "Failed to read manifest at '{}'", filepath);
      return 3;
    }
    const auto err = bin::fmt::TryReadOptionsFromManifest(*manifest_contents, print_opts);
    if (err) {
      std::println(stderr, "Failed to read options from manifest: {}", err->String());
      return 5;
    }
  }

  const auto result = format::PrintAst(ast, ast.root, print_opts) + "\n";
  if (use_dryrun) {
    const bool no_diffs = ast.src == result;
    return no_diffs ? 0 : 1;
  }

  if (!use_write) {
    std::cout << result;
  } else {
    if (read_from_stdin) {
      std::println(stderr, "Cannot use write-mode with stdin as input file path");
      return 4;
    }
    WriteFile(filepath, result);
  }

  return 0;
}
}  // namespace

DEFINE_VANADIUM_ENTRYPOINT(main);
