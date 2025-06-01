#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <initializer_list>
#include <ranges>
#include <string>
#include <string_view>

#include "Arena.h"
#include "Program.h"
#include "Semantic.h"
#include "helpers/ASTHelpers.h"
#include "helpers/CommonHelpers.h"
#include "helpers/TestPrinters.h"  // IWYU pragma: keep
#include "mocks/FilesystemMock.h"

using namespace ::testing;
using namespace ::vanadium;

using FileMapping = std::pair<std::string, std::string_view>;

struct ProgramTest : public ::Test {
  StrictMock<ut::FilesystemMock> fsMock;

  lib::Arena test_temporaries_;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  template <bool HasSemanticErrors = false>
  void prepareWorkingSet(core::Program& program, std::initializer_list<FileMapping> files, bool& ok) {
    for (const auto& [filename, contents] : files) {
      expectFileRead(filename, *test_temporaries_.Alloc<std::string>(wrapModule(filename, contents)));
    }
    program.Commit([&](auto& modify) {
      for (const auto& filename : files | std::ranges::views::keys) {
        modify.add(filename);
      }
    });

    ASSERT_EQ(program.Files().size(), files.size());
    for (const auto& filename : files | std::ranges::views::keys) {
      const auto it = program.Files().find(filename);
      ASSERT_TRUE(it != program.Files().end());

      const auto& sf = it->second;
      ASSERT_TRUE(sf.ast.errors.empty()) << sf.path << " :: " << sf.ast.errors;
      if constexpr (!HasSemanticErrors) {
        ASSERT_TRUE(sf.semantic_errors.empty()) << sf.path << " :: " << sf.semantic_errors;
      }
    }

    ok = true;
  }
  void prepareFaultyWorkingSet(core::Program& program, std::initializer_list<FileMapping> files, bool& ok) {
    prepareWorkingSet<true>(program, files, ok);
  }

  void expectFileRead(std::string_view filename, std::string_view contents) {
    EXPECT_CALL(fsMock, ReadFile(filename, _)).WillOnce(Return(contents));
  }

  constexpr static std::string kModuleA = "ModuleA";
  constexpr static std::string kModuleB = "ModuleB";
  constexpr static std::string kModuleC = "ModuleC";
  constexpr static std::string kModuleD = "ModuleD";

  static std::string wrapModule(std::string_view module_name, std::string_view contents) {
    return std::format("module {} {{\n{}\n}}", module_name, contents);
  }
};

struct CrossbindTest : ProgramTest {};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct DirectCrossbindTestParam {
  std::string test_name;
  //
  std::string_view moduleA;
  std::string_view moduleB;
};
struct DirectCrossbind : public CrossbindTest, public ::testing::WithParamInterface<DirectCrossbindTestParam> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(CrossbindTest, DirectCrossbind, testing::Values(
  DirectCrossbindTestParam{
    .test_name = "field_into_field",
    .moduleA = R"(
const integer imported_name := 1;
)",
    .moduleB = R"(
import from ModuleA all;
const integer canary := imported_name * 2;
)",
  },
  DirectCrossbindTestParam{
    .test_name = "field_into_function",
    .moduleA = R"(
const integer imported_name := 1;
)",
    .moduleB = R"(
import from ModuleA all;

function canary() {
  var integer i := imported_name * 2;
}
)",
  },
  DirectCrossbindTestParam{
    .test_name = "function_into_function",
    .moduleA = R"(
function imported_name() {}
)",
    .moduleB = R"(
import from ModuleA all;
function canary() {
  imported_name();
}
)",
  }
), ut::ExplicitParametrizedTestDisplayName<DirectCrossbind::ParamType>);
// clang-format on

