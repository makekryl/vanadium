#pragma once

#include <vanadium/ast/ASTNodes.h>

#include "vanadium/lint/Context.h"
#include "vanadium/lint/Rule.h"

namespace vanadium::lint {
namespace rules {

class NoEmpty final : public Rule {
 public:
  NoEmpty();
  void Register(const MatcherRegistrar&) const final;
  void Check(Context&, const ast::Node*) final;
};

}  // namespace rules
}  // namespace vanadium::lint
