#include "GatewayApp.h"

#include "../domain/SamplePayload.h"

GatewayApp::GatewayApp(SpiController &spi, WifiConnection &wifi, MqttPublisher &mqtt, TemperatureDisplay &display,
                       SerialLogger &logger, const char *topic, const GatewaySettings &settings)
    : spi(spi), wifi(wifi), mqtt(mqtt), display(display), logger(logger), topic(topic), settings(settings)
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
  if (millis() - lastSamplePollMs >= settings.samplePollIntervalMs)
    poll_sample();
  if (millis() - lastLivePollMs >= settings.livePollIntervalMs)
    poll_live();
  if (online && hasPendingSample)
    publish_pending_sample();
  if (millis() - lastDisplayMs >= settings.displayIntervalMs)
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

void GatewayApp::poll_sample()
{
  lastSamplePollMs = millis();
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

void GatewayApp::poll_live()
{
  lastLivePollMs = millis();
  SampleFrame reading;
  if (!spi.read_live(reading))
    return;
  if (hasShownTemperature && !differs_from_shown(reading.temperatureCentiC))
    return;
  shownTemperatureCentiC = reading.temperatureCentiC;
  hasShownTemperature = true;
  refresh_display();
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
  if (!hasShownTemperature)
  {
    display.show_waiting();
    return;
  }
  display.show_temperature(shownTemperatureCentiC / 100.0f, seconds_to_next_sample());
}

bool GatewayApp::differs_from_shown(int16_t temperatureCentiC) const
{
  int32_t difference = static_cast<int32_t>(temperatureCentiC) - shownTemperatureCentiC;
  return abs(difference) >= settings.displayMarginCentiC;
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
