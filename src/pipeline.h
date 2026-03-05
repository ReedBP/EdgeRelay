#pragma once

#include "message_sink.h"

#include <cstddef>

namespace edgerelay {

void run_pipeline(std::size_t count, MessageSink& sink);

} // namespace edgerelay