TEST_P(DirectCrossbind, Parametrized) {
  const auto& param = GetParam();

  core::Program program(&fsMock);
  {
    bool ok{false};
    prepareWorkingSet(program,
                      {
                          {kModuleA, param.moduleA},
                          {kModuleB, param.moduleB},
                      },
                      ok);
    ASSERT_TRUE(ok);
  }

  const auto& sf_a = program.Files().at(kModuleA);
  const auto& sf_b = program.Files().at(kModuleB);

  ASSERT_TRUE(sf_b.module.imports.contains(kModuleA));

  ASSERT_TRUE(sf_a.module.scope->Resolve("imported_name") != nullptr);
  ASSERT_TRUE(sf_b.module.scope->Resolve("canary") != nullptr);

  const auto* sym = sf_b.module.scope->Resolve("imported_name");
  ASSERT_TRUE(sym != nullptr);

  const auto* decl = sym->Declaration();
  ASSERT_TRUE(decl != nullptr);

  EXPECT_TRUE(ut::IsInHierarchyOf(decl, sf_a.ast.root));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct IndirectCrossbindTestParam {
  std::string test_name;
  //
  std::string_view moduleA;
  std::string_view moduleC;
};
struct IndirectCrossbind : public CrossbindTest, public ::testing::WithParamInterface<IndirectCrossbindTestParam> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(CrossbindTest, IndirectCrossbind, testing::Values(
  IndirectCrossbindTestParam{
    .test_name = "field_into_field",
    .moduleA = R"(
const integer imported_name := 1;
)",
    .moduleC = R"(
import from ModuleB { import all };
const integer canary := imported_name * 2;
)"
  },
  IndirectCrossbindTestParam{
    .test_name = "field_into_function",
    .moduleA = R"(
const integer imported_name := 1;
)",
    .moduleC = R"(
import from ModuleB { import all };
function canary() {
  var integer i := imported_name * 2;
}
)",
  },
  IndirectCrossbindTestParam{
    .test_name = "function_into_function",
    .moduleA = R"(
function imported_name() {}
)",
    .moduleC = R"(
import from ModuleB { import all };
function canary() {
  imported_name();
}
)",
  }
), ut::ExplicitParametrizedTestDisplayName<IndirectCrossbind::ParamType>);
// clang-format on

TEST_P(IndirectCrossbind, Parametrized) {
  const auto& param = GetParam();

  core::Program program(&fsMock);
  {
    bool ok{false};
    prepareWorkingSet(program,
                      {
                          {kModuleA, param.moduleA},
                          {kModuleB, "public import from ModuleA all;"},
                          {kModuleC, param.moduleC},
                      },
                      ok);
    ASSERT_TRUE(ok);
  }

  const auto& sf_a = program.Files().at(kModuleA);
  const auto& sf_b = program.Files().at(kModuleB);
  const auto& sf_c = program.Files().at(kModuleC);

  ASSERT_TRUE(sf_b.module.imports.contains(kModuleA));
  ASSERT_TRUE(sf_c.module.imports.contains(kModuleB));
  ASSERT_TRUE(std::ranges::any_of(sf_c.module.ImportsOf(kModuleB), [](const auto& desc) {
    return desc.transit;
  }));

  ASSERT_TRUE(sf_a.module.scope->Resolve("imported_name") != nullptr);
  ASSERT_TRUE(sf_c.module.scope->Resolve("canary") != nullptr);

  const auto* sym = sf_c.module.scope->Resolve("imported_name");
  ASSERT_TRUE(sym != nullptr);

  const auto* decl = sym->Declaration();
  ASSERT_TRUE(decl != nullptr);

  EXPECT_TRUE(ut::IsInHierarchyOf(decl, sf_a.ast.root));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct UnsuccessfulIndirectCrossbindTestParam {
  std::string test_name;
  //
  std::string_view moduleA;
  std::string_view moduleC;
};
using UnsuccessfulIndirectCrossbindTestParamModuleB = std::string_view;
using UnsuccessfulIndirectCrossbindTestParamModuleCImports = std::string_view;
struct UnsuccessfulIndirectCrossbind
    : public CrossbindTest,
      public ::testing::WithParamInterface<std::tuple<
          UnsuccessfulIndirectCrossbindTestParam, std::tuple<UnsuccessfulIndirectCrossbindTestParamModuleB,
                                                             UnsuccessfulIndirectCrossbindTestParamModuleCImports>>> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(CrossbindTest, UnsuccessfulIndirectCrossbind, testing::Combine(
  testing::Values(
    UnsuccessfulIndirectCrossbindTestParam{
      .test_name = "field_into_field",
      .moduleA = R"(
const integer imported_name := 1;
)",
      .moduleC = R"(
const integer canary := imported_name * 2;
)"
    },
    UnsuccessfulIndirectCrossbindTestParam{
      .test_name = "field_into_function",
      .moduleA = R"(
const integer imported_name := 1;
)",
      .moduleC = R"(
function canary() {
  var integer i := imported_name * 2;
}
)",
    },
    UnsuccessfulIndirectCrossbindTestParam{
      .test_name = "function_into_function",
      .moduleA = R"(
function imported_name() {}
)",
      .moduleC = R"(
