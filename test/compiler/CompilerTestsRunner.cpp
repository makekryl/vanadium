#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <print>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>

#include <vanadium/ast/ASTTypes.h>
#include <vanadium/compiler/Compiler.h>
#include <vanadium/core/Program.h>
#include <vanadium/runtime/rt_reflect.h>
#include <vanadium/runtime/rt_verdict.h>
#include <vanadium/runtime/runtime.h>
#include <vanadium/testing/gtest_helpers.h>
#include <vanadium/testing/utils.h>

#include "Runner.h"

namespace {
const vrt_module_t* vmodule{};
std::vector<std::string> logged_lines{};
}  // namespace

extern "C" {
void vrt_log_write(const char* s) {
  logged_lines.emplace_back(s);
}

void vrt_register_module(const vrt_module_t* m) {
  vmodule = m;
}
}

namespace {

using namespace vanadium;

//
class VerdictTest : public ::testing::Test {
 public:
  VerdictTest(std::filesystem::path file) : file_(std::move(file)) {};
  void TestBody() override;

  void TearDown() override {
    vmodule = nullptr;
  }

 private:
  const std::filesystem::path file_;
};

void VerdictTest::TestBody() {
  core::Program program;
  const auto& src = vanadium::testing::utils::ReadFile(file_);
  const auto read_src_into_sf = [&](const std::string&, std::string& srcbuf) -> void {
    srcbuf = src;
  };
  const auto sf_name = std::filesystem::relative(file_, file_.parent_path()).string();
  program.Commit([&](auto& modify) {
    modify.update(sf_name, read_src_into_sf);
  });

  const auto* sf = program.GetFile(sf_name);
  ASSERT_TRUE(sf);  // TODO: check for errors

  // unique_ptr is required for ThreadSafeModule ownership transfer
  auto ctx = std::make_unique<llvm::LLVMContext>();
  auto mod = std::make_unique<llvm::Module>(file_.filename().string(), *ctx);

  compiler::Compile(*sf, {.debug = true}, *mod);

  ASSERT_TRUE(llvm::verifyModule(*mod));

  auto jit = cantFail(llvm::orc::LLJITBuilder().create());
  auto& dylib = jit->getMainJITDylib();

  dylib.addGenerator(
      cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(jit->getDataLayout().getGlobalPrefix())));

  cantFail(jit->addIRModule(llvm::orc::ThreadSafeModule{std::move(mod), std::move(ctx)}));

  cantFail(jit->initialize(dylib));
  ASSERT_TRUE(vmodule);
  for (auto** tc = vmodule->testcases; *tc; tc++) {
    vrt_clearverdict();
    logged_lines.clear();

    std::println(" . {}", (*tc)->name);
    std::fflush(stdout);
    try {
      (*tc)->fn();
    } catch (const std::runtime_error& err) {
      ADD_FAILURE() << "Testcase '" << (*tc)->name << "' raised an error: '" << err.what() << "'";
      auto errverdict = vrt_integer_wrap(std::to_underlying(vrt_verdicttype::error));
      vrt_setverdict(&errverdict, nullptr);
    }

    const auto verdict = vrt_getverdict();
    if (verdict == vrt_verdicttype::pass) {
      continue;
    }

    ADD_FAILURE() << "Testcase '" << (*tc)->name << "' failed with verdict: " << magic_enum::enum_name(verdict);
    std::println(stderr, "Testcase '{}' log:", (*tc)->name);
    for (const auto& line : logged_lines) {
      std::println(stderr, " | {}", line);
    }
  }
  cantFail(jit->deinitialize(dylib));
}

}  // namespace

int main(int argc, char* argv[]) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  return e2e::runner::Run(argc, argv, "test/compiler/suites",
                          e2e::runner::SingleTestRegistrar("Compiler", [](auto file) {
                            return new VerdictTest(std::move(file));
                          }));
}
