#include "mqtt_sink.h"

#include "app_message.h"

#include <stdexcept>

namespace edgerelay {

MqttSink::MqttSink(std::string server_uri, std::string client_id, std::string topic)
#ifdef EDGERELAY_ENABLE_MQTT
    : client_(std::move(server_uri), std::move(client_id)), topic_(std::move(topic)) {
    client_.connect()->wait();
}
#else
    : topic_(std::move(topic)) {
    (void)server_uri;
    (void)client_id;
    throw std::runtime_error(
        "MQTT support is disabled. Reconfigure with -DEDGERELAY_ENABLE_MQTT=ON and vcpkg toolchain.");
}
#endif

MqttSink::~MqttSink() {
#ifdef EDGERELAY_ENABLE_MQTT
    try {
        if (client_.is_connected()) {
            client_.disconnect()->wait();
        }
    } catch (...) {
        // Destructors must not throw; ignore shutdown errors.
    }
#endif
}

bool MqttSink::publish(const SensorMessage& message) {
#ifdef EDGERELAY_ENABLE_MQTT
    auto mqtt_message = mqtt::make_message(topic_, to_json(message));
    mqtt_message->set_qos(1);
    client_.publish(mqtt_message)->wait();
    return true;
#else
    (void)message;
    throw std::runtime_error("MQTT support is disabled at compile time.");
#endif
}

} // namespace edgerelay
