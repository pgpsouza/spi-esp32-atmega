#pragma once

#include <protocol.h>

#include "../debug/SerialLogger.h"
#include "../drivers/SpiController.h"
#include "../drivers/TemperatureDisplay.h"
#include "../net/MqttPublisher.h"
#include "../net/WifiConnection.h"

/**
 * @brief Bridges the ATmega328P to the MQTT broker: polls samples over SPI, publishes each new one and shows the latest temperature.
 */
class GatewayApp
{
public:
  /**
   * @brief Wires the gateway to its collaborators, which must outlive it.
   *
   * @param spi Reads samples from the ATmega328P.
   * @param wifi Network connection.
   * @param mqtt Broker connection.
   * @param display Shows the latest temperature.
   * @param logger Receives connection changes, samples, publications and errors.
   * @param topic MQTT topic samples are published to.
   * @param pollIntervalMs Time between SPI reads in milliseconds.
   * @param displayIntervalMs Time between display refreshes in milliseconds.
   */
  GatewayApp(SpiController &spi, WifiConnection &wifi, MqttPublisher &mqtt, TemperatureDisplay &display,
             SerialLogger &logger, const char *topic, uint32_t pollIntervalMs, uint32_t displayIntervalMs);

  /**
   * @brief Starts the logger, the SPI bus, the display, the Wi-Fi connection and the MQTT client.
   *
   * A display that does not answer is logged and skipped; the gateway keeps running without it.
   */
  void begin();

  /**
   * @brief Keeps the connections alive, polls the peripheral, publishes a pending sample and refreshes the display when due.
   */
  void update();

private:
  static constexpr size_t payloadSize = 64;

  SpiController &spi;
  WifiConnection &wifi;
  MqttPublisher &mqtt;
  TemperatureDisplay &display;
  SerialLogger &logger;
  const char *topic;
  uint32_t pollIntervalMs;
  uint32_t displayIntervalMs;
  uint32_t lastPollMs = 0;
  uint32_t lastDisplayMs = 0;
  uint32_t lastSampleReceivedMs = 0;
  uint16_t lastReceivedSequence = 0;
  SampleFrame latestSample = {};
  bool hasPendingSample = false;
  bool displayReady = false;
  bool wifiWasConnected = false;
  bool mqttWasConnected = false;

  /**
   * @brief Services Wi-Fi and MQTT and logs their state changes.
   *
   * @return true if the broker is reachable.
   */
  bool maintain_connections();

  /**
   * @brief Reads a sample over SPI and keeps it as the latest, pending publication, when its sequence number is new.
   */
  void poll_peripheral();

  /**
   * @brief Publishes the latest sample; keeps it pending if the publication fails.
   */
  void publish_pending_sample();

  /**
   * @brief Shows the latest temperature, or a placeholder before the first valid sample.
   */
  void refresh_display();

  /**
   * @brief Estimates the time left until the ATmega328P makes its next sample available.
   *
   * The estimate can run up to pollIntervalMs late, because a new sample is only noticed on the next poll.
   *
   * @return Seconds left, rounded up and never negative.
   */
  uint8_t seconds_to_next_sample() const;

  /**
   * @brief Logs a connection change, remembering the last state seen.
   *
   * @param service Name of the connection.
   * @param connected Current state.
   * @param wasConnected Last state seen; updated in place.
   */
  void track_connection(const char *service, bool connected, bool &wasConnected);
};
