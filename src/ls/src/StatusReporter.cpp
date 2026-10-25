#include "vanadium/ls/StatusReporter.h"

#include <chrono>
#include <format>
#include <string>

#include <vanadium/tooling/Solution.h>

#include "vanadium/lib/lserver/Connection.h"
#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerLogger.h"
#include "vanadium/ls/LanguageServerTestFlags.h"

// TODO: change existing 'vanadiumd/*' methods to '$/vanadiumd/*'

namespace {
std::size_t getOwnMemoryUsage() {
#if defined(__linux__)
  const long page_size = sysconf(_SC_PAGE_SIZE);
  std::ifstream stat_stream("/proc/self/statm", std::ios_base::in);
  if (stat_stream.is_open()) {
    std::size_t rss_pages;
    stat_stream >> rss_pages;  // Skip virtual memory sizing
    stat_stream >> rss_pages;  // Fetch RSS page layout
    return rss_pages * page_size;
  }
  return 0;
#else
  return 0;
#endif
}
}  // namespace

namespace vanadium::ls {

namespace {
bool ShouldReportStatus() {
  static constexpr auto kInterval = std::chrono::seconds{10};

  static std::chrono::steady_clock::time_point last_time{};

  const auto now = std::chrono::steady_clock::now();
  if (now - last_time < kInterval) {
    return false;
  }

  last_time = now;
  return true;
}
}  // namespace

void ReportStatus(const LsContext& ctx) {
  if (!ShouldReportStatus()) {
    return;
  }

  std::size_t projects{0};
  std::size_t modules{0};
  //
  for (const auto& project : ctx.solution->Projects()) {
    ++projects;
    modules += project.program.Files().size();
  }

  static std::string buf;
  buf.clear();

  std::format_to(std::back_inserter(buf),
                 "Server version: `{}`\n"
                 "\n---\n\n"
                 "Projects: `{}` \\\n"
                 "Total modules: `{}` \\\n"
                 "Consumed memory: `{:.1f} MB`",
                 testflags::version_name,  //
                 projects,                 //
                 modules,                  //
                 static_cast<double>(getOwnMemoryUsage()) / (1024. * 1024.));

  ctx.connection->Notify<"$/vanadiumd/status">(std::string_view{buf});

  VLS_DEBUG("Sent a status report");
}

}  // namespace vanadium::ls
