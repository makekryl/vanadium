#pragma once

#include <string>

#include <vanadium/ast/ASTNodes.h>

#include "vanadium/ast/AST.h"
#include "vanadium/ast/ASTTypes.h"
#include "vanadium/lib/FunctionRef.h"

namespace vanadium::core {

struct ModuleWithAttributesConsumer {
  lib::Consumer<ast::Range, std::string&&> emit_error;

  struct ExtensionAnytypeConsumer {
    lib::Consumer<ast::Token> accept_member;
  };
  lib::Consumer<ast::Token, lib::Consumer<const ExtensionAnytypeConsumer&>> accept_extension_anytype;
};
void ParseModuleWithSpec(const ast::AST&, const ast::nodes::WithSpec*, const ModuleWithAttributesConsumer&);

struct TemplateWithAttributesConsumer {
  lib::Consumer<ast::Range, std::string&&> emit_error;

  struct OptionalConsumer {
    lib::FunctionRef<void()> implicit_omit;
  };
  lib::Consumer<lib::Consumer<const OptionalConsumer&>> accept_optional;
};
void ParseTemplateWithSpec(const ast::AST&, const ast::nodes::WithSpec*, const TemplateWithAttributesConsumer&);

}  // namespace vanadium::core
