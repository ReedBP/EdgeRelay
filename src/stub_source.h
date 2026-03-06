#pragma once

#include "app_message.h"

namespace edgerelay {

// Generates deterministic fake telemetry so we can validate the pipeline first.
class StubSource {
public:
    SensorMessage next();

private:
    std::uint64_t sequence_{0};
};

} // namespace edgerelay
