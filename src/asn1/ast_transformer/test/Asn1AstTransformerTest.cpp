#include <gtest/gtest.h>

#include <vanadium/asn1/ast/Asn1cAstWrapper.h>
#include <vanadium/lib/Arena.h>

#include "vanadium/asn1/ast/Asn1AstTransformer.h"

using namespace vanadium;
using namespace vanadium::asn1::ast;

namespace {
const asn1p_t* StubModuleProvider(const char*) {
  return nullptr;
}
}  // namespace

TEST(Asn1AstTransformerTest, ParametrizedConstructs) {
  // Inspired by the [TITAN Programmers' Technical Reference Guide] par. 6.4
  constexpr std::string_view kAsnSource = R"(
    Test DEFINITIONS AUTOMATIC TAGS ::=
    BEGIN

    ASequenceType ::= SEQUENCE {
      a INTEGER
    }

    ERROR-CLASS ::= CLASS
    {
      &category INTEGER,
      &code     ASequenceType,
      &Type
    }
    WITH SYNTAX {
      CATEGORY &category
      CODE &code
      TYPE &Type
    }

    ErrorType1 ::= INTEGER
    ErrorType2 ::= REAL
    ErrorType4 ::= GeneralString

    errorClass3 ERROR-CLASS ::= {
      CATEGORY 3
      CODE 1
      TYPE OCTET STRING
    }

    ErrorSet ERROR-CLASS ::=
    {
      {
        CATEGORY 1
        CODE 1
        TYPE ErrorType1
      } |
      {
        CATEGORY 2
        CODE 234567895
        TYPE ErrorType2
      } |
      errorClass3 |
      {
        CATEGORY 4
        CODE 2
        TYPE ErrorType4
      }
    }

    Error ::= SEQUENCE
    {
      errorCategory ERROR-CLASS.&category  ({ErrorSet}),
      errorCode     ERROR-CLASS.&code      ({ErrorSet}{@errorCategory}),
      errorInfo     ERROR-CLASS.&Type      ({ErrorSet}{@errorCategory})
    }

    ErrorReturn ::= SEQUENCE
    {
      errs SEQUENCE OF Error
    }

    END
  )";

  lib::Arena arena;

  const auto& ast = Parse(arena, kAsnSource);
  ASSERT_TRUE(ast);

  TransformAsn1Ast(ast.Raw(), kAsnSource, arena, [](const char*) -> const asn1p_module_t* {
    return nullptr;
  });

  // todo:
  //  0) there should not be any errors in TTCN ast
  //  1) check absence of ERROR-CLASS and ErrorSet in TTCN AST
  //  2) check presence of Error
  //  3) check Error fields expansion
}
