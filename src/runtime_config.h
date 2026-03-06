#pragma once

#include <cstddef>
#include <string>

namespace edgerelay {

enum class SinkMode {
    Console,
    Mqtt,
};

struct RuntimeConfig {
    SinkMode sink_mode{SinkMode::Console};
    std::size_t count{5};
    std::string mqtt_server_uri{"tcp://localhost:1883"};
    std::string mqtt_client_id{"edge-relay-dev"};
    std::string mqtt_topic{"edge/stub"};
};

RuntimeConfig parse_runtime_config(int argc, char* argv[]);

} // namespace edgerelay
