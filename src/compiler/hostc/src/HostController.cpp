#include <print>
#include <stdexcept>
#include <utility>

#include <magic_enum/magic_enum.hpp>

#include <vanadium/runtime/rt_verdict.h>

#include "vanadium/hostc/RuntimeInternals.h"
#include "vanadium/runtime/rt_integer.h"

using namespace vanadium;

int main(int argc, char* argv[]) {
  std::println("VRT::Bootstrap()");

  const auto& modules = hostc::GetModules();
  for (const auto& mod : modules) {
    for (vrt_testcase_t** t = mod->testcases; *t; t++) {
      std::println("Test case {} started.", (*t)->name);
      vrt_clearverdict();
      std::println(" | - {}", (*t)->name);
      try {
        (*t)->fn();
      } catch (const std::runtime_error& err) {
        std::println(stderr, "KAPUT : {}", err.what());
        auto errverdict = vrt_integer_wrap(std::to_underlying(vrt_verdicttype::error));
        vrt_setverdict(&errverdict, nullptr);
      }
      std::println("Test case {} finished. Verdict: {}", mod->name, magic_enum::enum_name(vrt_getverdict()));
    }
  }

  return 0;
}
