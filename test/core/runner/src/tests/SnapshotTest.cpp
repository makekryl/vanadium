#include <gtest/gtest.h>
#include <vanadium/testing/gtest_helpers.h>
#include <vanadium/testing/utils.h>

#include <concepts>
#include <filesystem>
#include <fstream>
#include <magic_enum/magic_enum.hpp>

#include "ASTTypes.h"
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

std::string TakeSnapshot(const ::vanadium::core::Program& program) {
  IndentedWriter w;
  const auto file_loc = [](const ::vanadium::core::SourceFile* psf, ::vanadium::core::ast::pos_t pos) -> std::string {
    const auto& loc = psf->ast.lines.Translate(pos);
    return std::format("{}:{}", loc.line + 1, loc.column + 1);
  };

  std::map<std::string, const ::vanadium::core::SourceFile*> files;  // ordered
  for (const auto& [name, sf] : program.Files()) {
    files[name] = &sf;
  }

  for (const auto& [name, psf] : files) {
    if (!psf->ast.errors.empty()) {
      w.WriteLine(std::format("./{}:", name));
      w.Indented([&] {
        w.WriteLine("Syntax errors:");
        w.Indented([&] {
          for (const auto& err : psf->ast.errors) {
            w.WriteLine(std::format("{}: {}", file_loc(psf, err.range.begin), err.description));
          }
        });
      });
      continue;
    }
    if (!psf->module) {
      continue;
    }
    w.WriteLine(std::format("module {}:", psf->module->name));
    w.Indented([&] {
      for (const auto& ident : psf->module->unresolved) {
        w.WriteLine(std::format("{}: unresolved symbol '{}'", file_loc(psf, ident->nrange.begin), psf->Text(ident)));
      }
      for (const auto& err : psf->semantic_errors) {
        w.WriteLine(std::format("{}: {}", file_loc(psf, err.range.begin), magic_enum::enum_name(err.type)));
      }
      for (const auto& err : psf->type_errors) {
        w.WriteLine(std::format("{}: {}", file_loc(psf, err.range.begin), err.message));
      }
    });
    w.WriteLine("");
  }

  auto str = w.Build();
  while (str.ends_with('\n')) {
    str.pop_back();
  }
  str += '\n';

  return str;
}
}  // namespace

void SnapshotTest::TestBody() {
  const auto& snapshot_file = (file_.parent_path() / std::format("{}.snapshot", file_.filename().string()));

  ::vanadium::core::Program program;
  const auto read_src_into_sf = [&](const std::string&, std::string& srcbuf) -> void {
    srcbuf = vanadium::testing::utils::ReadFile(file_);
  };
  program.Commit([&](auto& modify) {
    modify.update(std::filesystem::relative(file_, suite_).string(), read_src_into_sf);
  });

  const auto actual_snapshot = TakeSnapshot(program);

  const bool snapshot_exists{std::filesystem::exists(snapshot_file)};
  if (snapshot_exists) {
    const auto expected_snapshot = vanadium::testing::utils::ReadFile(snapshot_file);
    EXPECT_STREQ_COLORED_DIFF(expected_snapshot.c_str(), actual_snapshot.c_str());
  } else {
    ADD_FAILURE() << "Snapshot file does not exist and will be created";
  }

  if (!snapshot_exists || opts::overwrite_snapshots) {
    std::ofstream snapshot_of(snapshot_file);
    snapshot_of << actual_snapshot;
  }
}

}  // namespace vanadium::e2e::core
