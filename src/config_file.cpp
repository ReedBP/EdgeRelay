#include "config_file.h"

#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>

namespace edgerelay {
namespace {

std::string trim(std::string value) {
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front())) != 0) {
        value.erase(value.begin());
    }
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back())) != 0) {
        value.pop_back();
    }
    return value;
}

std::string unquote(std::string value) {
    value = trim(std::move(value));
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        return value.substr(1, value.size() - 2);
    }
    return value;
}

SinkMode parse_sink_mode_value(const std::string& value) {
    if (value == "console") {
        return SinkMode::Console;
    }
    if (value == "mqtt") {
        return SinkMode::Mqtt;
    }
    throw std::runtime_error("[app].sink must be \"console\" or \"mqtt\"");
}

} // namespace

void load_config_file(const std::string& path, RuntimeConfig& config) {
    std::ifstream input(path);
    if (!input.is_open()) {
        throw std::runtime_error("Failed to open config file: " + path);
    }

    std::string section;
    std::string line;
    std::size_t line_number = 0;

    while (std::getline(input, line)) {
        ++line_number;

        const auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        line = trim(std::move(line));
        if (line.empty()) {
            continue;
        }

        if (line.front() == '[' && line.back() == ']') {
            section = trim(line.substr(1, line.size() - 2));
            continue;
        }

        const auto equals_pos = line.find('=');
        if (equals_pos == std::string::npos) {
            throw std::runtime_error("Invalid config line " + std::to_string(line_number) + ": " + line);
        }

        const auto key = trim(line.substr(0, equals_pos));
        const auto value = unquote(line.substr(equals_pos + 1));

        if (section == "app" && key == "sink") {
            config.sink_mode = parse_sink_mode_value(value);
        } else if (section == "app" && key == "count") {
            config.count = static_cast<std::size_t>(std::stoull(value));
        } else if (section == "mqtt" && key == "server_uri") {
            config.mqtt_server_uri = value;
        } else if (section == "mqtt" && key == "client_id") {
            config.mqtt_client_id = value;
        } else if (section == "mqtt" && key == "topic") {
            config.mqtt_topic = value;
        } else {
            throw std::runtime_error(
                "Unknown config key at line " + std::to_string(line_number) + ": [" + section + "]." + key);
        }
    }
}

} // namespace edgerelay
