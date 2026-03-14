#include <fstream>

#include <argparse/argparse.hpp>

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

  vanadium::compiler::CompileIR(*sf);

  return 0;
}
}  // namespace

DEFINE_VANADIUM_ENTRYPOINT(main);
