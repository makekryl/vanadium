#include "vanadium/bin/Bootstrap.h"

#include <csignal>
#include <print>
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
  const auto trace = std::stacktrace::current();

  std::println(stderr,
               "\n"
               "--- Signal ({}) has been received ---"
               "\n"
               "--- STACKTRACE ---"
               "\n"
               "{}"
               "\n",
               signum, trace);
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
