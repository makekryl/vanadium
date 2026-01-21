#include <asn1c/libasn1common/genhash.h>
#include <asn1c/libasn1parser/asn1parser_cxx.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vanadium/asn1/ast/Asn1cAstWrapper.h>
#include <vanadium/lib/Arena.h>

#include <string_view>
#include <vector>

#include "vanadium/asn1/ast/ClassObjectParser.h"

using namespace ::testing;

using namespace vanadium;
using namespace vanadium::asn1::ast;

// this looks more like a parametrized test though, but it will require pain with naming, TODO

// TODO: add error cases

struct ClassObjectParserTest : public ::testing::Test {
 protected:
  lib::Arena arena_;

  MockFunction<bool(ClassObjectRow)> f_accept_row_mock_;
  MockFunction<void(const asn1p_src_range_t&, std::string)> f_emit_error_mock_;

 private:
  // due to FunctionRef functions lifetime requirements (stack-use-after-return otherwise)
  decltype(f_accept_row_mock_.AsStdFunction()) f_accept_row_mock_FUNC_OBJ_ = f_accept_row_mock_.AsStdFunction();
  decltype(f_emit_error_mock_.AsStdFunction()) f_emit_error_mock_FUNC_OBJ_ = f_emit_error_mock_.AsStdFunction();

 protected:
  const ClassObjectConsumer class_object_consumer_{
      .accept_row = f_accept_row_mock_FUNC_OBJ_,
      .emit_error = f_emit_error_mock_FUNC_OBJ_,
  };

  //

  struct TestParam {
    const std::vector<ClassObjectRow>& expected_rows;
    const std::vector<std::string> expected_errors;
    bool expected_wsyntax_chunks = true;
  };

  void RunTest(std::string_view src, const char* class_name, const char* obj_name, const TestParam& test_param) {
    const auto& ast = Parse(arena_, src);
    ASSERT_TRUE(ast);

    const auto* mod = TQ_FIRST(&(ast->Raw()->modules));
    ASSERT_TRUE(mod);

    const auto* cls = (asn1p_expr_t*)genhash_get(mod->members_hash, class_name);
    ASSERT_TRUE(cls);
    ASSERT_EQ(cls->meta_type, AMT_OBJECTCLASS);
    ASSERT_EQ(cls->expr_type, A1TC_CLASSDEF);
    ASSERT_EQ(!!cls->with_syntax, test_param.expected_wsyntax_chunks);

    const auto* obj = (asn1p_expr_t*)genhash_get(mod->members_hash, obj_name);
    ASSERT_TRUE(obj);
    ASSERT_EQ(obj->meta_type, AMT_VALUE);
    ASSERT_EQ(obj->expr_type, A1TC_REFERENCE);

    ON_CALL(f_accept_row_mock_, Call(_)).WillByDefault(Return(true));
    {
      InSequence s;
      for (const auto& row : test_param.expected_rows) {
        EXPECT_CALL(f_accept_row_mock_, Call(Eq(row)));
      }
    }

    {
      InSequence s;
      for (const auto& err : test_param.expected_errors) {
        EXPECT_CALL(f_emit_error_mock_, Call(_, Eq(err)));
      }
    }

    ParseClassObject(
        std::string_view{
            (char*)obj->value->value.string.buf,
            (size_t)obj->value->value.string.size,
        },
        cls->with_syntax, class_object_consumer_);
  }
};

TEST_F(ClassObjectParserTest, WithSyntax_Values) {
  constexpr std::string_view kAsnSource = R"(
    Test DEFINITIONS AUTOMATIC TAGS ::=
    BEGIN

    ERROR-CLASS ::= CLASS
    {
      &category INTEGER,
      &code     ASequenceType
    }
    WITH SYNTAX {
      CATEGORY &category
      CODE &code
    }

    errorClass1 ERROR-CLASS ::= {
      CATEGORY 1
      CODE 2
    }

    END
  )";

  RunTest(kAsnSource, "ERROR-CLASS", "errorClass1",
          {.expected_rows = {
               {.name = "&category", .value = "1"},
               {.name = "&code", .value = "2"},
           }});
}

TEST_F(ClassObjectParserTest, WithSyntax_OpenType) {
  constexpr std::string_view kAsnSource = R"(
    Test DEFINITIONS AUTOMATIC TAGS ::=
    BEGIN

    ERROR-CLASS ::= CLASS
    {
      &category INTEGER,
      &code     INTEGER,
      &Type
    }
    WITH SYNTAX {
      CATEGORY &category
      CODE &code
      TYPE &Type
    }

    errorClass1 ERROR-CLASS ::= {
      CATEGORY 1
      CODE 2
      TYPE INTEGER
    }

    END
  )";

  RunTest(kAsnSource, "ERROR-CLASS", "errorClass1",
          {.expected_rows = {
               {.name = "&category", .value = "1"},
               {.name = "&code", .value = "2"},
               {.name = "&Type", .value = "INTEGER"},
           }});
}

TEST_F(ClassObjectParserTest, WithSyntax_OpenType_Multiword) {
  constexpr std::string_view kAsnSource = R"(
    Test DEFINITIONS AUTOMATIC TAGS ::=
    BEGIN

    ERROR-CLASS ::= CLASS
    {
      &category INTEGER,
      &code     INTEGER,
      &Type
    }
    WITH SYNTAX {
      CATEGORY &category
      CODE &code
      TYPE &Type
    }

    errorClass1 ERROR-CLASS ::= {
      CATEGORY 1
      CODE 2
      TYPE OCTET STRING
    }

    END
  )";

  RunTest(kAsnSource, "ERROR-CLASS", "errorClass1",
          {.expected_rows = {
               {.name = "&category", .value = "1"},
               {.name = "&code", .value = "2"},
               {.name = "&Type", .value = "OCTET STRING"},
           }});
}
