#include <argparse/argparse.hpp>
#include <chrono>
#include <csignal>
#include <iostream>
#include <print>
#include <string_view>
#include <thread>

#include "Bootstrap.h"
#include "LSTransport.h"
#include "LanguageServer.h"

namespace {
int main(int argc, char* argv[]) {
  argparse::ArgumentParser ap("vanadiumd");
  ap.add_description("TTCN-3 language server");
  //
  std::uint32_t jobs{std::clamp(std::thread::hardware_concurrency(), 1U, 4U)};
  ap.add_argument("-j", "--parallel", "").store_into(jobs).help("maximum number of worker threads");
  std::uint32_t concurrency{1};  // >1 is not supported at the moment
  ap.add_argument("--concurrency").store_into(concurrency).help("number of concurrently served LS requests");
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

  vanadium::lserver::StdioTransport::Setup();
  vanadium::lserver::StdioTransport transport;

  vanadium::ls::Serve(transport, concurrency, jobs);

  return 0;
}
}  // namespace

DEFINE_VANADIUM_ENTRYPOINT(main);
