#include "Bootstrap.h"

#include <csignal>
#include <iostream>
#include <print>
#include <sstream>
#include <stacktrace>

#if (defined(__has_feature) && __has_feature(address_sanitizer)) || defined(__SANITIZE_ADDRESS__)
#define VANADIUM_BOOTSTRAP__ASAN_ENABLED 1
#else
#define VANADIUM_BOOTSTRAP__ASAN_ENABLED 0
#endif

namespace vanadium::bin {
EntryPoint* EntryPoint::list_{nullptr};
}

namespace {
void HandleSignal(int signum) {
  std::stringstream ss;
  ss << "--- Signal (" << signum << ") has been received ---" << std::endl;
  ss << "--- STACKTRACE ---" << std::endl;
  ss << std::stacktrace::current();

  const auto str = ss.str();
  std::println(stderr, "\n{}\n", str);
  std::fflush(stderr);

  std::signal(signum, SIG_DFL);
  std::raise(signum);
}
}  // namespace

int main(int argc, char* argv[]) {
#if VANADIUM_BOOTSTRAP__ASAN_ENABLED
  // Do not mess with ASAN error reporting then
  // The line below is to avoid "unused function" warning
  (void)HandleSignal;
#else
  std::signal(SIGABRT, HandleSignal);
  std::signal(SIGSEGV, HandleSignal);
#endif

  // TODO: support multiple entry points
  return vanadium::bin::EntryPoint::list_->invoke(argc, argv);
}
