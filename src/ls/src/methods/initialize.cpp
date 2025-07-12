#include <filesystem>
#include <memory>
#include <ranges>

#include "FsDirectory.h"
#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Program.h"
#include "Project.h"

namespace vanadium::ls {

namespace {
void InitSubproject(SubprojectEntry& subproject, std::string path, std::unique_ptr<core::IDirectory>&& dir,
                    std::string name, std::vector<std::string> references) {
  // TODO: think about merging path&dir by relativizing dir->Path()
  subproject.name = std::move(name);
  subproject.path = std::move(path);
  subproject.dir = std::move(dir);
  subproject.references = std::move(references);

  const auto read_file = [&](std::string_view path, vanadium::lib::Arena& arena) -> std::string_view {
    const auto contents = subproject.dir->ReadFile(path, [&](std::size_t size) {
      return arena.AllocStringBuffer(size).data();
    });
    if (!contents) {
      // TODO, +requires IDirectory::ReadFile signature modification
      std::fprintf(stderr, "Failed to read file '%s' on init, I'm dying :( :)\n", path.data());
      std::fflush(stderr);
      std::abort();
    }
    return *contents;
  };

  subproject.program.Update([&](const auto& modify) {
    subproject.dir->VisitFiles([&](const std::string& path) {
      if (!path.ends_with(".ttcn")) {
        return;
      }
      modify.update(std::format("{}/{}", subproject.path, path), read_file);
    });
  });
}

void LoadProject(LsContext& ctx) {
  {
    auto result = tooling::Project::Load(ctx->root_directory / tooling::Project::kDescriptorFilename);
    if (!result.has_value()) {
      // TODO
      std::fprintf(stderr, "Failed to load project: %s\n", result.error().What().c_str());
      return;
    }

    ctx->root_project = std::move(*result);
  }

  const auto& root_desc = ctx->root_project->Read();

  if (root_desc.external) {
    for (const auto& [ext_name, ext_desc] : *root_desc.external) {
      auto& subproject = ctx->subprojects[ext_name];
      InitSubproject(subproject, ext_desc.path, ctx->root_project->Directory().Subdirectory(ext_desc.path), ext_name,
                     ext_desc.references.value_or(std::vector<std::string>{}));
    }
  }

  const auto add_subproject = [&](std::string_view path, const tooling::ProjectDescriptor& desc) {
    auto& subproject = ctx->subprojects[desc.project.name];
    InitSubproject(subproject, std::string{path}, ctx->root_project->Directory().Subdirectory(path), desc.project.name,
                   desc.project.references.value_or(std::vector<std::string>{}));
  };
  if (root_desc.project.subprojects) {
    for (const auto& path : *root_desc.project.subprojects) {
      auto result = tooling::Project::Load(std::filesystem::path(ctx->root_project->Directory().Path()) / path /
                                           tooling::Project::kDescriptorFilename);
      assert(result.has_value());
      add_subproject(path, result->Read());
    }
  } else {
    add_subproject("", root_desc);
  }

  for (auto& [name, subproj] : ctx->subprojects) {
    for (const auto& ref : subproj.references) {
      core::Program* program{nullptr};
      if (auto it = ctx->subprojects.find(ref); it != ctx->subprojects.end()) {
        program = &it->second.program;
      } else {
        // TODO
        std::fprintf(stderr, "reference '%s' of '%s' could not be satisfied\n", ref.c_str(), name.c_str());
        std::fflush(stderr);
        std::abort();
      }
      subproj.program.AddReference(program);
    }
  }

  for (auto& proj : ctx->subprojects | std::views::values) {
    proj.program.Commit([](auto&) {});
  }
}
}  // namespace

template <>
rpc::ExpectedResult<lsp::InitializeResult> methods::initialize::operator()(LsContext& ctx,
                                                                           const lsp::InitializeParams& params) {
  if (params.workspaceFolders) {
    const auto& folders = *params.workspaceFolders;

    // TODO
    const auto prefix_size = std::string_view{"file://"}.size();
    ctx->root_directory =
        std::filesystem::path{folders[0].uri.substr(prefix_size, folders[0].uri.size() - prefix_size)};

    auto descriptor_path = ctx->root_directory / tooling::Project::kDescriptorFilename;
    if (std::filesystem::exists(descriptor_path)) {
      LoadProject(ctx);
    }
  }

  return lsp::InitializeResult{
      .capabilities =
          lsp::ServerCapabilities{
              .textDocumentSync = lsp::TextDocumentSyncKind::kFull,
              .completionProvider =
                  lsp::CompletionOptions{
                      .triggerCharacters = {{"."}},
                  },
              .hoverProvider = true,
              .definitionProvider = true,
              .typeDefinitionProvider = true,
              .referencesProvider = true,
              .documentHighlightProvider = true,
              .codeActionProvider = true,
              .renameProvider = true,
              // .diagnosticProvider =
              //     lsp::DiagnosticOptions{
              //         .identifier = "vanadium",
              //         .interFileDependencies = true,
              //     },
          },
      .serverInfo =
          lsp::ServerInfo{
              .name = "vanadiumd",
              .version = "0.0.0.3",
          },
  };
}

}  // namespace vanadium::ls
