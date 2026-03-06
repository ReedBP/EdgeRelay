#include "app_message.h"

#include <sstream>

namespace edgerelay {

std::string to_json(const SensorMessage& message) {
    std::ostringstream os;
    os << "{"
       << "\"sequence\":" << message.sequence << ","
       << "\"source\":\"" << message.source_name << "\"," 
       << "\"timestamp_utc\":\"" << message.timestamp_utc << "\"," 
       << "\"value\":" << message.value
       << "}";
    return os.str();
}

} // namespace edgerelay
