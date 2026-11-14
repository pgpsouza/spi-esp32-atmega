#pragma once

#include <protocol.h>

#include "../debug/SerialLogger.h"
#include "../drivers/SpiController.h"
#include "../net/MqttPublisher.h"
#include "../net/WifiConnection.h"

/**
 * @brief Bridges the ATmega328P to the MQTT broker: polls samples over SPI and publishes each new one.
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
   * @param logger Receives connection changes, samples, publications and errors.
   * @param topic MQTT topic samples are published to.
   * @param pollIntervalMs Time between SPI reads in milliseconds.
   */
  GatewayApp(SpiController &spi, WifiConnection &wifi, MqttPublisher &mqtt, SerialLogger &logger,
             const char *topic, uint32_t pollIntervalMs);

  /**
   * @brief Starts the logger, the SPI bus, the Wi-Fi connection and the MQTT client.
   */
  void begin();

  /**
   * @brief Keeps the connections alive, polls the peripheral when due and publishes a pending sample; never blocks for long.
   */
  void update();

private:
  static constexpr size_t payloadSize = 64;

  SpiController &spi;
  WifiConnection &wifi;
  MqttPublisher &mqtt;
  SerialLogger &logger;
  const char *topic;
  uint32_t pollIntervalMs;
  uint32_t lastPollMs = 0;
  uint16_t lastReceivedSequence = 0;
  SampleFrame pendingSample = {};
  bool hasPendingSample = false;
  bool wifiWasConnected = false;
  bool mqttWasConnected = false;

  /**
   * @brief Services Wi-Fi and MQTT and logs their state changes.
   *
   * @return true if the broker is reachable.
   */
  bool maintain_connections();

  /**
   * @brief Reads a sample over SPI and keeps it as pending when its sequence number is new.
   */
  void poll_peripheral();

  /**
   * @brief Publishes the pending sample; keeps it pending if the publication fails.
   */
  void publish_pending_sample();

  /**
   * @brief Logs a connection change, remembering the last state seen.
   *
   * @param service Name of the connection.
   * @param connected Current state.
   * @param wasConnected Last state seen; updated in place.
   */
  void track_connection(const char *service, bool connected, bool &wasConnected);
};
