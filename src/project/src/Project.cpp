#include "Project.h"

#include <cassert>
#include <filesystem>
#include <ryml.hpp>

#include "Arena.h"
#include "utils/FileReader.h"

namespace {
std::string_view RymlSubstrToStringView(const ryml::csubstr &str) {
  return {str.data(), str.size()};
}
}  // namespace

namespace vanadium {
namespace tooling {

std::optional<std::string_view> Project::GetProperty(std::string_view path) const {
  const auto &d = (*tree_)[path.data()];
  return RymlSubstrToStringView(d.val());
}

bool Project::VisitPropertyList(std::string_view path, const lib::Predicate<std::string_view> &pred) const {
  const auto &d = (*tree_)[path.data()];
  for (const auto &v : d) {
    if (!pred(RymlSubstrToStringView(v.key()))) {
      break;
    }
  }
  return true;
}

bool Project::VisitPropertyDict(std::string_view path,
                                const lib::Predicate<std::string_view, std::string_view> &pred) const {
  const auto &d = (*tree_)[path.data()];
  for (const auto &v : d) {
    if (!pred(RymlSubstrToStringView(v.key()), RymlSubstrToStringView(v.val()))) {
      break;
    }
  }
  return true;
}

Project::Project(std::filesystem::path path) : path_(std::move(path)) {}

void Project::VisitFiles(const lib::Consumer<std::string_view> &accept) const {
  for (const auto &v : (*tree_)["project"]["sources"]) {
    const auto &src_path = v.val();
    for (const auto &entry : std::filesystem::directory_iterator(path_ / RymlSubstrToStringView(src_path))) {
      if (entry.is_directory()) continue;
      if (entry.path().extension() != ".ttcn") continue;

      accept(std::filesystem::relative(entry.path(), path_).string());
    }
  }
}

void Project::Init() {
  // TODO
  vfs_.AddSearchPath(path_.string() + "/");
}

std::optional<Project> Project::Load(const std::filesystem::path &config_path) {
  assert(std::filesystem::exists(config_path));

  Project project(config_path.parent_path());
  auto source = core::utils::ReadFile(config_path, [&](std::size_t size) {
    return project.arena_.AllocStringBuffer(size).data();
  });

  if (!source) {
    // TODO
    std::fprintf(stderr, "Failed to read project configuration\n");
    std::abort();
  }
  ryml::substr ss{const_cast<char *>(source->data()), source->length()};

  // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage)
  project.tree_ = project.arena_.Alloc<c4::yml::Tree>(ryml::parse_in_place(ss));

  project.Init();

  return project;
}

}  // namespace tooling
}  // namespace vanadium
