#include "GatewayApp.h"

#include "../domain/SamplePayload.h"

GatewayApp::GatewayApp(SpiController &spi, WifiConnection &wifi, MqttPublisher &mqtt, TemperatureDisplay &display,
                       SerialLogger &logger, const char *topic, uint32_t pollIntervalMs, uint32_t displayIntervalMs)
    : spi(spi), wifi(wifi), mqtt(mqtt), display(display), logger(logger), topic(topic),
      pollIntervalMs(pollIntervalMs), displayIntervalMs(displayIntervalMs)
{
}

void GatewayApp::begin()
{
  logger.begin();
  spi.begin();
  displayReady = display.begin();
  if (!displayReady)
    logger.log_error("SSD1306 not found, running without display");
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
  if (millis() - lastDisplayMs >= displayIntervalMs)
    refresh_display();
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
  lastSampleReceivedMs = millis();
  latestSample = sample;
  hasPendingSample = true;
  logger.log_sample(sample);
}

void GatewayApp::publish_pending_sample()
{
  char payload[payloadSize];
  SamplePayload::format(latestSample, payload, sizeof(payload));
  if (!mqtt.publish(topic, payload))
  {
    logger.log_error("MQTT publish failed");
    return;
  }
  hasPendingSample = false;
  logger.log_published(topic, payload);
}

void GatewayApp::refresh_display()
{
  lastDisplayMs = millis();
  if (!displayReady)
    return;
  if (lastReceivedSequence == 0)
  {
    display.show_waiting();
    return;
  }
  display.show_temperature(latestSample.temperatureCentiC / 100.0f, seconds_to_next_sample());
}

uint8_t GatewayApp::seconds_to_next_sample() const
{
  uint32_t elapsedMs = millis() - lastSampleReceivedMs;
  return elapsedMs >= samplePeriodMs ? 0 : (samplePeriodMs - elapsedMs + 999) / 1000;
}

void GatewayApp::track_connection(const char *service, bool connected, bool &wasConnected)
{
  if (connected == wasConnected)
    return;
  wasConnected = connected;
  logger.log_connection(service, connected);
}
