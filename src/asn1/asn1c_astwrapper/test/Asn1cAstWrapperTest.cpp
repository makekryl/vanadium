#include <gtest/gtest.h>

#include <vanadium/lib/Arena.h>

#include "vanadium/asn1/ast/Asn1cAstWrapper.h"

// NOTE: tests below DO NOT and SHOULD NOT check the correctness of the parser:
//       it is in scope of (currently non-existent) asn1c_libasn1parser tests

using namespace vanadium;
using namespace vanadium::asn1::ast;

TEST(Asn1cAstWrapper, ParseSuccess) {
  vanadium::lib::Arena arena;

  auto res = Parse(arena, R"(
    MyModule DEFINITIONS ::=
    BEGIN

    MyTypes ::= SEQUENCE {
        myObjectId   OBJECT IDENTIFIER,
        mySeqOf      SEQUENCE OF MyInt,
        myBitString  BIT STRING {
                            muxToken(0),
                            modemToken(1)
                    }
    }

    MyInt ::= INTEGER (0..65535)

    END
  )");

  EXPECT_TRUE(res);
}

TEST(Asn1cAstWrapper, ParseFailure) {
  vanadium::lib::Arena arena;

  auto res = Parse(arena, R"(
    MyModule DEFINITIONS ::=
    BEGIN

    MyInt ::= INTEG....ER (0..65535)

    END
  )");

  ASSERT_FALSE(res);
  EXPECT_EQ(res.Errors().size(), 1);
}
