#include <argparse/argparse.hpp>
#include <chrono>
#include <csignal>
#include <iostream>
#include <print>
#include <stacktrace>
#include <string_view>
#include <thread>

#include "LSTransport.h"
#include "LanguageServer.h"

#ifndef NDEBUG
namespace {
void HandleSignal(int signum) {
  std::cerr << "RECEIVED SIGNAL " << signum << std::endl;
  std::cerr << "STACKTRACE:" << std::endl;
  std::cerr << std::stacktrace::current();
  std::cerr.flush();
  std::exit(signum);  // TODO: call original sighandler
}
}  // namespace
#endif

int main(int argc, char* argv[]) {
  argparse::ArgumentParser ap("vanadiumd");
  ap.add_description("TTCN-3 language server");
  //
  ap.add_argument("--wait-dbg").flag();
  //
  auto& group = ap.add_mutually_exclusive_group();
  group.add_argument("--stdio").help("run LSP over standard IO").flag();

  /////////////////////////////////////////////
  try {
    ap.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << ap;
    return 1;
  }
  /////////////////////////////////////////////

  if (ap["--wait-dbg"] == true) {  // == true, yes
    constexpr auto kSecondsToWait{12};
    std::println(stderr, "\nWaiting {} seconds for debugger to attach...\n", kSecondsToWait);
    std::this_thread::sleep_for(std::chrono::seconds(kSecondsToWait));
  }

#ifndef NDEBUG
  std::signal(SIGABRT, HandleSignal);
  std::signal(SIGSEGV, HandleSignal);
#endif

  vanadium::lserver::StdioTransport::Setup();
  vanadium::lserver::StdioTransport transport;

  vanadium::ls::Serve(transport, 3, 4);

  return 0;
}
