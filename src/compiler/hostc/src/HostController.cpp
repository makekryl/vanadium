#include <print>
#include <stdexcept>

#include <magic_enum/magic_enum.hpp>

#include <vanadium/runtime/rt_verdict.h>

#include "vanadium/hostc/RuntimeInternals.h"

using namespace vanadium;

int main(int argc, char* argv[]) {
  std::println("VRT::Bootstrap()");

  const auto& modules = hostc::GetModules();
  for (const auto& mod : modules) {
    std::println(" * {}", mod->name);
    for (vrt_testcase_t** t = mod->testcases; *t; t++) {
      vrt_clearverdict();
      std::println(" | - {}", (*t)->name);
      try {
        (*t)->fn();
      } catch (const std::runtime_error& err) {
        std::println(stderr, "KAPUT : {}", err.what());
      }
      std::println("   . verdict: {}", magic_enum::enum_name(vrt_getverdict()));
    }
  }

  return 0;
}
