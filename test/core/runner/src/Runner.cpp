#include <gtest/gtest.h>

#include <filesystem>
#include <print>

#include <vanadium/testing/gtest_helpers.h>

#include "CoreTests.h"
#include "RunnerOpts.h"

namespace vanadium::e2e::core {
namespace opts {
std::filesystem::path suites_dir;
bool overwrite_snapshots{false};
}  // namespace opts
}  // namespace vanadium::e2e::core

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

// not portable
std::optional<std::string> GetGitRoot() {
  std::array<char, 512> buf;
  std::string result;

  struct FileHandleCloser {
    void operator()(FILE* f) {
      pclose(f);
    }
  };
  std::unique_ptr<FILE, FileHandleCloser> pipe(popen("git rev-parse --show-toplevel", "r"));
  if (!pipe) {
    return std::nullopt;
  }

  while (std::fgets(buf.data(), static_cast<int>(buf.size()), pipe.get()) != nullptr) {
    result += buf.data();
  }

  if (!result.empty() && result.back() == '\n') {
    result.pop_back();
  }

  return result;
}
}  // namespace

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  const std::span argspan{argv, argv + argc};

  vanadium::e2e::core::opts::overwrite_snapshots =
      LookupCliArgument(argspan, "--vanadium_overwrite_snapshots").has_value();
  if (vanadium::e2e::core::opts::overwrite_snapshots) {
    std::println("WARNING: Snapshot overwrite is enabled");
  }

  std::optional<std::string> suites_dir_opt;
  if (const auto& git_root = GetGitRoot(); git_root) {
    suites_dir_opt = std::format("{}/test/core/suites", *git_root);
  }
  if (const auto& cli_arg = LookupCliArgument(argspan, "--vanadium_testsuites_dir="); cli_arg) {
    suites_dir_opt = cli_arg;
  }
  //
  if (!suites_dir_opt) {
    std::println(stderr, "Missing argument --vanadium_testsuites_dir=<test suites dir>");
    return 1;
  }
  vanadium::e2e::core::opts::suites_dir = std::filesystem::path{*suites_dir_opt};
  if (!std::filesystem::exists(vanadium::e2e::core::opts::suites_dir)) {
    std::println(stderr, "Test suites directory does not exist: '{}'", vanadium::e2e::core::opts::suites_dir.string());
    return 1;
  }

  for (const auto& suite_dir : std::filesystem::directory_iterator{vanadium::e2e::core::opts::suites_dir}) {
    if (!suite_dir.is_directory()) {
      continue;
    }
    const auto& suite_name = suite_dir.path().filename().string();

    for (const auto& test_file : std::filesystem::directory_iterator{suite_dir}) {
      if (test_file.is_directory()) {
        continue;
      }

      constexpr std::string_view kFilenameExtension{".ttcn"};
      auto test_name = test_file.path().filename().string();
      if (!test_name.ends_with(kFilenameExtension)) {
        continue;
      }
      test_name = test_name.substr(0, test_name.length() - kFilenameExtension.length());

      vanadium::testing::gtest::RegisterSingleTest(
          suite_name.c_str(), test_name.c_str(), [suite_dir, test_file]() -> ::testing::Test* {
            return new vanadium::e2e::core::SnapshotTest(std::move(suite_dir), std::move(test_file));
          });
    }
  }

  return RUN_ALL_TESTS();
}
