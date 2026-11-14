#pragma once

#include <protocol.h>

#include "../debug/SerialLogger.h"
#include "../domain/TemperatureConverter.h"
#include "../drivers/AnalogSensor.h"
#include "../drivers/SpiPeripheral.h"
#include "../drivers/TemperatureDisplay.h"

/**
 * @brief Runs the acquisition cycle: refreshes the display often and publishes a sample to the SPI controller periodically.
 */
class TemperatureMonitor
{
public:
  /**
   * @brief Wires the monitor to its collaborators, which must outlive it.
   *
   * @param sensor Source of raw ADC readings.
   * @param converter Converts raw readings to degrees Celsius.
   * @param display Shows the latest reading.
   * @param spi Serves the latest sample to the ESP32-S3.
   * @param logger Receives readings, samples and initialization errors.
   * @param displayIntervalMs Time between display refreshes in milliseconds.
   * @param sampleIntervalMs Time between samples served over SPI in milliseconds.
   */
  TemperatureMonitor(AnalogSensor &sensor, TemperatureConverter &converter, TemperatureDisplay &display,
                     SpiPeripheral &spi, SerialLogger &logger, uint16_t displayIntervalMs, uint32_t sampleIntervalMs);

  /**
   * @brief Starts the logger, the display and the SPI peripheral, then publishes the first sample.
   *
   * Halts if the display does not answer.
   */
  void begin();

  /**
   * @brief Refreshes the display and publishes a sample when their intervals have elapsed; never blocks.
   */
  void update();

private:
  AnalogSensor &sensor;
  TemperatureConverter &converter;
  TemperatureDisplay &display;
  SpiPeripheral &spi;
  SerialLogger &logger;
  uint16_t displayIntervalMs;
  uint32_t sampleIntervalMs;
  uint32_t lastDisplayMs = 0;
  uint32_t lastSampleMs = 0;
  uint16_t sequence = 0;

  /**
   * @brief Reads the sensor and shows the reading on the display.
   */
  void refresh_display();

  /**
   * @brief Reads the sensor and makes a new sample available to the SPI controller.
   */
  void publish_sample();

  /**
   * @brief Builds a protocol frame with the next sequence number and a valid checksum.
   *
   * @param rawValue ADC value of the reading.
   * @param temperatureC Temperature of the reading in degrees Celsius.
   * @return Frame ready to be served over SPI.
   */
  SampleFrame build_sample(uint16_t rawValue, float temperatureC);

  /**
   * @brief Checks whether an interval has elapsed since a timestamp.
   *
   * @param sinceMs Timestamp from millis().
   * @param intervalMs Interval in milliseconds.
   * @return true if at least intervalMs passed since sinceMs.
   */
  static bool has_elapsed(uint32_t sinceMs, uint32_t intervalMs);

  /**
   * @brief Logs an error and stops execution.
   *
   * @param message Error text, wrapped with F().
   */
  void halt(const __FlashStringHelper *message);
};
