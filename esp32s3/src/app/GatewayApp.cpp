#include "GatewayApp.h"

#include "../domain/SamplePayload.h"

GatewayApp::GatewayApp(SpiController &spi, WifiConnection &wifi, MqttPublisher &mqtt, SerialLogger &logger,
                       const char *topic, uint32_t pollIntervalMs)
    : spi(spi), wifi(wifi), mqtt(mqtt), logger(logger), topic(topic), pollIntervalMs(pollIntervalMs)
{
}

void GatewayApp::begin()
{
  logger.begin();
  spi.begin();
  wifi.begin();
  mqtt.begin();
}

void GatewayApp::update()
{
  bool online = maintain_connections();
  if (millis() - lastPollMs >= pollIntervalMs)
    poll_peripheral();
  if (online && hasPendingSample)
    publish_pending_sample();
}

bool GatewayApp::maintain_connections()
{
  bool wifiConnected = wifi.is_connected();
  track_connection("wifi", wifiConnected, wifiWasConnected);
  bool mqttConnected = wifiConnected && mqtt.maintain();
  track_connection("mqtt", mqttConnected, mqttWasConnected);
  return mqttConnected;
}

void GatewayApp::poll_peripheral()
{
  lastPollMs = millis();
  SampleFrame sample;
  if (!spi.read_sample(sample))
  {
    logger.log_error("invalid SPI frame");
    return;
  }
  if (sample.sequence == lastReceivedSequence)
    return;
  lastReceivedSequence = sample.sequence;
  pendingSample = sample;
  hasPendingSample = true;
  logger.log_sample(sample);
}

void GatewayApp::publish_pending_sample()
{
  char payload[payloadSize];
  SamplePayload::format(pendingSample, payload, sizeof(payload));
  if (!mqtt.publish(topic, payload))
  {
    logger.log_error("MQTT publish failed");
    return;
  }
  hasPendingSample = false;
  logger.log_published(topic, payload);
}

void GatewayApp::track_connection(const char *service, bool connected, bool &wasConnected)
{
  if (connected == wasConnected)
    return;
  wasConnected = connected;
  logger.log_connection(service, connected);
}
