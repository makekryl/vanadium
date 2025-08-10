#pragma once

#include <optional>
#include <string_view>

#include "Solution.h"

namespace vanadium::ls {
namespace helpers {

[[nodiscard]] std::optional<std::pair<tooling::SolutionProject&, std::string>> ResolveFileUri(
    tooling::Solution& solution, std::string_view file_uri);
[[nodiscard]] std::string PathToFileUri(const tooling::Solution& solution, std::string_view path);

}  // namespace helpers
}  // namespace vanadium::ls
