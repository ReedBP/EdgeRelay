#include "console_sink.h"

#include "app_message.h"

#include <iostream>

namespace edgerelay {

bool ConsoleSink::publish(const SensorMessage& message) {
    std::cout << "[Phase1-PR1] topic=edge/stub payload=" << to_json(message) << '\n';
    return true;
}

} // namespace edgerelay
