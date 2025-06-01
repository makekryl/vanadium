#pragma once

#include <cstdio>
#include <format>
#include <ranges>

#include "ASTNodes.h"
#include "Context.h"
#include "Rule.h"

namespace vanadium::lint {
namespace rules {

class Agent final : public Rule {
 public:
  Agent() : Rule("import") {}

  void Register(const MatcherRegistrar&) const final {}

  void Check(Context&, const core::ast::Node*) final {}

  void Exit(Context& ctx) final {
    for (const auto& import : ctx.GetFile().module.imports | std::ranges::views::keys) {
      ctx.Report(this, {}, std::format("`{}`", import));
    }
  };
};

}  // namespace rules
}  // namespace vanadium::lint
