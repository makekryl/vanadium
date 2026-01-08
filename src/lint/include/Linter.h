#pragma once

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/lib/FunctionRef.h>

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "Context.h"
#include "Program.h"
#include "Rule.h"

namespace vanadium::lint {

class Linter {
 public:
  template <class Rule>
  void RegisterRule();

  void Lint(const core::Program& program,
            const lib::FunctionRef<void(const core::SourceFile&, ProblemSet)>& report) const;
  [[nodiscard]] ProblemSet Lint(const core::SourceFile& sf) const;

  std::pair<std::optional<std::string>, ProblemSet> Fix(const core::SourceFile& sf, ProblemSet&& problems) const;

 private:
  void BindRule(Rule& rule);

  std::unordered_map<ast::NodeKind, std::vector<Rule*>> matching_;
  std::vector<std::unique_ptr<Rule>> rules_;
};

template <class Rule>
void Linter::RegisterRule() {
  auto& rule = rules_.emplace_back(std::make_unique<Rule>());
  BindRule(*rule);
}

}  // namespace vanadium::lint
