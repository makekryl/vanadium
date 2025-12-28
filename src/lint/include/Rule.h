#pragma once

#include <vanadium/ast/ASTNodes.h>

#include <string_view>

#include "FunctionRef.h"

namespace vanadium::lint {

using MatcherRegistrar = lib::Consumer<vanadium::ast::NodeKind>;

class Context;

class Rule {
 public:
  Rule(std::string_view name) : name_(name) {}

  virtual ~Rule() = default;

  virtual void Register(const MatcherRegistrar& match) const {};  // TODO: reconsider constness

  virtual void Check(Context& ctx, const ast::Node* node) {};
  virtual void Exit(Context&) {};

  [[nodiscard]] std::string_view GetName() const {
    return name_;
  }

 private:
  const std::string_view name_;
};

}  // namespace vanadium::lint
