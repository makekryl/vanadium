#include <glaze/json/generic.hpp>

#include "LSMethod.h"
#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "vanadium/lib/jsonrpc/Common.h"

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
template <>
void methods::initialized::invoke(LsContext& ctx, const lib::jsonrpc::Empty&) {
  ctx.connection->Request<"client/registerCapability", std::nullptr_t>(lsp::RegistrationParams{
      .registrations =
          {
              lsp::Registration{
                  .id = kRegistrationId,
                  .method = "workspace/didChangeWatchedFiles",
                  .registerOptions = generify(lsp::DidChangeWatchedFilesRegistrationOptions{
                      .watchers =
                          {
                              lsp::FileSystemWatcher{.globPattern = "**/*.ttcn"},
                              lsp::FileSystemWatcher{.globPattern = "**/.vanadiumrc"},
                          },
                  }),
              },
          },
  });
}
}  // namespace vanadium::ls
