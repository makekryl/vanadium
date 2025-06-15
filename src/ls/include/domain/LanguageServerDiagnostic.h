#pragma once

#include <cstdint>
#include <vector>

#include "LSProtocol.h"
#include "Program.h"

namespace vanadium::ls {
namespace domain {

enum DiagnosticKind : std::uint8_t {
  kUnresolvedName,
};

std::vector<lsp::Diagnostic> CollectDiagnostics(const core::SourceFile& file);

}  // namespace domain
}  // namespace vanadium::ls
