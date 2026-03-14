#include <print>
#include <stdexcept>
#include <string_view>

#include "vanadium/runtime/RuntimeInternals.h"

// TODO: move out from RT

int main(int argc, char* argv[]) {
  std::println("VRT::Bootstrap()");

  const auto& modules = vanadium::rt::GetModules();
  for (const auto& mod : modules) {
    std::println(" * {}", mod->name);
    for (vrt_testcase_t** t = mod->testcases; *t; t++) {
      std::println(" | - {}", (*t)->name);
      try {
        (*t)->fn();
      } catch (const std::runtime_error& err) {
        std::println(stderr, "KAPUT : {}", err.what());
      }
    }
  }

  return 0;
}
