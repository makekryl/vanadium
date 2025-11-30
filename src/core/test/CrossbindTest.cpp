#include <gtest/gtest.h>

#include <initializer_list>
#include <ranges>
#include <string>
#include <string_view>

#include "Program.h"
#include "Semantic.h"
#include "helpers/TestPrinters.h"
#include "utils/ASTUtils.h"

using namespace vanadium;
using namespace vanadium::core;

using FileMapping = std::pair<std::string, std::string_view>;

struct ProgramTest : public ::testing::Test {
  template <bool HasSemanticErrors = false>
  bool prepareWorkingSet(core::Program& program, const std::unordered_map<std::string, std::string_view>& files) {
    const auto read_source = [&](const std::string& path, std::string& srcbuf) -> void {
      srcbuf = std::format("module {} {{\n{}\n}}", path, files.at(path));
    };
    program.Commit([&](auto& modify) {
      for (const auto& filename : files | std::ranges::views::keys) {
        modify.update(filename, read_source);
      }
    });

    bool ok{false};
    ([&] {
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
    })();
    return ok;
  }
  bool prepareFaultyWorkingSet(core::Program& program, const std::unordered_map<std::string, std::string_view>& files) {
    return prepareWorkingSet<true>(program, files);
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

struct DirectTestParam {
  std::string_view testcase;
  //
  std::string_view moduleA;
  std::string_view moduleB;
};
DEFINE_NAMED_TEST_PARAM_PRINTER(DirectTestParam, testcase);
struct Direct : public CrossbindTest, public ::testing::WithParamInterface<DirectTestParam> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(CrossbindTest, Direct, testing::Values(
  DirectTestParam{
    .testcase = "field_into_field",
    .moduleA = R"(
      const integer imported_name := 1;
    )",
    .moduleB = R"(
      import from ModuleA all;
      const integer canary := imported_name * 2;
    )",
  },
  DirectTestParam{
    .testcase = "field_into_function",
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
  DirectTestParam{
    .testcase = "function_into_function",
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
));
// clang-format on

TEST_P(Direct, Parametrized) {
  const auto& param = GetParam();

  core::Program program;
  ASSERT_TRUE(prepareWorkingSet(program, {
                                             {kModuleA, param.moduleA},
                                             {kModuleB, param.moduleB},
                                         }));

  const auto& sf_a = program.Files().at(kModuleA);
  const auto& sf_b = program.Files().at(kModuleB);

  ASSERT_TRUE(sf_b.module->imports.contains(kModuleA));

  ASSERT_TRUE(sf_a.module->scope->Resolve("imported_name") != nullptr);
  ASSERT_TRUE(sf_b.module->scope->Resolve("canary") != nullptr);

  const auto* sym = sf_b.module->scope->Resolve("imported_name");
  ASSERT_TRUE(sym != nullptr);

  const auto* decl = sym->Declaration();
  ASSERT_TRUE(decl != nullptr);

  EXPECT_TRUE(ast::utils::IsInHierarchyOf(decl, sf_a.ast.root));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct IndirectTestParam {
  std::string_view testcase;
  //
  std::string_view moduleA;
  std::string_view moduleC;
};
DEFINE_NAMED_TEST_PARAM_PRINTER(IndirectTestParam, testcase);
struct Indirect : public CrossbindTest, public ::testing::WithParamInterface<IndirectTestParam> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(CrossbindTest, Indirect, testing::Values(
  IndirectTestParam{
    .testcase = "field_into_field",
    .moduleA = R"(
      const integer imported_name := 1;
    )",
    .moduleC = R"(
      import from ModuleB { import all };
      const integer canary := imported_name * 2;
    )"
  },
  IndirectTestParam{
    .testcase = "field_into_function",
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
  IndirectTestParam{
    .testcase = "function_into_function",
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
));
// clang-format on

TEST_P(Indirect, Parametrized) {
  const auto& param = GetParam();

  core::Program program;
  ASSERT_TRUE(prepareWorkingSet(program, {
                                             {kModuleA, param.moduleA},
                                             {kModuleB, "public import from ModuleA all;"},
                                             {kModuleC, param.moduleC},
                                         }));

  const auto& sf_a = program.Files().at(kModuleA);
  const auto& sf_b = program.Files().at(kModuleB);
  const auto& sf_c = program.Files().at(kModuleC);

  ASSERT_TRUE(sf_b.module->imports.contains(kModuleA));
  ASSERT_TRUE(sf_c.module->imports.contains(kModuleB));
  ASSERT_TRUE(std::ranges::any_of(sf_c.module->ImportsOf(kModuleB), [](const auto& desc) {
    return desc.transit;
  }));

  ASSERT_TRUE(sf_a.module->scope->Resolve("imported_name") != nullptr);
  ASSERT_TRUE(sf_c.module->scope->Resolve("canary") != nullptr);

  const auto* sym = sf_c.module->scope->Resolve("imported_name");
  ASSERT_TRUE(sym != nullptr);

  const auto* decl = sym->Declaration();
  ASSERT_TRUE(decl != nullptr);

  EXPECT_TRUE(ast::utils::IsInHierarchyOf(decl, sf_a.ast.root));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct UnsuccessfulIndirectTestParam {
  std::string_view testcase;
  //
  std::string_view moduleA;
  std::string_view moduleC;
};
DEFINE_NAMED_TEST_PARAM_PRINTER(UnsuccessfulIndirectTestParam, testcase);
using UnsuccessfulIndirectTestParamModuleB = std::string_view;
using UnsuccessfulIndirectTestParamModuleCImports = std::string_view;
struct UnsuccessfulIndirect
    : public CrossbindTest,
      public ::testing::WithParamInterface<
          std::tuple<UnsuccessfulIndirectTestParam,
                     std::tuple<UnsuccessfulIndirectTestParamModuleB, UnsuccessfulIndirectTestParamModuleCImports>>> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(CrossbindTest, UnsuccessfulIndirect, testing::Combine(
  testing::Values(
    UnsuccessfulIndirectTestParam{
      .testcase = "field_into_field",
      .moduleA = R"(
        const integer imported_name := 1;
      )",
      .moduleC = R"(
        const integer canary := imported_name * 2;
      )"
    },
    UnsuccessfulIndirectTestParam{
      .testcase = "field_into_function",
      .moduleA = R"(
        const integer imported_name := 1;
      )",
      .moduleC = R"(
        function canary() {
          var integer i := imported_name * 2;
        }
      )",
    },
    UnsuccessfulIndirectTestParam{
      .testcase = "function_into_function",
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
), [](const testing::TestParamInfo<UnsuccessfulIndirect::ParamType>& info) {
  return std::format("{}__{}", info.index, std::get<0>(info.param).testcase);
});
// clang-format on

TEST_P(UnsuccessfulIndirect, Parametrized) {
  const auto& [param, bound_imports] = GetParam();
  const auto& [moduleB, moduleCImports] = bound_imports;
  const auto moduleC = std::format("{}\n{}", moduleCImports, param.moduleC);  // NOLINT(readability-identifier-naming)

  core::Program program;

  ASSERT_TRUE(prepareWorkingSet(program, {
                                             {kModuleA, param.moduleA},
                                             {kModuleB, moduleB},
                                             {kModuleC, moduleC},
                                         }));

  const auto& sf_a = program.Files().at(kModuleA);
  const auto& sf_c = program.Files().at(kModuleC);

  ASSERT_TRUE(sf_a.module->scope->Resolve("imported_name") != nullptr);
  ASSERT_TRUE(sf_c.module->scope->Resolve("canary") != nullptr);

  EXPECT_FALSE(sf_c.module->unresolved.empty());

  const auto* sym = sf_c.module->scope->Resolve("imported_name");
  EXPECT_TRUE(sym == nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FarIndirectTestParam {
  std::string_view testcase;
  //
  std::string_view moduleA;
  std::string_view moduleD;
};
DEFINE_NAMED_TEST_PARAM_PRINTER(FarIndirectTestParam, testcase);
using FarIndirectTestParamModuleB = std::string_view;
using FarIndirectTestParamModuleC = std::string_view;
struct FarIndirect : public CrossbindTest,
                     public ::testing::WithParamInterface<
                         std::tuple<FarIndirectTestParam, FarIndirectTestParamModuleB, FarIndirectTestParamModuleC>> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(CrossbindTest, FarIndirect, testing::Combine(
  testing::Values(
    FarIndirectTestParam{
      .testcase = "field_into_field",
      .moduleA = R"(
        const integer imported_name := 1;
      )",
      .moduleD = R"(
        import from ModuleC { import all };
        const integer canary := imported_name * 2;
      )"
    },
    FarIndirectTestParam{
      .testcase = "field_into_function",
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
    FarIndirectTestParam{
      .testcase = "function_into_function",
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
  testing::Values(
    R"(
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
), [](const testing::TestParamInfo<FarIndirect::ParamType>& info) {
  return std::format("{}__{}", info.index, std::get<0>(info.param).testcase);
});
// clang-format on

TEST_P(FarIndirect, Parametrized) {
  const auto& [param, moduleB, moduleC] = GetParam();

  core::Program program;

  ASSERT_TRUE(prepareWorkingSet(program, {
                                             {kModuleA, param.moduleA},
                                             {kModuleB, moduleB},
                                             {kModuleC, moduleC},
                                             {kModuleD, param.moduleD},
                                         }));

  const auto& sf_a = program.Files().at(kModuleA);
  const auto& sf_b = program.Files().at(kModuleB);
  const auto& sf_c = program.Files().at(kModuleC);
  const auto& sf_d = program.Files().at(kModuleD);

  ASSERT_TRUE(std::ranges::any_of(sf_b.module->ImportsOf(kModuleA), [](const auto& desc) {
    return !desc.transit && desc.is_public;
  }));
  ASSERT_TRUE(std::ranges::any_of(sf_c.module->ImportsOf(kModuleB), [](const auto& desc) {
    return desc.transit && desc.is_public;
  }));
  ASSERT_TRUE(std::ranges::any_of(sf_d.module->ImportsOf(kModuleC), [](const auto& desc) {
    return desc.transit;
  }));

  ASSERT_TRUE(sf_a.module->scope->Resolve("imported_name") != nullptr);
  ASSERT_TRUE(sf_d.module->scope->Resolve("canary") != nullptr);

  const auto* sym = sf_d.module->scope->Resolve("imported_name");
  ASSERT_TRUE(sym != nullptr);

  const auto* decl = sym->Declaration();
  ASSERT_TRUE(decl != nullptr);

  EXPECT_TRUE(ast::utils::IsInHierarchyOf(decl, sf_a.ast.root));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct AugmentedDirect : public CrossbindTest, public ::testing::WithParamInterface<DirectTestParam> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(CrossbindTest, AugmentedDirect, testing::Values(
  DirectTestParam{
    .testcase = "component_into_testcase",
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
));
// clang-format on

TEST_P(AugmentedDirect, Parametrized) {
  const auto& param = GetParam();

  core::Program program;
  ASSERT_TRUE(prepareWorkingSet(program, {
                                             {kModuleA, param.moduleA},
                                             {kModuleB, param.moduleB},
                                         }));

  const auto& sf_a = program.Files().at(kModuleA);
  const auto& sf_b = program.Files().at(kModuleB);

  ASSERT_TRUE(sf_b.module->imports.contains(kModuleA));

  const auto* provider_sym = sf_a.module->scope->Resolve("Provider");
  // TODO: ASSERT_TRUE(provider_sym.Flags() & core::semantic::SymbolFlags::kStructure);
  ASSERT_TRUE(provider_sym != nullptr);
  //
  const auto* tgt_sym = sf_b.module->scope->Resolve("target");
  ASSERT_TRUE(tgt_sym != nullptr);
  ASSERT_TRUE(tgt_sym->Flags() & core::semantic::SymbolFlags::kFunction);
  ASSERT_TRUE(tgt_sym->OriginatedScope()->GetChildren().size() == 1);
  const auto* tgt_scope = tgt_sym->OriginatedScope()->GetChildren().front();

  const auto* sym = tgt_scope->Resolve("imported_name");
  ASSERT_TRUE(sym != nullptr);

  EXPECT_EQ(sym, provider_sym->OriginatedScope()->ResolveDirect("imported_name"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
