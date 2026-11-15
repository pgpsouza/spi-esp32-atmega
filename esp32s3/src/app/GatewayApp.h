#pragma once

#include <protocol.h>

#include "../debug/SerialLogger.h"
#include "../drivers/SpiController.h"
#include "../drivers/TemperatureDisplay.h"
#include "../net/MqttPublisher.h"
#include "../net/WifiConnection.h"

/**
 * @brief Timing and display settings of the gateway.
 */
struct GatewaySettings
{
  uint32_t samplePollIntervalMs;
  uint32_t livePollIntervalMs;
  uint32_t displayIntervalMs;
  int16_t displayMarginCentiC;
};

/**
 * @brief Bridges the ATmega328P to the MQTT broker: publishes each new official sample and shows the live temperature.
 */
class GatewayApp
{
public:
  /**
   * @brief Wires the gateway to its collaborators, which must outlive it.
   *
   * @param spi Reads samples and live readings from the ATmega328P.
   * @param wifi Network connection.
   * @param mqtt Broker connection.
   * @param display Shows the live temperature and the time to the next sample.
   * @param logger Receives connection changes, samples, publications and errors.
   * @param topic MQTT topic samples are published to.
   * @param settings Polling intervals, display refresh interval and display change margin.
   */
  GatewayApp(SpiController &spi, WifiConnection &wifi, MqttPublisher &mqtt, TemperatureDisplay &display,
             SerialLogger &logger, const char *topic, const GatewaySettings &settings);

  /**
   * @brief Starts the logger, the SPI bus, the display, the Wi-Fi connection and the MQTT client.
   *
   * A display that does not answer is logged and skipped; the gateway keeps running without it.
   */
  void begin();

  /**
   * @brief Keeps the connections alive, polls samples and live readings, publishes a pending sample and refreshes the display when due.
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
  GatewaySettings settings;
  uint32_t lastSamplePollMs = 0;
  uint32_t lastLivePollMs = 0;
  uint32_t lastDisplayMs = 0;
  uint32_t lastSampleReceivedMs = 0;
  uint16_t lastReceivedSequence = 0;
  SampleFrame latestSample = {};
  int16_t shownTemperatureCentiC = 0;
  bool hasShownTemperature = false;
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
   * @brief Reads the official sample over SPI and keeps it pending publication when its sequence number is new.
   */
  void poll_sample();

  /**
   * @brief Reads the live temperature over SPI and redraws the display when it moved at least displayMarginCentiC.
   *
   * Invalid live frames are skipped silently; poll_sample already logs SPI failures.
   */
  void poll_live();

  /**
   * @brief Publishes the latest sample; keeps it pending if the publication fails.
   */
  void publish_pending_sample();

  /**
   * @brief Shows the live temperature and the time to the next sample, or a placeholder before the first reading.
   */
  void refresh_display();

  /**
   * @brief Checks whether a temperature differs enough from the one on screen to redraw it.
   *
   * @param temperatureCentiC Temperature in hundredths of a degree Celsius.
   * @return true if the difference is at least displayMarginCentiC.
   */
  bool differs_from_shown(int16_t temperatureCentiC) const;

  /**
   * @brief Estimates the time left until the ATmega328P makes its next sample available.
   *
   * The estimate can run up to samplePollIntervalMs late, because a new sample is only noticed on the next poll.
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
