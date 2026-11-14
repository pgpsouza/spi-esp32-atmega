#pragma once

#include <Arduino.h>
#include <protocol.h>

/**
 * @brief Writes debug messages to the serial port.
 */
class SerialLogger
{
public:
  /**
   * @brief Configures the serial baud rate.
   *
   * @param baudRate Serial speed in bits per second.
   */
  explicit SerialLogger(uint32_t baudRate);

  /**
   * @brief Opens the serial port.
   */
  void begin();

  /**
   * @brief Logs a change in the Wi-Fi or MQTT connection.
   *
   * @param service Name of the connection, e.g. "wifi" or "mqtt".
   * @param connected New state of the connection.
   */
  void log_connection(const char *service, bool connected);

  /**
   * @brief Logs a new sample received from the ATmega328P.
   *
   * @param sample Valid frame received over SPI.
   */
  void log_sample(const SampleFrame &sample);

  /**
   * @brief Logs a message published to the broker.
   *
   * @param topic Topic the message was published to.
   * @param payload Message text.
   */
  void log_published(const char *topic, const char *payload);

  /**
   * @brief Logs an error message.
   *
   * @param message Error text.
   */
  void log_error(const char *message);

private:
  uint32_t baudRate;
};
