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
  std::print(stdout, "{}", buf);
}

void StdioTransport::Flush() {
  std::fflush(stdout);
}

void StdioTransport::Setup() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

}  // namespace vanadium::lserver
