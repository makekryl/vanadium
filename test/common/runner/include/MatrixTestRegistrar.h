#pragma once

#include <ranges>

#include <vanadium/testing/gtest_helpers.h>

#include "Runner.h"

namespace vanadium::e2e::runner {

template <std::ranges::range R, typename F>
  requires std::invocable<F&, std::filesystem::path, std::ranges::range_reference_t<R>>
TestcaseRegistrar MatrixTestRegistrar(std::string group, R&& params, F&& instantiate) {
  return [group = std::move(group), params, instantiate](const char* suite_name, const char* test_name,
                                                         std::filesystem::path test_file) {
    vanadium::testing::gtest::RegisterTestMatrix(std::format("{}/{}", group, suite_name).c_str(), test_name, params,
                                                 [test_file = std::move(test_file), instantiate](auto&& param) {
                                                   return [test_file = std::move(test_file), instantiate,
                                                           param]() -> ::testing::Test* {
                                                     return instantiate(std::move(test_file), param);
                                                   };
                                                 });
  };
}

}  // namespace vanadium::e2e::runner
