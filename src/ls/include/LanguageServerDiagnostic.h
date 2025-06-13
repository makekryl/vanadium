#pragma once

#include <cstdint>

namespace vanadium::ls {
namespace domain {

enum DiagnosticKind : std::uint8_t {
  kUnresolvedName,
};

}
}  // namespace vanadium::ls
