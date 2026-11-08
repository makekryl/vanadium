#include "vanadium/core/utils/WithUtils.h"

#include <vanadium/ast/AST.h>

#include "vanadium/core/WithParser.h"

namespace vanadium::core::utils {

TemplateOmitMode ResolveTemplateOmitMode(const ast::AST& ast, const ast::nodes::TemplateDecl* n) {
  TemplateOmitMode mode = TemplateOmitMode::kExplicit;  // TODO: take default from module's 'with'
  ParseTemplateWithSpec(ast, n->with,
                        {
                            .emit_error = [&](auto, std::string&&) {},
                            .accept_optional =
                                [&](const auto& parse_optional) {
                                  parse_optional({
                                      .implicit_omit =
                                          [&] {
                                            mode = TemplateOmitMode::kImplicit;
                                          },
                                  });
                                },
                        });
  return mode;
}

}  // namespace vanadium::core::utils
