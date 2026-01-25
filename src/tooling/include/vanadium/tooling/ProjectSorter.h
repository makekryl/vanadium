#pragma once

// TODO: split include/ into include/ and interface/, keep this header private

#include <optional>
#include <unordered_map>
#include <vector>

#include "vanadium/tooling/Solution.h"

namespace vanadium::tooling {

std::optional<std::vector<const SolutionProject*>> SortProjects(
    const std::unordered_map<std::string, SolutionProject>&);

}
