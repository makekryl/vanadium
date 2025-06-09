#include <fmt/color.h>

#include <CLI/CLI.hpp>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <thread>

#include "Context.h"
#include "FunctionRef.h"
#include "Linter.h"
#include "Program.h"
#include "Project.h"
#include "oneapi/tbb/task_arena.h"
#include "rules/NoEmpty.h"
#include "rules/NoUnusedImports.h"
#include "rules/NoUnusedVars.h"

namespace {
vanadium::lint::Linter CreateLinter() {
  vanadium::lint::Linter linter;
  linter.RegisterRule<vanadium::lint::rules::NoEmpty>();
  linter.RegisterRule<vanadium::lint::rules::NoUnusedVars>();
  linter.RegisterRule<vanadium::lint::rules::NoUnusedImports>();
  return linter;
}
}  // namespace

int main(int argc, char* argv[]) {
  CLI::App app{"TTCN-3 Code Static Analyzer"};
  //
  std::filesystem::path manifest;
  app.add_option("project", manifest, "Vanadium project directory")  //
      ->required()
      ->transform(CLI::Validator{[](std::string& input) -> std::string {
                                   const std::filesystem::path path = std::filesystem::path(input) / ".vanadiumrc.yml";
                                   if (!std::filesystem::exists(path)) {
                                     return "project manifest does not exist";
                                   }
                                   input = path.string();
                                   return std::string{};
                                 },
                                 "Vanadium project", "Vanadium project"});
  //
  bool use_fix{false};
  app.add_flag("--fix", use_fix, "autofix if possible");
  //
  std::size_t jobs{std::thread::hardware_concurrency()};
  app.add_option("-j,--jobs", jobs, "number of jobs");
  //
  CLI11_PARSE(app, argc, argv);

  tbb::task_arena task_arena(jobs);

  const auto t_load_begin = std::chrono::steady_clock::now();

  auto project = vanadium::tooling::Project::Load(manifest);
  vanadium::core::Program program(&project->GetFS());

  task_arena.execute([&] {
    program.Commit([&](const auto& modify) {
      project->VisitFiles([&modify](auto path) {
        modify.add(std::string(path));
      });
    });
  });

  const auto t_load_end = std::chrono::steady_clock::now();

  const auto t_lint_begin = std::chrono::steady_clock::now();

  std::size_t total_problems = 0;
  std::size_t fixed_problems = 0;
  auto linter = CreateLinter();
  for (const auto& [virtual_path, sf] : program.Files()) {
    if (!sf.ast.errors.empty()) {
      std::cout << "errs in " << sf.path << std::endl;
      return 1;
    }
    fmt::print(fmt::emphasis::underline | fmt::emphasis::bold, "{}\n", (project->GetPath() / sf.path).string());

    auto problems = linter.Lint(program, sf);
    if (use_fix) {
      const auto initial_problems_count = problems.size();
      const auto&& [fixed_source, refined_problems] = linter.Fix(program, sf, std::move(problems));
      if (fixed_source) {
        if (!program.GetFS().WriteFile(sf.path, *fixed_source)) {
          // TODO
          std::puts("FILE WRITE FAILED\n");
          std::abort();
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
