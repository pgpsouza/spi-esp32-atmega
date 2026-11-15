#pragma once

#include <Arduino.h>

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
   * @brief Logs a sample made available to the SPI controller.
   *
   * @param sequence Sequence number of the sample.
   * @param temperatureC Temperature of the sample in degrees Celsius.
   */
  void log_sample(uint16_t sequence, float temperatureC);

private:
  uint32_t baudRate;
};
