#pragma once

#include <string_view>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/lib/FunctionRef.h>

namespace vanadium::lint {

using MatcherRegistrar = lib::Consumer<vanadium::ast::NodeKind>;

class Context;

class Rule {
 public:
  Rule(std::string_view name) : name_(name) {}

  virtual ~Rule() = default;

  virtual void Register(const MatcherRegistrar&) const {};  // TODO: reconsider constness

  virtual void Check(Context&, const ast::Node*) {};
  virtual void Exit(Context&) {};

  [[nodiscard]] std::string_view GetName() const {
    return name_;
  }

 private:
  const std::string_view name_;
};

}  // namespace vanadium::lint
