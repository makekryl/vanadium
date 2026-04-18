#pragma once

#include <gtest/gtest.h>

#include <concepts>
#include <ranges>

namespace vanadium::testing::gtest {
namespace internal {
::testing::AssertionResult CmpHelperSTREQColoredDiff(const char* s1_expression, const char* s2_expression,
                                                     const char* s1, const char* s2);
::testing::AssertionResult CmpHelperSTREQColoredDiff(const char* lhs_expression, const char* rhs_expression,
                                                     const wchar_t* lhs, const wchar_t* rhs);
}  // namespace internal

inline void RegisterSingleTest(const char* test_suite_name, const char* test_name, std::invocable auto factory,
                               const char* value_param = nullptr) {
  // TODO: take __FILE__ & __LINE__ as argument defaults to resolve properly
  ::testing::RegisterTest(test_suite_name,       //
                          test_name,             //
                          nullptr, value_param,  //
                          __FILE__, __LINE__,    //
                          std::move(factory));
}

void RegisterTestMatrix(const char* test_suite_name, const char* test_name, std::ranges::range auto value_params,
                        auto create_factory) {
  for (const auto& param : value_params) {
    RegisterSingleTest(test_suite_name, test_name, create_factory(param), ::testing::PrintToString(param).c_str());
  }
}
}  // namespace vanadium::testing::gtest

#define EXPECT_STREQ_COLORED_DIFF(s1, s2) \
  EXPECT_PRED_FORMAT2(::vanadium::testing::gtest::internal::CmpHelperSTREQColoredDiff, s1, s2)

#define ASSERT_STREQ_COLORED_DIFF(s1, s2) \
  ASSERT_PRED_FORMAT2(::vanadium::testing::gtest::internal::CmpHelperSTREQColoredDiff, s1, s2)
