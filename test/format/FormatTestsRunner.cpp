#include <gtest/gtest.h>

#include <filesystem>
#include <format>
#include <print>

#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/Parser.h>
#include <vanadium/format/AstPrinter.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/testing/gtest_helpers.h>
#include <vanadium/testing/utils.h>

#include "MatrixTestRegistrar.h"
#include "Runner.h"

namespace {

using namespace vanadium;

const auto kTestablePrintOptions = std::array{
    format::PrintOptions{
        .tab_width = 4,
        .print_width = 80,
        .max_empty_newlines = 2,
    },
    format::PrintOptions{
        .tab_width = 4,
        .print_width = 80,
        .max_empty_newlines = 1,  // <--
    },
    format::PrintOptions{
        .tab_width = 4,
        .print_width = 80,
        .max_empty_newlines = 3,  // <--
    },

    format::PrintOptions{
        .tab_width = 2,  // <--
        .print_width = 80,
        .max_empty_newlines = 2,
    },

    format::PrintOptions{
        .tab_width = 4,
        .print_width = 120,  // <--
        .max_empty_newlines = 2,
    },
};

std::string GetPrintOptionsTag(const format::PrintOptions& p) {
  return std::format("{}_{}_{}", p.tab_width, p.print_width, p.max_empty_newlines);
}

std::optional<std::string> RunFormatter(std::string_view src, const format::PrintOptions& popts) {
  lib::Arena arena;
  const auto ast = ast::Parse(arena, src);
  if (!ast.errors.empty()) {
    for (const auto& err : ast.errors) {
      std::println(stderr, " ({}:{}) :: {}", err.range.begin, err.range.end, err.description);
    }
    return std::nullopt;
  }

  lib::Arena fmt_arena;
  return format::PrintAst(ast, ast.root, fmt_arena, popts) + "\n";
}

//

class SnapshotTest : public ::testing::Test {
 public:
  SnapshotTest(std::filesystem::path file, format::PrintOptions popts)
      : file_(std::move(file)), popts_(std::move(popts)) {};
  void TestBody() override;

 private:
  const std::filesystem::path file_;
  const format::PrintOptions popts_;
};

void SnapshotTest::TestBody() {
  const std::filesystem::path snapshot_file{std::format("{}.{}.snapshot", file_.string(), GetPrintOptionsTag(popts_))};

  const auto& src = vanadium::testing::utils::ReadFile(file_);

  const auto& first_pass = RunFormatter(src, popts_);
  ASSERT_TRUE(first_pass) << "Syntax errors in source";

  const auto& second_pass = RunFormatter(*first_pass, popts_);
  if (!second_pass) {
    vanadium::testing::utils::WriteFile(std::format("{}.1.bad", snapshot_file.string()), *first_pass);
    FAIL() << "First pass produced a syntactically invalid output";
  }

  if (second_pass != first_pass) {
    vanadium::testing::utils::WriteFile(std::format("{}.1.bad", snapshot_file.string()), *first_pass);
    vanadium::testing::utils::WriteFile(std::format("{}.2.bad", snapshot_file.string()), *second_pass);
    ASSERT_STREQ_COLORED_DIFF(second_pass->c_str(), first_pass->c_str()) << "Idempotency is violated";
  }

  const bool snapshot_exists{std::filesystem::exists(snapshot_file)};
  if (snapshot_exists) {
    const auto& expected_snapshot_text = vanadium::testing::utils::ReadFile(snapshot_file);
    EXPECT_STREQ_COLORED_DIFF(second_pass->c_str(), expected_snapshot_text.c_str());
  } else {
    ADD_FAILURE() << "Snapshot file does not exist and will be created";
  }

  if (!snapshot_exists || e2e::runner::opts::overwrite_snapshots) {
    vanadium::testing::utils::WriteFile(snapshot_file, *second_pass);
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  return vanadium::e2e::runner::Run(
      argc, argv, "test/format/suites",
      e2e::runner::MatrixTestRegistrar("Format", kTestablePrintOptions, [](auto file, auto popts) {
        return new SnapshotTest(std::move(file), std::move(popts));
      }));
}

namespace vanadium::format {
// NOLINTNEXTLINE(misc-use-internal-linkage)
void PrintTo(const PrintOptions& p, std::ostream* os) {
  *os << GetPrintOptionsTag(p);
}
}  // namespace vanadium::format
