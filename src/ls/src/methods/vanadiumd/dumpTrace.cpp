#include <filesystem>
#include <fstream>

#include <LSProtocol.h>

#include <vanadium/lib/jsonrpc/Common.h>
#include <vanadium/lib/trace/GlobalTracer.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"

namespace vanadium::ls {
rpc::ExpectedResult<std::string> methods::vanadiumd::dumpTrace::invoke(LsContext&, const lib::jsonrpc::Empty&) {
  if (!trace::global.Active()) {
    return std::unexpected(lib::jsonrpc::Error{
        .code = lib::jsonrpc::ErrorCode::kInternal,
        .data = std::nullopt,
        .message = "Tracing is not enabled",
    });
  }

  // TODO: this won't be so easy when/if LS becomes concurrent

  std::filesystem::path path{trace::global_save_path};

  std::ofstream of(path);
  trace::global.Unwrap()->Serialize([&](auto sv) {
    of << sv;
  });
  of << "\n";

  trace::global.Unwrap()->Clear();

  return path;
}
}  // namespace vanadium::ls
