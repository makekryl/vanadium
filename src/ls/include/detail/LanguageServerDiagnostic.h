#pragma once

#include <cstdint>
#include <vector>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "Program.h"

namespace vanadium::ls {
namespace detail {

enum DiagnosticKind : std::uint8_t {
  kUnresolvedName,
};

std::vector<lsp::Diagnostic> CollectDiagnostics(LsContext& ctx, const core::Program& program,
                                                const core::SourceFile& file);

}  // namespace detail
}  // namespace vanadium::ls
