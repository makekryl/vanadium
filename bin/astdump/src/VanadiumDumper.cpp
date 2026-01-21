#include <vanadium/asn1/ast/Asn1ModuleBasket.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/Parser.h>
#include <vanadium/bin/Bootstrap.h>
#include <vanadium/lib/Arena.h>

#include <argparse/argparse.hpp>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <sstream>

#include "Asn1Transparser.h"
#include "vanadium/bin/astdump/TextDumper.h"
#include "vanadium/bin/astdump/asciicolorss.h"

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

bool IsAsnFile(std::string_view path) {
  return path.ends_with(".asn");
}
}  // namespace

namespace {
int main(int argc, char* argv[]) {
  argparse::ArgumentParser ap("astdump");
  ap.add_description("Vanadium TTCN-3 syntax tree printer");
  //
  std::vector<std::string> asn_include_filepaths;
  ap.add_argument("-A").append().store_into(asn_include_filepaths).help("additional ASN.1 files");
  //
  std::string filepath;
  ap.add_argument("path").store_into(filepath).help("file path");

  //
  PARSE_CLI_ARGS_OR_EXIT(ap, argc, argv, 1);
  //

  const bool is_asn = IsAsnFile(filepath);
  if (!is_asn && !asn_include_filepaths.empty()) {
    std::println("Arguments of kind '-A' are not expected for a TTCN-3 file");
    return 1;
  }

  auto src = ReadFile(filepath);
  if (!src) {
    std::println(stderr, "Failed to open '{}'", filepath);
    return 1;
  }

  lib::Arena arena;
  std::optional<ast::AST> ast;  // lateinit
  //
  if (is_asn) {
    asn1::ast::Asn1ModuleBasket basket;
    for (auto& extra_filepath : asn_include_filepaths) {
      auto extra_src = ReadFile(extra_filepath);
      if (!extra_src) {
        std::println(stderr, "Failed to open '{}'", extra_filepath);
        return 1;
      }

      // move std::string handle to arena so it does not dispose
      basket.Update(&extra_filepath, *arena.Alloc<std::string>(std::move(*extra_src)));
    }
    basket.Update(&src, *src);
    ast = basket.Transform(&src, arena);
  } else {
    ast = ast::Parse(arena, *src);
  }

  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  if (!ast->errors.empty()) {
    std::println(stderr, "{}{}{}{}", asciicolors::kRed, asciicolors::kBold,
                 std::format(" (!) {} syntax{} errors", ast->errors.size(), is_asn ? " and/or semantic" : ""),
                 asciicolors::kReset);
    for (const auto& err : ast->errors) {
      std::println(stderr, "{}{}{}{}{}{} - {}{}", asciicolors::kDim, asciicolors::kRed, asciicolors::kBold,
                   std::format("{}:{}", err.range.begin, err.range.end), asciicolors::kReset, asciicolors::kRed,
                   err.description, asciicolors::kReset);
    }
  }

  TextASTDumper::Create(*ast, std::cout).Apply([](auto& d) {
    d.Dump();
  });

  return 0;
}
}  // namespace

DEFINE_VANADIUM_ENTRYPOINT(main);
