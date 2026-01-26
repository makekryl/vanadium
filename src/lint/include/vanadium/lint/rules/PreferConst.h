#pragma once

#include <vanadium/ast/ASTNodes.h>

#include "vanadium/lint/Context.h"
#include "vanadium/lint/Rule.h"

namespace vanadium::lint {
namespace rules {

class PreferConst final : public Rule {
 public:
  PreferConst();
  void Exit(Context&) final;
};

}  // namespace rules
}  // namespace vanadium::lint
