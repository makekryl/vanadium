#pragma once

#include <gtest/gtest.h>

#include <filesystem>
#include <functional>
#include <string_view>

namespace vanadium::e2e::runner {

namespace opts {
extern std::filesystem::path suites_dir;
extern bool overwrite_snapshots;
}  // namespace opts

using TestcaseRegistrar =
    std::function<void(const char* suite_name, const char* test_name, std::filesystem::path test_file)>;

TestcaseRegistrar SingleTestRegistrar(std::string group,
                                      std::function<::testing::Test*(std::filesystem::path)> instantiate);

int Run(int argc, char* argv[], std::string_view default_suites_dir, const TestcaseRegistrar& register_test);

}  // namespace vanadium::e2e::runner
