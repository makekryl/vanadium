#include <glaze/json/generic.hpp>

#include <LSProtocol.h>

#include "vanadium/lib/jsonrpc/Common.h"
#include "vanadium/lib/lserver/Connection.h"
#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"

namespace {
// (obviously) move this uh... "constant" somewhere else and modify it if we would need to deregister
constexpr std::string_view kRegistrationId = "123";

// TODO: propose to move into glaze or move somewhere else anyway
template <typename T>
[[nodiscard]] glz::generic generify(const T& val) {
  glz::generic g;
  // we also deal with glz::generic in InlayHint&CodeActions,
  // but it require less allocations there, because it is coded by hand
  // while glz::generic's operator= serializes T to JSON and deserializes back to generic
  g = val;
  return g;
}
}  // namespace

namespace vanadium::ls {
void methods::initialized::invoke(LsContext& ctx, const lib::jsonrpc::Empty&) {
  ctx.connection->Request<"client/registerCapability", lserver::NoAwaitResponse /*<std::nullptr_t>*/>(
      lsp::RegistrationParams{
          .registrations =
              {
                  lsp::Registration{
                      .id = kRegistrationId,
                      .method = "workspace/didChangeWatchedFiles",
                      .registerOptions = generify(lsp::DidChangeWatchedFilesRegistrationOptions{
                          .watchers = ([&] -> std::vector<lsp::FileSystemWatcher> {
                            std::vector<lsp::FileSystemWatcher> watchers = {
                                lsp::FileSystemWatcher{.globPattern = "**/*.ttcn"},
                            };
                            for (const auto& sproj : ctx.solution->Projects()) {
                              for (const auto& search_path : sproj.project.SearchPaths()) {
                                watchers.emplace_back(std::format("{}/**/*.ttcn", search_path.base_path));
                              }
                            }
                            return watchers;
                          })(),
                      }),
                  },
              },
      });
}
}  // namespace vanadium::ls
