#pragma once

#include <set>
#include <string>

#include "ASTTypes.h"
#include "Program.h"
#include "Rule.h"

namespace vanadium::lint {

struct Autofix {
  core::ast::Range range;
  std::string replacement;

  [[nodiscard]] auto operator<=>(const Autofix& other) const {
    return range <=> other.range;
  }

  static Autofix Removal(const core::ast::Range& range) {
    return {
        .range = range,
        .replacement = "",
    };
  }
};

struct Problem {
  core::ast::Range range;
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
  Context(const core::Program& program, const core::SourceFile& sf) : program_(&program), sf_(&sf) {}

  void Report(const Rule* reporter, const core::ast::Range& range, std::string&& message,
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
    return *program_;
  }

  [[nodiscard]] const core::SourceFile& GetFile() const {
    return *sf_;
  }

 private:
  ProblemSet problems_;
  const core::Program* program_;
  const core::SourceFile* sf_;
};

}  // namespace vanadium::lint
