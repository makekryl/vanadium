#pragma once

#include <vanadium/ast/AST.h>
#include <vanadium/lib/Arena.h>

#include <vector>

using asn1p_t = struct asn1p_s;

namespace vanadium::asn1::ast {

// NOLINTNEXTLINE(readability-identifier-naming)
namespace ttcn3_ast = vanadium::ast;

struct TransformedAsn1Ast {
  struct Error {
    ttcn3_ast::Range range;
    std::string message;
  };

  std::string_view adjusted_src;
  ttcn3_ast::RootNode* root;
  std::vector<Error> errors;
};

TransformedAsn1Ast TransformAsn1Ast(const asn1p_t* ast, std::string_view src, lib::Arena& arena);

}  // namespace vanadium::asn1::ast
