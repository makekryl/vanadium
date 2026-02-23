#pragma once

#include <optional>
#include <string_view>

namespace vanadium {

namespace tooling {
class Solution;
struct SolutionProject;
}  // namespace tooling

namespace ls {

[[nodiscard]] std::optional<std::pair<tooling::SolutionProject&, std::string>> ResolveFileUri(
    tooling::Solution& solution, std::string_view file_uri);
[[nodiscard]] std::string PathToFileUri(const tooling::Solution& solution, std::string_view path);

}  // namespace ls

}  // namespace vanadium