function canary() {
  imported_name();
}
)",
    }
  ),
  testing::Values(
    std::tuple{
      R"(
import from ModuleA all;  // non-public
      )",
      R"(
import from ModuleB { import all };
      )"
    },
    std::tuple{
      R"(
import from ModuleA { import all };  // non-public
      )",
      R"(
import from ModuleB { import all };
      )"
    },
    std::tuple{
      R"(
public import from ModuleA { import all };  // indirect
      )",
      R"(
import from ModuleB { import all };
      )"
    },
    std::tuple{
      R"(
public import from ModuleA all;
      )",
      R"(
import from ModuleB all;  // direct
      )"
    },
    std::tuple{
      R"(
      )",
      R"(
import from ModuleB all;  // direct
      )"
    },
    std::tuple{
      R"(
public import from ModuleA all;
      )",
      R"(
      )"
    },
    std::tuple{
      R"(
      )",
      R"(
      )"
    }
  )
), [](const testing::TestParamInfo<UnsuccessfulIndirectCrossbind::ParamType>& info) {
  return std::format("{}__{}", info.index, std::get<0>(info.param).test_name);
});
// clang-format on

TEST_P(UnsuccessfulIndirectCrossbind, Parametrized) {
  const auto& [param, bound_imports] = GetParam();
  const auto& [moduleB, moduleCImports] = bound_imports;
  const auto moduleC = std::format("{}\n{}", moduleCImports, param.moduleC);  // NOLINT(readability-identifier-naming)

  core::Program program(&fsMock);
  {
    bool ok{false};
    prepareWorkingSet(program,
                      {
                          {kModuleA, param.moduleA},
                          {kModuleB, moduleB},
                          {kModuleC, moduleC},
                      },
                      ok);
    ASSERT_TRUE(ok);
  }

  const auto& sf_a = program.Files().at(kModuleA);
  const auto& sf_c = program.Files().at(kModuleC);

  ASSERT_TRUE(sf_a.module.scope->Resolve("imported_name") != nullptr);
  ASSERT_TRUE(sf_c.module.scope->Resolve("canary") != nullptr);

  EXPECT_FALSE(sf_c.module.unresolved.empty());

  const auto* sym = sf_c.module.scope->Resolve("imported_name");
  EXPECT_TRUE(sym == nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FarIndirectCrossbindTestParam {
  std::string test_name;
  //
  std::string_view moduleA;
  std::string_view moduleD;
};
using FarIndirectCrossbindTestParamModuleB = std::string_view;
using FarIndirectCrossbindTestParamModuleC = std::string_view;
struct FarIndirectCrossbind
    : public CrossbindTest,
      public ::testing::WithParamInterface<std::tuple<
          FarIndirectCrossbindTestParam, FarIndirectCrossbindTestParamModuleB, FarIndirectCrossbindTestParamModuleC>> {
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(CrossbindTest, FarIndirectCrossbind, testing::Combine(
  testing::Values(
    FarIndirectCrossbindTestParam{
      .test_name = "field_into_field",
      .moduleA = R"(
const integer imported_name := 1;
  )",
      .moduleD = R"(
import from ModuleC { import all };
const integer canary := imported_name * 2;
  )"
    },
    FarIndirectCrossbindTestParam{
      .test_name = "field_into_function",
      .moduleA = R"(
const integer imported_name := 1;
  )",
      .moduleD = R"(
import from ModuleC { import all };
function canary() {
  var integer i := imported_name * 2;
}
  )",
    },
    FarIndirectCrossbindTestParam{
      .test_name = "function_into_function",
      .moduleA = R"(
function imported_name() {
  //
}
  )",
      .moduleD = R"(
import from ModuleC { import all };
function canary() {
  imported_name();
}
  )",
    }
  ),
  testing::Values(R"(
public import from ModuleA all;
    )",
    R"(
public import from ModuleA { import all };
public import from ModuleA all;
    )",
    R"(
public import from ModuleA all;
public import from ModuleA { import all };
    )"
  ),
  testing::Values(R"(
public import from ModuleB { import all };
    )",
    R"(
