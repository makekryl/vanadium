#include "vanadium/ls/LanguageServerToolsOptions.h"

#include <vanadium/tooling/Project.h>
#include <vanadium/tooling/ProjectSpecReader.h>

namespace vanadium::ls::tools {

std::expected<PartialManifest, Error> ReadPartialManifest(const tooling::Project& project) {
  return project.ReadSpec<PartialManifest>();
}

}  // namespace vanadium::ls::tools
