#include <algorithm>
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
    for (const auto& tc : std::span{mod->testcases, mod->testcases_count}) {
      std::println("Test case {} started.", tc.name);
      vrt_clearverdict();
      try {
        tc.fn();
      } catch (const std::runtime_error& err) {
        std::println(stderr, "KAPUT : {}", err.what());
        auto errverdict = vrt_integer_wrap(std::to_underlying(vrt_verdicttype::error));
        vrt_setverdict(&errverdict, nullptr);
      }
      std::println("Test case {} finished. Verdict: {}", tc.name, magic_enum::enum_name(vrt_getverdict()));
    }
  }

  return 0;
}
