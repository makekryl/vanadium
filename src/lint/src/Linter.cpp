#include "Linter.h"

#include <algorithm>
#include <numeric>
#include <ranges>

#include "Context.h"
#include "Program.h"

namespace vanadium::lint {

void Linter::BindRule(Rule& rule) {
  rule.Register([&](vanadium::core::ast::NodeKind kind) {
    matching_[kind].push_back(&rule);
  });
}

void Linter::Lint(const core::Program& program,
                  const lib::FunctionRef<void(const core::SourceFile&, ProblemSet)>& report) const {
  for (const auto& [filename, sf] : program.Files()) {
    report(sf, Lint(sf));
  }
}

ProblemSet Linter::Lint(const core::SourceFile& sf) const {
  if (!sf.module.has_value()) {
    return {};
  }

  Context ctx(sf);

  sf.ast.root->Accept([&](const vanadium::core::ast::Node* n) {
    const auto it = matching_.find(n->nkind);
    if (it != matching_.end()) {
      for (auto* rule : it->second) {
        rule->Check(ctx, n);
      }
    }
    return true;
  });
  for (auto& rule : rules_) {
    rule->Exit(ctx);
  }

  return std::move(ctx.GetProblems());
}

std::pair<std::optional<std::string>, ProblemSet> Linter::Fix(const core::SourceFile& sf, ProblemSet&& problems) const {
  if (problems.empty()) {
    return {std::nullopt, std::move(problems)};
  }

  std::set<Autofix> fixes_buf;
  const auto collect_fixes = [&fixes_buf](ProblemSet&& problems) -> const std::set<Autofix>& {
    fixes_buf.clear();
    for (const auto& problem : problems) {
      if (problem.autofix.has_value()) {
        fixes_buf.insert(std::move(*problem.autofix));
      }
    }
    return fixes_buf;
  };

  std::string buf;
  std::string sbuf;
  const auto attempt_fix = [&buf, &sbuf](std::string& source, const std::set<Autofix>& fixes) -> void {
    const std::size_t predicted_size =
        source.length() +
        std::accumulate(fixes.begin(), fixes.end(), std::int64_t(0), [](std::int64_t acc, const Autofix& fix) {
          return acc + (static_cast<std::int64_t>(fix.replacement.length()) - fix.range.Length());
        });
    buf.reserve(predicted_size);
    sbuf.reserve(predicted_size);

    auto last_index = source.length();
    for (const auto& fix : fixes | std::ranges::views::reverse) {
      if (fix.range.end > last_index) {
        continue;
      }

      //// buf = fix.replacement + source.substr(fix.range.end, last_index - fix.range.end) + buf; ////

      sbuf.clear();
      sbuf += fix.replacement;
      sbuf += source.substr(fix.range.end, last_index - fix.range.end);
      sbuf += buf;
      std::swap(buf, sbuf);

      last_index = fix.range.begin;
    }
    source = source.substr(0, last_index) + buf;
  };

  std::string fixed_source(sf.ast.src);

  constexpr std::size_t kMaxPasses = 10;
  std::size_t pass;
  for (pass = 0; pass < kMaxPasses; pass++) {
    const auto& fixes = collect_fixes(std::move(problems));
    if (fixes.empty()) {
      break;
    }

    attempt_fix(fixed_source, fixes);

    // TODO: update SourceFile via Program
    //       maybe stop injecting Filesystem into Program and supply file contents externally
    //       + verify that emitted code can be parsed without errors at all beforehand
    // This stub is needed just to collect stats
    std::erase_if(problems, [](const Problem& p) {
      return p.autofix.has_value();
    });
    break;
    // problems = Lint(program, sf);
  }

  if (pass == kMaxPasses) {
    throw "Failed to fix file";  // TODO
  }

  return {fixed_source, problems};
}

}  // namespace vanadium::lint
