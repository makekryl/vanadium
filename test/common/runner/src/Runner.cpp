#include "Runner.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <print>

#include <vanadium/testing/gtest_helpers.h>

namespace {
std::optional<std::string_view> LookupCliArgument(std::span<const char* const> args, std::string_view argname) {
  for (const auto* argp : args) {
    std::string_view arg{argp};
    if (arg.starts_with(argname)) {
      arg.remove_prefix(argname.length());
      return arg;
    }
  }
  return std::nullopt;
}
}  // namespace

namespace vanadium::e2e::runner {

namespace opts {
std::filesystem::path suites_dir;
bool overwrite_snapshots{false};
}  // namespace opts

TestcaseRegistrar SingleTestRegistrar(std::string group,
                                      std::function<::testing::Test*(std::filesystem::path)> instantiate) {
  return [group = std::move(group), instantiate = std::move(instantiate)](const char* suite_name, const char* test_name,
                                                                          std::filesystem::path test_file) {
    vanadium::testing::gtest::RegisterSingleTest(std::format("{}/{}", group, suite_name).c_str(), test_name,
                                                 [test_file = std::move(test_file), instantiate]() -> ::testing::Test* {
                                                   return instantiate(std::move(test_file));
                                                 });
  };
}

int Run(int argc, char* argv[], std::string_view default_suites_dir, const TestcaseRegistrar& register_test) {
  ::testing::InitGoogleTest(&argc, argv);

  const std::span argspan{argv, argv + argc};

  opts::overwrite_snapshots = LookupCliArgument(argspan, "--vanadium_overwrite_snapshots").has_value();
  if (opts::overwrite_snapshots) {
    std::println("WARNING: Snapshot overwrite is enabled");
  }

  if (const auto& cli_testsuites_arg = LookupCliArgument(argspan, "--vanadium_testsuites_dir=")) {
    opts::suites_dir = *cli_testsuites_arg;
  } else {
    if (const auto& cli_root_arg = LookupCliArgument(argspan, "--vanadium_project_root=")) {
      opts::suites_dir = std::filesystem::path{*cli_root_arg} / std::move(default_suites_dir);
    } else {
      opts::suites_dir = std::move(default_suites_dir);
    }
  }
  //
  if (!std::filesystem::exists(opts::suites_dir)) {
    std::println(stderr, "Test suites directory does not exist: '{}'", opts::suites_dir.string());
    return 42;
  }

  for (const auto& suite_dir : std::filesystem::directory_iterator{opts::suites_dir}) {
    if (!suite_dir.is_directory()) {
      continue;
    }

    const auto& suite_name = suite_dir.path().filename().string();

    for (auto test_file : std::filesystem::directory_iterator{suite_dir}) {
      if (test_file.is_directory()) {
        continue;
      }

      constexpr std::string_view kFilenameExtension{".ttcn"};
      auto test_name = test_file.path().filename().string();
      if (!test_name.ends_with(kFilenameExtension)) {
        continue;
      }
      test_name = test_name.substr(0, test_name.length() - kFilenameExtension.length());

      register_test(suite_name.c_str(), test_name.c_str(), std::move(test_file));
    }
  }

  return RUN_ALL_TESTS();
}
}  // namespace vanadium::e2e::runner
