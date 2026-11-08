#pragma once

#include <cstdint>

#include <vanadium/ast/AST.h>

namespace vanadium::core::utils {

enum class TemplateOmitMode : std::uint8_t {
  kExplicit,
  kImplicit,
};

TemplateOmitMode ResolveTemplateOmitMode(const ast::AST&, const ast::nodes::TemplateDecl*);

}  // namespace vanadium::core::utils