public import from ModuleB { import all };
public import from ModuleB all;
    )",
    R"(
public import from ModuleB all;
public import from ModuleB { import all };
    )"
  )
), [](const testing::TestParamInfo<FarIndirectCrossbind::ParamType>& info) {
  return std::format("{}__{}", info.index, std::get<0>(info.param).test_name);
});
// clang-format on

TEST_P(FarIndirectCrossbind, Parametrized) {
  const auto& [param, moduleB, moduleC] = GetParam();

  core::Program program(&fsMock);
  {
    bool ok{false};
    prepareWorkingSet(program,
                      {
                          {kModuleA, param.moduleA},
                          {kModuleB, moduleB},
                          {kModuleC, moduleC},
                          {kModuleD, param.moduleD},
                      },
                      ok);
    ASSERT_TRUE(ok);
  }

  const auto& sf_a = program.Files().at(kModuleA);
  const auto& sf_b = program.Files().at(kModuleB);
  const auto& sf_c = program.Files().at(kModuleC);
  const auto& sf_d = program.Files().at(kModuleD);

  ASSERT_TRUE(std::ranges::any_of(sf_b.module.ImportsOf(kModuleA), [](const auto& desc) {
    return !desc.transit && desc.is_public;
  }));
  ASSERT_TRUE(std::ranges::any_of(sf_c.module.ImportsOf(kModuleB), [](const auto& desc) {
    return desc.transit && desc.is_public;
  }));
  ASSERT_TRUE(std::ranges::any_of(sf_d.module.ImportsOf(kModuleC), [](const auto& desc) {
    return desc.transit;
  }));

  ASSERT_TRUE(sf_a.module.scope->Resolve("imported_name") != nullptr);
  ASSERT_TRUE(sf_d.module.scope->Resolve("canary") != nullptr);

  const auto* sym = sf_d.module.scope->Resolve("imported_name");
  ASSERT_TRUE(sym != nullptr);

  const auto* decl = sym->Declaration();
  ASSERT_TRUE(decl != nullptr);

  EXPECT_TRUE(ut::IsInHierarchyOf(decl, sf_a.ast.root));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct AugmentedDirectCrossbind : public CrossbindTest,
                                  public ::testing::WithParamInterface<DirectCrossbindTestParam> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(CrossbindTest, AugmentedDirectCrossbind, testing::Values(
  DirectCrossbindTestParam{
    .test_name = "component_into_testcase",
    .moduleA = R"(
type component Provider {
  var integer imported_name;
}
)",
    .moduleB = R"(
import from ModuleA all;
testcase target() runs on Provider {
  imported_name := 1;
}
)"
  }
), ut::ExplicitParametrizedTestDisplayName<AugmentedDirectCrossbind::ParamType>);
// clang-format on

TEST_P(AugmentedDirectCrossbind, Parametrized) {
  const auto& param = GetParam();

  core::Program program(&fsMock);
  {
    bool ok{false};
    prepareWorkingSet(program,
                      {
                          {kModuleA, param.moduleA},
                          {kModuleB, param.moduleB},
                      },
                      ok);
    ASSERT_TRUE(ok);
  }

  const auto& sf_a = program.Files().at(kModuleA);
  const auto& sf_b = program.Files().at(kModuleB);

  ASSERT_TRUE(sf_b.module.imports.contains(kModuleA));

  const auto* provider_sym = sf_a.module.scope->Resolve("Provider");
  // TODO: ASSERT_TRUE(provider_sym.Flags() & core::semantic::SymbolFlags::kStructure);
  ASSERT_TRUE(provider_sym != nullptr);
  //
  const auto* tgt_sym = sf_b.module.scope->Resolve("target");
  ASSERT_TRUE(tgt_sym != nullptr);
  ASSERT_TRUE(tgt_sym->Flags() & core::semantic::SymbolFlags::kFunction);
  ASSERT_TRUE(tgt_sym->OriginatedScope()->GetChildren().size() == 1);
  const auto* tgt_scope = tgt_sym->OriginatedScope()->GetChildren().front();

  const auto* sym = tgt_scope->Resolve("imported_name");
  ASSERT_TRUE(sym != nullptr);

  EXPECT_EQ(sym, provider_sym->Members()->Lookup("imported_name"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
