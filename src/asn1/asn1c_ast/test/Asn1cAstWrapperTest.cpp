#include <gtest/gtest.h>

#include "Arena.h"
#include "vanadium/asn/ast/Asn1cAstWrapper.h"

TEST(Asn1cAstWrapper, ParseSuccess) {
  vanadium::lib::Arena arena;

  auto res = vanadium::asn::ast::Asn1cAstWrapper::Parse(arena, R"(
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

  auto ast = std::move(*res);
  (void)ast;
}

TEST(Asn1cAstWrapper, ParseFailure) {
  vanadium::lib::Arena arena;

  auto res = vanadium::asn::ast::Asn1cAstWrapper::Parse(arena, R"(
    MyModule DEFINITIONS ::=
    BEGIN

    MyInt ::= INTEG....ER (0..65535)

    END
  )");

  ASSERT_FALSE(res);
  EXPECT_EQ(res.error().size(), 1);
}
