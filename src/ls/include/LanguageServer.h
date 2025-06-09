#pragma once

#include "LSTransport.h"

namespace vanadium::ls {
void Serve(lserver::Transport& transport, std::size_t concurrency, std::size_t jobs);
}
