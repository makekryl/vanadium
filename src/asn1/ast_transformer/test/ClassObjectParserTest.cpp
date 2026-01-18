#include <asn1c/libasn1common/genhash.h>
#include <asn1c/libasn1parser/asn1parser_cxx.h>
#include <gtest/gtest.h>
#include <vanadium/asn1/ast/Asn1cAstWrapper.h>
#include <vanadium/lib/Arena.h>

#include <string_view>

#include "vanadium/asn1/ast/ClassObjectParser.h"

using namespace vanadium;
using namespace vanadium::asn1::ast;

TEST(ClassObjectParserTest, abcd) {
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
        CODE 2
        TYPE ErrorType1
      } |
      {
        CATEGORY 3
        CODE 4
        TYPE ErrorType2
      } |
      errorClass3 |
      {
        CATEGORY 5
        CODE 6
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

  const auto* mod = TQ_FIRST(&(ast->Get()->modules));
  ASSERT_TRUE(mod);

  const auto* cls = (asn1p_expr_t*)genhash_get(mod->members_hash, "ERROR-CLASS");
  ASSERT_TRUE(cls);
  ASSERT_TRUE(cls->with_syntax);

  const auto* set = (asn1p_expr_t*)genhash_get(mod->members_hash, "ErrorSet");
  ASSERT_TRUE(set);
  ASSERT_EQ(set->constraints->el_count, 4);

  const auto* constr = set->constraints->elements[0];
  ASSERT_TRUE(constr);

  const auto& fields = ParseClassObject(
      std::string_view{(char*)constr->value->value.string.buf, (size_t)constr->value->value.string.size},
      cls->with_syntax);

  EXPECT_EQ(fields, (ClassObjectList{
                        {"&category", "1"},
                        {"&code", "2"},
                        {"&Type", "ErrorType1"},
                    }));
}
