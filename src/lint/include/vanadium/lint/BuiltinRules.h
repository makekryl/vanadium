#pragma once

#include <vanadium/ast/ASTNodes.h>

#include "vanadium/lint/Context.h"
#include "vanadium/lint/Rule.h"

namespace vanadium::lint {
namespace rules {

class NoEmpty final : public Rule {
 public:
  NoEmpty();
  void Register(const MatcherRegistrar& match) const final;
  void Check(Context& ctx, const ast::Node* node) final;
};

class NoUnusedImports final : public Rule {
 public:
  NoUnusedImports();
  void Exit(Context& ctx) final;
};

class NoUnusedVars final : public Rule {
 public:
  NoUnusedVars();
  void Exit(Context& ctx) final;
};

class PreferConst final : public Rule {
 public:
  PreferConst();
  void Exit(Context& ctx) final;
};

class NoUnnecessaryValueof final : public Rule {
 public:
  NoUnnecessaryValueof();
  void Register(const MatcherRegistrar& match) const final;
  void Check(Context& ctx, const ast::Node* node) final;
};

}  // namespace rules
}  // namespace vanadium::lint
