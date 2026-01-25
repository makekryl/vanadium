#include "vanadium/tooling/ProjectSorter.h"

#include <optional>
#include <queue>
#include <ranges>
#include <vector>

#include "vanadium/tooling/Project.h"

namespace vanadium::tooling {

std::optional<std::vector<const SolutionProject*>> SortProjects(
    const std::unordered_map<std::string, SolutionProject>& projects) {
  std::unordered_map<const SolutionProject*, std::vector<const SolutionProject*>> adj;
  //
  std::unordered_map<const SolutionProject*, int> indegree;
  indegree.reserve(projects.size());
  for (const auto& proj : projects | std::views::values) {
    indegree[&proj] = 0;
  }

  for (const auto& proj : projects | std::views::values) {
    if (!proj.project.Manifest().project.references) {
      continue;
    }
    for (const auto& dep_name : *(proj.project.Manifest().project.references)) {
      const auto& dep = projects.at(dep_name);
      adj[&dep].push_back(&proj);
      indegree[&proj]++;
    }
  }

  auto cmp = [](const SolutionProject* a, const SolutionProject* b) {
    return a->Name() > b->Name();
  };
  std::priority_queue<const SolutionProject*, std::vector<const SolutionProject*>, decltype(cmp)> q(cmp);

  for (auto& [proj, deg] : indegree) {
    if (deg == 0) {
      q.push(proj);
    }
  }

  std::vector<const SolutionProject*> sorted;
  sorted.reserve(projects.size());

  while (!q.empty()) {
    const auto* u = q.top();
    q.pop();
    sorted.push_back(u);

    for (const auto* v : adj[u]) {
      if (--indegree[v] == 0) {
        q.push(v);
      }
    }
  }

  if (sorted.size() != projects.size()) {
    return std::nullopt;
  }

  return sorted;
}

}  // namespace vanadium::tooling
