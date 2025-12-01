#pragma once

#include "vanadium/lib/lserver/Transport.h"

namespace vanadium::ls {
void Serve(lserver::Transport& transport, std::size_t concurrency, std::size_t jobs);
}
