#include <gtest/gtest.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/testing/gtest_helpers.h>
#include <vanadium/testing/utils.h>

#include <algorithm>
#include <concepts>
#include <filesystem>
#include <fstream>
#include <magic_enum/magic_enum.hpp>

#include "CoreTests.h"
#include "Program.h"
#include "RunnerOpts.h"

namespace vanadium::e2e::core {

namespace {
class IndentedWriter {
 public:
  void WriteLine(std::string_view s) {
    for (std::size_t i = 0; i < indent_; ++i) {
      constexpr std::size_t kTabWidth = 2;
      for (std::size_t j = 0; j < kTabWidth; ++j) {
        buf_ += " ";
      }
    }
    buf_ += s;
    buf_ += "\n";
  }

  void Indented(std::invocable auto f) {
    ++indent_;
    f();
    --indent_;
  }

  std::string&& Build() {
    return std::move(buf_);
  }

 private:
  std::size_t indent_{0};
  std::string buf_;
};

struct Snapshot {
  std::string text;
  std::map<const vanadium::core::SourceFile*, std::vector<vanadium::ast::pos_t>> errlines;
};

Snapshot TakeSnapshot(const vanadium::core::Program& program) {
  IndentedWriter w;
  std::map<const vanadium::core::SourceFile*, std::vector<vanadium::ast::pos_t>> errlines;

  std::string highlight_buf;

  const auto write_err = [&w, &errlines, &highlight_buf](const vanadium::core::SourceFile* psf,
                                                         const vanadium::ast::Range& range, std::string_view text) {
    const auto& loc_begin = psf->ast.lines.Translate(range.begin);
    w.WriteLine(std::format("{}:{}: {}", loc_begin.line + 1, loc_begin.column + 1, text));
    errlines[psf].emplace_back(loc_begin.line);

    const auto& loc_end = psf->ast.lines.Translate(range.end);
    w.Indented([&] {
      if (loc_begin.line == loc_end.line) {
        const auto first_line = psf->ast.lines.RangeOf(loc_begin.line).String(psf->src);
        highlight_buf.resize(first_line.length());
        //
        auto it = std::fill_n(highlight_buf.begin(), loc_begin.column, ' ');
        it = std::fill_n(it, loc_end.column - loc_begin.column, '~');
        std::fill(it, highlight_buf.end(), ' ');

        w.WriteLine(first_line);
        w.WriteLine(highlight_buf);
      } else {
        for (auto l = loc_begin.line; l < loc_end.line; ++l) {
          const auto line = psf->ast.Text(psf->ast.lines.RangeOf(l));

          highlight_buf.resize(line.length());
          //
          if (l == loc_begin.line) {
            auto it = std::fill_n(highlight_buf.begin(), loc_begin.column, ' ');
            std::fill(it, highlight_buf.end(), '~');
          } else if (l == loc_end.line) {
            auto it = std::fill_n(highlight_buf.begin(), loc_begin.column, '~');
            std::fill(it, highlight_buf.end(), ' ');
          } else {
            for (std::size_t i = 0; i < line.length(); ++i) {
              if (line[i] == ' ') {
                highlight_buf[i] = ' ';
              } else {
                highlight_buf[i] = '~';
              }
            }
          }

          w.WriteLine(line);
          w.WriteLine(highlight_buf);
        }
      }
    });
  };

  std::map<std::string, const vanadium::core::SourceFile*> files;  // ordered
  for (const auto& [name, sf] : program.Files()) {
    files[name] = &sf;
  }

  for (const auto& [name, psf] : files) {
    w.WriteLine(std::format("./{}:", name));
    if (!psf->ast.errors.empty()) {
      w.Indented([&] {
        w.WriteLine("Syntax errors:");
        w.Indented([&] {
          for (const auto& err : psf->ast.errors) {
            write_err(psf, err.range, err.description);
          }
        });
      });
      continue;
    }
    if (!psf->module) {
      continue;
    }
    w.Indented([&] {
      for (const auto& ident : psf->module->unresolved) {
        write_err(psf, ident->nrange, std::format("unresolved symbol '{}'", psf->Text(ident)));
      }
      for (const auto& err : psf->semantic_errors) {
        write_err(psf, err.range, magic_enum::enum_name(err.type));
      }
      for (const auto& err : psf->type_errors) {
        write_err(psf, err.range, err.message);
      }
    });
    w.WriteLine("");
  }

  auto str = w.Build();
  while (str.ends_with('\n')) {
    str.pop_back();
  }
  str += '\n';

  return {
      .text = std::move(str),
      .errlines = std::move(errlines),
  };
}
}  // namespace

void SnapshotTest::TestBody() {
  const auto& snapshot_file = (file_.parent_path() / std::format("{}.snapshot", file_.filename().string()));

  vanadium::core::Program program;
  const auto& src = vanadium::testing::utils::ReadFile(file_);
  const auto read_src_into_sf = [&](const std::string&, std::string& srcbuf) -> void {
    srcbuf = src;
  };
  program.Commit([&](auto& modify) {
    modify.update(std::filesystem::relative(file_, suite_).string(), read_src_into_sf);
  });

  const auto actual_snapshot = TakeSnapshot(program);

  const bool snapshot_exists{std::filesystem::exists(snapshot_file)};
  if (snapshot_exists) {
    const auto expected_snapshot_text = vanadium::testing::utils::ReadFile(snapshot_file);
    EXPECT_STREQ_COLORED_DIFF(expected_snapshot_text.c_str(), actual_snapshot.text.c_str());
  } else {
    ADD_FAILURE() << "Snapshot file does not exist and will be created";
  }

  constexpr std::string_view kErrorExpectationDirective{"// EXPECT_ERROR"};
  for (const auto& [psf, errlines] : actual_snapshot.errlines) {
    for (const auto& l : errlines) {
      const auto line = psf->ast.lines.RangeOf(l).String(psf->src);
      if (!line.contains(kErrorExpectationDirective)) {
        ADD_FAILURE() << std::format("{}:{}: unexpected error", psf->path, l + 1);
      }
    }
    const auto count_occurrences = [](std::string_view haystack, std::string_view needle) -> std::size_t {
      std::size_t occurrences = 0;
      std::string_view::size_type pos = 0;
      while ((pos = haystack.find(needle, pos)) != std::string::npos) {
        ++occurrences;
        pos += needle.length();
      }
      return occurrences;
    };
    EXPECT_EQ(count_occurrences(psf->src, kErrorExpectationDirective), errlines.size())
        << "Not all error expectations were met";
  }

  if (!snapshot_exists || opts::overwrite_snapshots) {
    std::ofstream snapshot_of(snapshot_file);
    snapshot_of << actual_snapshot.text;
  }
}

}  // namespace vanadium::e2e::core
