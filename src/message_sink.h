#pragma once

#include "app_message.h"

namespace edgerelay {

class MessageSink {
public:
    virtual ~MessageSink() = default;
    virtual bool publish(const SensorMessage& message) = 0;
};

} // namespace edgerelay
