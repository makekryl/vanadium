#pragma once

#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"

namespace vanadium::ls {
namespace clientMessaging {

lsp::ShowMessageRequestResult ShowMessage(LsContext&, lsp::ShowMessageRequestParams&&);

}  // namespace clientMessaging
}  // namespace vanadium::ls
