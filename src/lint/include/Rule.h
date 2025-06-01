#pragma once

#include <string_view>

#include "ASTNodes.h"
#include "FunctionRef.h"

namespace vanadium::lint {

using MatcherRegistrar = lib::Consumer<vanadium::core::ast::NodeKind>;

class Context;

class Rule {
 public:
  Rule(std::string_view name) : name_(name) {}

  virtual ~Rule() = default;

  virtual void Register(const MatcherRegistrar& match) const = 0;  // TODO: reconsider constness

  virtual void Check(Context& ctx, const core::ast::Node* node) = 0;
  virtual void Exit(Context&) {};

  [[nodiscard]] std::string_view GetName() const {
    return name_;
  }

 private:
  const std::string_view name_;
};

}  // namespace vanadium::lint
