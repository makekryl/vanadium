#pragma once

#include <set>
#include <string>

#include "ASTTypes.h"
#include "Program.h"
#include "Rule.h"

namespace vanadium::lint {

struct Autofix {
  ast::Range range;
  std::string replacement;

  [[nodiscard]] auto operator<=>(const Autofix& other) const {
    return range <=> other.range;
  }

  static Autofix Removal(const core::SourceFile& file, const ast::Range& range) {
    return {
        .range =
            {
                .begin = range.begin,
                .end = range.end + (file.ast.src[range.end] == '\n' ? 1 : 0),
            },
        .replacement = "",
    };
  }
};

struct Problem {
  ast::Range range;
  std::string description;
  std::string_view reporter;
  std::optional<Autofix> autofix;

  [[nodiscard]] auto operator<=>(const Problem& other) const {
    return range <=> other.range;
  }
};

using ProblemSet = std::set<Problem>;

class Context {
 public:
  Context(const core::SourceFile& sf) : sf_(&sf) {}

  void Report(const Rule* reporter, const ast::Range& range, std::string&& message,
              std::optional<Autofix>&& autofix = std::nullopt) {
    problems_.emplace(range, std::move(message), reporter->GetName(), std::move(autofix));
  }

  [[nodiscard]] const ProblemSet& GetProblems() const {
    return problems_;
  }

  ProblemSet&& GetProblems() {
    return std::move(problems_);
  }

  [[nodiscard]] const core::Program& GetProgram() const {
    return *sf_->program;
  }

  [[nodiscard]] const core::SourceFile& GetFile() const {
    return *sf_;
  }

 private:
  ProblemSet problems_;
  const core::SourceFile* sf_;
};

}  // namespace vanadium::lint
