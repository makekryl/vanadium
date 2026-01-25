#pragma once

#include <LSProtocol.h>
#include <vanadium/core/Program.h>

#include <vector>

#include "vanadium/ls/LanguageServerSession.h"

namespace vanadium::ls {
namespace detail {

std::vector<lsp::Diagnostic> CollectDiagnostics(const core::SourceFile& file, LsSessionRef d);

}  // namespace detail
}  // namespace vanadium::ls
