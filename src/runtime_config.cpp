#include "runtime_config.h"

#include "config_file.h"

#include <stdexcept>
#include <string>

namespace edgerelay {
namespace {

SinkMode parse_sink_mode(const std::string& value) {
    if (value == "console") {
        return SinkMode::Console;
    }
    if (value == "mqtt") {
        return SinkMode::Mqtt;
    }
    throw std::runtime_error("--sink must be console or mqtt");
}

} // namespace

RuntimeConfig parse_runtime_config(const int argc, char* argv[]) {
    RuntimeConfig config{};

    // First pass: only read --config so file values become runtime defaults.
    for (int i = 1; i < argc; ++i) {
        const std::string arg{argv[i]};
        if (arg == "--config" && i + 1 < argc) {
            load_config_file(argv[++i], config);
        }
    }

    // Second pass: explicit CLI arguments override config file values.
    for (int i = 1; i < argc; ++i) {
        const std::string arg{argv[i]};

        if (arg == "--config" && i + 1 < argc) {
            ++i;
        } else if (arg == "--sink" && i + 1 < argc) {
            config.sink_mode = parse_sink_mode(argv[++i]);
        } else if (arg == "--count" && i + 1 < argc) {
            config.count = static_cast<std::size_t>(std::stoull(argv[++i]));
        } else if (arg == "--mqtt-server" && i + 1 < argc) {
            config.mqtt_server_uri = argv[++i];
        } else if (arg == "--mqtt-client-id" && i + 1 < argc) {
            config.mqtt_client_id = argv[++i];
        } else if (arg == "--mqtt-topic" && i + 1 < argc) {
            config.mqtt_topic = argv[++i];
        } else {
            throw std::runtime_error("Unknown or incomplete argument: " + arg);
        }
    }

    return config;
}

} // namespace edgerelay
