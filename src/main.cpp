#include "console_sink.h"
#include "mqtt_sink.h"
#include "pipeline.h"
#include "runtime_config.h"

#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
    try {
        const auto config = edgerelay::parse_runtime_config(argc, argv);

        std::unique_ptr<edgerelay::MessageSink> sink;
        if (config.sink_mode == edgerelay::SinkMode::Mqtt) {
            sink = std::make_unique<edgerelay::MqttSink>(
                config.mqtt_server_uri,
                config.mqtt_client_id,
                config.mqtt_topic);
            std::cout << "[EdgeRelay] sink=mqtt topic=" << config.mqtt_topic << '\n';
        } else {
            sink = std::make_unique<edgerelay::ConsoleSink>();
            std::cout << "[EdgeRelay] sink=console\n";
        }

        edgerelay::run_pipeline(config.count, *sink);
        std::cout << "[EdgeRelay] done\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "[EdgeRelay] startup error: " << ex.what() << '\n';
        return 1;
    }
}
