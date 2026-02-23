#include "vanadium/ls/LanguageServerContext.h"

#include <vanadium/core/Program.h>
#include <vanadium/lint/Linter.h>
#include <vanadium/tooling/Solution.h>

namespace vanadium::ls {

LsContext::LsContext(lserver::Connection& connection) : connection(&connection) {}

LsContext::~LsContext() = default;

const core::SourceFile* LsContext::ResolveSourceFile(std::string_view file_uri) const {
  if (auto resolution = ResolveFileUri(file_uri)) {
    auto& [project, path] = *resolution;
    return project.program.GetFile(path);
  }
  return nullptr;
}

}  // namespace vanadium::ls
