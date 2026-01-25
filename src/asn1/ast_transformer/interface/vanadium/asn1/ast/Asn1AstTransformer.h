#pragma once

#include <vanadium/ast/AST.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>

#include <vector>

using asn1p_t = struct asn1p_s;
using asn1p_module_t = struct asn1p_module_s;

namespace vanadium::asn1::ast {

// NOLINTNEXTLINE(readability-identifier-naming)
namespace ttcn_ast = vanadium::ast;

struct TransformedAsn1Ast {
  struct TransformationError {
    ttcn_ast::Range range;
    std::string message;
  };

  std::string_view adjusted_src;
  ttcn_ast::RootNode* root;
  std::vector<TransformationError> errors;
};

using Asn1pModuleProvider = lib::FunctionRef<const asn1p_module_t*(const char*)>;

TransformedAsn1Ast TransformAsn1Ast(const asn1p_t* ast, std::string_view src, lib::Arena& arena, Asn1pModuleProvider);

namespace compilerExtensions {
extern bool eag_grouping;
}

}  // namespace vanadium::asn1::ast
