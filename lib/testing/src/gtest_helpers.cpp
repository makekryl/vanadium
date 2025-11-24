#include "vanadium/testing/gtest_helpers.h"

#include <ranges>

#include "gtest/gtest.h"

namespace {
testing::AssertionResult MonkeyPatchCmpHelperSTREQ(::testing::AssertionResult&& ar, const char* lhs_expression,
                                                   const char* rhs_expression) {
  if (ar) {  // -> success
    return ar;
  }

  ::testing::Message msg;
  const auto msg_write_colored = [&msg](std::string_view text, std::string_view ccode) {
    msg << "\033[" << ccode << text << "\033[m";
  };

  msg << "Expected equality of these values:";
  msg << "\n  " << lhs_expression;
  msg << "\n  " << rhs_expression;

  const std::string_view fmsg{ar.failure_message()};

  constexpr std::string_view kNeedle{"\nWith diff:\n"};
  const auto pos = fmsg.find(kNeedle);
  if (pos != std::string_view::npos) {
    const std::string_view diff = fmsg.substr(pos + kNeedle.length());
    msg << kNeedle;
    if (GTEST_FLAG_GET(color) == "no") {
      msg << diff;
    } else {
      for (const auto& line_v : std::views::split(diff, '\n')) {
        const std::string_view line{line_v};
        if (line.starts_with("@@ ") && line.ends_with(" @@")) {
          msg_write_colored(line, "36m");
        } else if (line.starts_with("-")) {
          msg_write_colored(line, "31m");
        } else if (line.starts_with("+")) {
          msg_write_colored(line, "32m");
        } else {
          msg << line;
        }
        msg << "\n";
      }
    }
  }

  return ::testing::AssertionFailure() << msg;
}
}  // namespace

::testing::AssertionResult vanadium::testing::gtest::internal::CmpHelperSTREQColoredDiff(const char* s1_expression,
                                                                                         const char* s2_expression,
                                                                                         const char* s1,
                                                                                         const char* s2) {
  return MonkeyPatchCmpHelperSTREQ(::testing::internal::CmpHelperSTREQ(s1_expression, s2_expression, s1, s2),
                                   s1_expression, s2_expression);
}

::testing::AssertionResult vanadium::testing::gtest::internal::CmpHelperSTREQColoredDiff(const char* lhs_expression,
                                                                                         const char* rhs_expression,
                                                                                         const wchar_t* lhs,
                                                                                         const wchar_t* rhs) {
  return MonkeyPatchCmpHelperSTREQ(::testing::internal::CmpHelperSTREQ(lhs_expression, rhs_expression, lhs, rhs),
                                   lhs_expression, rhs_expression);
}
