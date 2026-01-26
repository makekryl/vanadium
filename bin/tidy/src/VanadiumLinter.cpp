#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include <argparse/argparse.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <oneapi/tbb/task_arena.h>

#include <vanadium/bin/Bootstrap.h>
#include <vanadium/core/Program.h>
#include <vanadium/lint/BuiltinRules.h>
#include <vanadium/lint/Context.h>
#include <vanadium/lint/Linter.h>
#include <vanadium/tooling/Filesystem.h>
#include <vanadium/tooling/Solution.h>
#include <vanadium/tooling/impl/SystemFS.h>

namespace {
vanadium::lint::Linter CreateLinter() {
  vanadium::lint::Linter linter;
  linter.RegisterRule<vanadium::lint::rules::NoEmpty>();
  linter.RegisterRule<vanadium::lint::rules::NoUnusedVars>();
  linter.RegisterRule<vanadium::lint::rules::NoUnusedImports>();
  linter.RegisterRule<vanadium::lint::rules::NoUnnecessaryValueof>();
  return linter;
}

int main(int argc, char* argv[]) {
  std::uint32_t jobs{std::clamp(std::thread::hardware_concurrency(), 1U, 4U)};
  bool use_autofix{false};
  std::string solution_path;

  argparse::ArgumentParser ap("vanadium-tidy");
  ap.add_description("TTCN-3 source code static analyzer");
  //
  ap.add_argument("--fix").store_into(use_autofix).help("apply autofixes where possible");
  ap.add_argument("-j", "--parallel", "").store_into(jobs).help("maximum number of worker threads");
  //
  ap.add_argument("path").store_into(solution_path).help("solution directory path");

  //
  PARSE_CLI_ARGS_OR_EXIT(ap, argc, argv, 1);
  //

  tbb::task_arena task_arena(jobs);

  const auto t_load_begin = std::chrono::steady_clock::now();
  auto solution_opt = task_arena.execute([&] {
    return vanadium::tooling::Solution::Load(
        vanadium::tooling::fs::Root<vanadium::tooling::fs::SystemFS>(solution_path));
  });
  if (!solution_opt) {
    fmt::println("{} {}", fmt::format(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error:"),
                 solution_opt.error().String());
    return 2;
  }
  auto& solution = *solution_opt;
  const auto& dir = solution.Directory();
  const auto t_load_end = std::chrono::steady_clock::now();

  //

  const auto t_lint_begin = std::chrono::steady_clock::now();

  std::size_t total_problems = 0;
  std::size_t fixed_problems = 0;
  auto linter = CreateLinter();
  for (const auto& project : solution.Projects()) {
    if (!project.managed) {
      continue;
    }

    const auto& program = project.program;
    for (const auto& [virtual_path, sf] : program.Files()) {
      if (sf.path.ends_with(".asn")) {
        continue;
      }

      fmt::print(fmt::emphasis::underline | fmt::emphasis::bold, "{}\n", project.Directory().Join(sf.path));
      if (!sf.ast.errors.empty()) {
        fmt::println("\tFile has syntax errors");
        return 2;
      }

      auto problems = linter.Lint(sf);
      if (use_autofix) {
        const auto initial_problems_count = problems.size();
        const auto&& [fixed_source, refined_problems] = linter.Fix(sf, std::move(problems));
        if (fixed_source) {
          if (const auto& err = dir.WriteFile(sf.path, *fixed_source); err) {
            fmt::println(
                "{} {}",
                fmt::format(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "failed to write to file {}:", sf.path),
                err->String());
            return 2;
          }
          fixed_problems += (refined_problems.size() >= initial_problems_count)
                                ? 0
                                : (initial_problems_count - refined_problems.size());
        }
        problems = std::move(refined_problems);
      }
      for (const auto& problem : problems) {
        const auto&& loc = sf.ast.lines.Translate(problem.range.begin);
        fmt::print(" {}   {}   {}  {}\n",
                   fmt::format(fmt::fg(fmt::color::black), "{:5}:{:<3}", loc.line + 1, loc.column + 1),
                   fmt::format(fmt::fg(fmt::color::tomato), "error"), fmt::format("{:<60}", problem.description),
                   fmt::format(fmt::fg(fmt::color::black), problem.reporter));
      }
      total_problems += problems.size();
    }
  }

  if (fixed_problems > 0) {
    fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::cornflower_blue), "\n ● Fixed {} problems\n", fixed_problems);
  }

  const bool has_problems = total_problems > 0;
  fmt::print(fmt::emphasis::bold | fmt::fg(has_problems ? fmt::color::tomato : fmt::color::green), "\n {}  {}\n",
             has_problems ? "✘" : "✔",
             has_problems ? fmt::format("{} errors detected", total_problems) : "no problems found");

  fmt::print(fmt::fg(fmt::color::cyan), "\n * Project loaded in {} ms with {} jobs\n\n",
             std::chrono::duration_cast<std::chrono::milliseconds>(t_load_end - t_load_begin).count(), jobs);
  const auto t_lint_end = std::chrono::steady_clock::now();
  fmt::print(fmt::fg(fmt::color::cyan), "\n                         ({} ms)\n",
             std::chrono::duration_cast<std::chrono::milliseconds>(t_lint_end - t_lint_begin).count());

  return has_problems ? 1 : 0;
}
}  // namespace

DEFINE_VANADIUM_ENTRYPOINT(main);
