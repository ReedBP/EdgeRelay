#pragma once

#include "message_sink.h"

#ifdef EDGERELAY_ENABLE_MQTT
#include <mqtt/async_client.h>
#endif

#include <string>

namespace edgerelay {

class MqttSink final : public MessageSink {
  public:
    MqttSink(std::string server_uri, std::string client_id, std::string topic);
    ~MqttSink() override;

    bool publish(const SensorMessage& message) override;

  private:
#ifdef EDGERELAY_ENABLE_MQTT
    mqtt::async_client client_;
#endif
    std::string topic_;
};

} // namespace edgerelay
