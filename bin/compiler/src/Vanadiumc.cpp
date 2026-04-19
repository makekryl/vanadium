#include <fstream>
#include <print>

#include <argparse/argparse.hpp>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <vanadium/bin/Bootstrap.h>
#include <vanadium/compiler/Compiler.h>
#include <vanadium/core/Program.h>
#include <vanadium/version.h>

namespace {
int main(int argc, char* argv[]) {
  argparse::ArgumentParser ap("vanadiumc", vanadium::bin::kVersion);
  ap.add_description("TTCN-3 Compiler");
  //
  std::string filepath;
  ap.add_argument("path").store_into(filepath).help("file path");
  //
  bool use_debug;
  ap.add_argument("-g").store_into(use_debug).flag().help("emit debug symbols");

  //
  PARSE_CLI_ARGS_OR_EXIT(ap, argc, argv, 1);
  //

  vanadium::core::Program program;
  program.Commit([&](auto& modify) {
    const auto read_file = [](const std::string& path, std::string& buf) {
      if (auto f = std::ifstream(path)) {
        std::stringstream ss;
        ss << f.rdbuf();
        buf = ss.str();
      } else {
        assert(false);
      }
    };
    modify.update(filepath, read_file);
  });

  const auto* sf = program.GetFile(filepath);
  assert(sf != nullptr);
  assert(sf->ast.errors.empty());
  assert(sf->semantic_errors.empty());
  assert(sf->type_errors.empty());
  assert(sf->module.has_value());

  vanadium::compiler::Compile(program, {.debug = use_debug},
                              [&](const vanadium::core::SourceFile& sf, llvm::Module& mod) {
                                std::println("Compiled '{}'", sf.path);
                                std::error_code ec;
                                llvm::raw_fd_ostream dest(std::format("{}.ll", sf.path), ec);
                                mod.print(dest, nullptr);
                              });

  return 0;
}
}  // namespace

DEFINE_VANADIUM_ENTRYPOINT(main);
