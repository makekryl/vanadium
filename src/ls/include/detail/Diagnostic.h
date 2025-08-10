#pragma once

#include <vector>

#include "LSProtocol.h"
#include "LanguageServerSession.h"
#include "Program.h"

namespace vanadium::ls {
namespace detail {

std::vector<lsp::Diagnostic> CollectDiagnostics(const core::SourceFile& file, LsSessionRef d);

}  // namespace detail
}  // namespace vanadium::ls
