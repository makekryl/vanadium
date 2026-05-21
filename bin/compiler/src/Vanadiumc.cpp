#include <filesystem>
#include <format>
#include <fstream>
#include <print>
#include <ranges>
#include <thread>

#include <argparse/argparse.hpp>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <vanadium/ast/ASTTypes.h>
#include <vanadium/bin/Bootstrap.h>
#include <vanadium/compiler/Compiler.h>
#include <vanadium/compiler/ErrorFormatter.h>
#include <vanadium/core/Program.h>
#include <vanadium/lib/Error.h>
#include <vanadium/lib/FunctionRef.h>
#include <vanadium/lib/concurrency/TaskArena.h>
#include <vanadium/tooling/Filesystem.h>
#include <vanadium/tooling/Project.h>
#include <vanadium/tooling/Solution.h>
#include <vanadium/tooling/impl/SystemFS.h>
#include <vanadium/version.h>

namespace {
using namespace vanadium;

std::size_t CheckErrors(const tooling::Solution& solution) {
  std::string errbuf;
  const auto flush_err = [&](const std::string& err) {
    std::fputs(err.c_str(), stderr);
  };

  std::size_t errors = 0;
  for (const auto& project : solution.Projects()) {
    for (const auto& sf : project.program.Files() | std::views::values) {
      errors += compiler::PrintErrors(errbuf, sf, solution.Directory().Join(sf.path), flush_err);
    }
  }
  return errors;
}

int main(int argc, char* argv[]) {
  std::uint32_t jobs{std::clamp(std::thread::hardware_concurrency(), 1U, 4U)};
  bool use_debug;

  argparse::ArgumentParser ap("vanadiumc", bin::kVersion);
  ap.add_description("TTCN-3 Compiler");
  //
  ap.add_argument("-j", "--parallel", "").store_into(jobs).help("maximum number of worker threads");
  ap.add_argument("-g").store_into(use_debug).flag().help("emit debug symbols");
  //
  std::string path;
  ap.add_argument("path").store_into(path).help("project path");

  //
  PARSE_CLI_ARGS_OR_EXIT(ap, argc, argv, 1);
  //

  //
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  //

  lib::concurrency::TaskArena task_arena(jobs);

  auto solution = task_arena.Execute([&] {
    if (path.ends_with(".ttcn")) {
      // TODO: better CLI interface for this, drop Solution::WrapSingular hack
      tooling::Project project(
          tooling::fs::Root<tooling::fs::SystemFS>(std::filesystem::path(path).parent_path().string()), "",
          {.root = true});
      auto sol = tooling::Solution::WrapSingular(project);
      const auto read_file = [&path](const std::string&, std::string& buf) {
        if (auto f = std::ifstream(path)) {
          std::stringstream ss;
          ss << f.rdbuf();
          buf = ss.str();
        } else {
          assert(false);
        }
      };
      sol.Projects().front().program.Commit([&](auto& modify) {
        modify.update(std::filesystem::path(path).filename(), read_file);
      });
      return std::expected<tooling::Solution, Error>{std::move(sol)};
    }
    return tooling::Solution::Load(tooling::fs::Root<tooling::fs::SystemFS>(path));
  });
  if (!solution) {
    std::println("error: {}", solution.error().String());
    return 2;
  }
  if (CheckErrors(*solution) != 0) {
    return 1;
  }

  for (const auto& project : solution->Projects()) {
    task_arena.Execute([&] {
      compiler::Compile(project.program, {.debug = use_debug}, [&](const core::SourceFile& sf, llvm::Module& mod) {
        auto full_path = solution->Directory().Join(sf.path);
        std::println("Compiled '{}'", full_path);
        std::error_code ec;
        llvm::raw_fd_ostream dest(std::format("{}.ll", full_path), ec);
        mod.print(dest, nullptr);
      });
    });
  }

  return 0;
}
}  // namespace

DEFINE_VANADIUM_ENTRYPOINT(main);
