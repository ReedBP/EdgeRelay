#pragma once

#include <cstdint>
#include <string>

namespace edgerelay {

// A normalized message format used across source->transport pipeline.
struct SensorMessage {
    std::uint64_t sequence{};
    std::string source_name;
    std::string timestamp_utc;
    double value{};
};

std::string to_json(const SensorMessage& message);

} // namespace edgerelay
