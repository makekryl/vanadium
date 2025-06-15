#include <cstdio>
#include <iostream>
#include <print>
#include <span>
#include <string_view>

#include "LSTransport.h"

namespace vanadium::lserver {

void StdioTransport::Read(std::span<char> chunk) {
  std::fread(chunk.data(), chunk.size(), 1, stdin);
}

void StdioTransport::ReadLine(std::span<char> chunk) {
  std::fgets(chunk.data(), chunk.size(), stdin);
}

void StdioTransport::Write(std::string_view buf) {
  std::println(stderr, "WRITE({})", buf);
  std::fputs(buf.data(), stdout);  // TODO: check null-termination guarantees from our side
}

void StdioTransport::Flush() {
  std::fflush(stdout);
}

void StdioTransport::Setup() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

}  // namespace vanadium::lserver
