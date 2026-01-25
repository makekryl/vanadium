#include "vanadium/ls/LanguageServerSolution.h"

#include <filesystem>
#include <optional>
#include <string_view>

#include "Solution.h"

namespace vanadium::ls {

static constexpr std::string_view kFileSchema = "file://";
static constexpr auto kFileSchemaLength = kFileSchema.size();

std::optional<std::pair<tooling::SolutionProject&, std::string>> ResolveFileUri(tooling::Solution& solution,
                                                                                std::string_view file_uri) {
  if (!file_uri.starts_with(kFileSchema)) {
    return std::nullopt;
  }

  const std::string_view path = file_uri.substr(kFileSchemaLength, file_uri.size() - kFileSchemaLength);

  auto* project = solution.ProjectOf(path);
  if (!project) {
    return std::nullopt;
  }
  return {{*project, std::filesystem::relative(path, solution.Directory().base_path).string()}};
}

std::string PathToFileUri(const tooling::Solution& solution, std::string_view path) {
  return std::format("{}{}", kFileSchema, (std::filesystem::path(solution.Directory().base_path) / path).string());
}

}  // namespace vanadium::ls
