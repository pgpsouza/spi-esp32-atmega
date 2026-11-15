#pragma once

#include <protocol.h>

#include "../debug/SerialLogger.h"
#include "../domain/TemperatureConverter.h"
#include "../drivers/AnalogSensor.h"
#include "../drivers/SpiPeripheral.h"

/**
 * @brief Runs the acquisition cycle: reads the sensor and publishes a sample to the SPI controller periodically.
 */
class TemperatureMonitor
{
public:
  /**
   * @brief Wires the monitor to its collaborators, which must outlive it.
   *
   * @param sensor Source of raw ADC readings.
   * @param converter Converts raw readings to degrees Celsius.
   * @param spi Serves the latest sample to the ESP32-S3.
   * @param logger Receives each published sample.
   * @param sampleIntervalMs Time between samples served over SPI in milliseconds.
   */
  TemperatureMonitor(AnalogSensor &sensor, TemperatureConverter &converter, SpiPeripheral &spi,
                     SerialLogger &logger, uint32_t sampleIntervalMs);

  /**
   * @brief Starts the logger and the SPI peripheral, then publishes the first sample.
   */
  void begin();

  /**
   * @brief Publishes a sample when sampleIntervalMs has elapsed; never blocks.
   */
  void update();

private:
  AnalogSensor &sensor;
  TemperatureConverter &converter;
  SpiPeripheral &spi;
  SerialLogger &logger;
  uint32_t sampleIntervalMs;
  uint32_t lastSampleMs = 0;
  uint16_t sequence = 0;

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
};
