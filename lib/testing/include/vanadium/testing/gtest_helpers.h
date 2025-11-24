#include <gtest/gtest.h>

namespace vanadium::testing::gtest {
namespace internal {
::testing::AssertionResult CmpHelperSTREQColoredDiff(const char* s1_expression, const char* s2_expression,
                                                     const char* s1, const char* s2);
::testing::AssertionResult CmpHelperSTREQColoredDiff(const char* lhs_expression, const char* rhs_expression,
                                                     const wchar_t* lhs, const wchar_t* rhs);
}  // namespace internal

template <typename Factory>
void RegisterSingleTest(const char* test_suite_name, const char* test_name, Factory factory) {
  ::testing::RegisterTest(test_suite_name,     //
                          test_name,           //
                          nullptr, nullptr,    //
                          __FILE__, __LINE__,  //
                          std::move(factory));
}
}  // namespace vanadium::testing::gtest

#define EXPECT_STREQ_COLORED_DIFF(s1, s2) \
  EXPECT_PRED_FORMAT2(::vanadium::testing::gtest::internal::CmpHelperSTREQColoredDiff, s1, s2)
