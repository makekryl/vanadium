#include <cctype>
#include <filesystem>
#include <format>
#include <fstream>
#include <iterator>
#include <print>
#include <ranges>
#include <string_view>
#include <thread>

#include <argparse/argparse.hpp>
#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <vanadium/ast/ASTTypes.h>
#include <vanadium/bin/Bootstrap.h>
#include <vanadium/compiler/Compiler.h>
#include <vanadium/core/Program.h>
#include <vanadium/lib/ColoredFmt.h>
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

void FormatError(std::string& buf, const tooling::fs::Path& base_path, const core::SourceFile* sf,
                 const ast::Range& range, std::string_view message) {
  const auto loc_begin = sf->ast.lines.Translate(range.begin);
  const auto loc_end = sf->ast.lines.Translate(range.end);

  cfmt::format_to(std::back_inserter(buf), cfmt::emphasis::bold, "{}:{}:{}: ", base_path.Join(sf->path),
                  loc_begin.line + 1, loc_begin.column + 1);
  cfmt::format_to(std::back_inserter(buf), cfmt::emphasis::bold | cfmt::fg(cfmt::terminal_color::red), "error: ");
  buf += message;
  buf += '\n';

  const auto append_source_line = [&](std::size_t line_no, std::string_view line, std::size_t marker_begin,
                                      std::size_t marker_end) {
    cfmt::format_to(std::back_inserter(buf), cfmt::emphasis::faint, " {:6} | ", line_no + 1);
    buf += line;
    buf += '\n';

    cfmt::format_to(std::back_inserter(buf), cfmt::emphasis::faint, " {:6} | ", "");
    buf += cfmt::detail::make_foreground_color<char>(cfmt::terminal_color::cyan);
    for (std::size_t i = 0; i < line.size(); ++i) {
      char ch = ' ';
      if (marker_begin == marker_end) {
        if (i == marker_begin) {
          ch = '^';
        }
      } else if (i >= marker_begin && i < marker_end) {
        ch = '~';
      }
      buf += ch;
    }
    cfmt::detail::reset_color(std::back_inserter(buf));
    buf += '\n';
  };

  if (loc_begin.line == loc_end.line) {
    const auto line = sf->ast.lines.RangeOf(loc_begin.line).String(sf->src);
    append_source_line(loc_begin.line, line, loc_begin.column, std::max(loc_begin.column + 1, loc_end.column));
    return;
  }

  for (std::size_t line_no = loc_begin.line; line_no <= loc_end.line; ++line_no) {
    const auto line = sf->ast.lines.RangeOf(line_no).String(sf->src);
    const auto begin = line_no == loc_begin.line ? loc_begin.column : 0;
    const auto end = line_no == loc_end.line ? loc_end.column : line.size();
    append_source_line(line_no, line, begin, end);
  }
}

std::size_t CheckErrors(const tooling::Solution& solution) {
  std::string errbuf;
  const auto print_err = [&](const core::SourceFile& sf, const ast::Range& range, std::string_view message) {
    FormatError(errbuf, solution.Directory(), &sf, range, message);
    std::cerr << errbuf;
    errbuf.clear();
  };

  std::size_t errors = 0;
  for (const auto& project : solution.Projects()) {
    for (const auto& sf : project.program.Files() | std::views::values) {
      for (const auto& err : sf.ast.errors) {
        print_err(sf, err.range, err.description);
      }
      for (const auto& err : sf.semantic_errors) {
        print_err(sf, err.range, magic_enum::enum_name(err.type));
      }
      for (const auto& ident : sf.module->unresolved) {
        print_err(sf, ident->nrange, std::format("unknown symbol '{}'", sf.Text(ident)));
      }
      for (const auto& err : sf.type_errors) {
        print_err(sf, err.range, err.message);
      }
      errors += sf.ast.errors.size() + sf.semantic_errors.size() + sf.module->unresolved.size() + sf.type_errors.size();
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
