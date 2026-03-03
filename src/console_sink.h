#pragma once

#include "message_sink.h"

namespace edgerelay {

// Temporary sink used before real MQTT client integration.
class ConsoleSink final : public MessageSink {
public:
    bool publish(const SensorMessage& message) override;
};

} // namespace edgerelay
