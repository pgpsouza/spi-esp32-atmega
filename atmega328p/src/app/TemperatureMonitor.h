#pragma once

#include <protocol.h>

#include "../debug/SerialLogger.h"
#include "../domain/TemperatureConverter.h"
#include "../drivers/AnalogSensor.h"
#include "../drivers/SpiPeripheral.h"

/**
 * @brief Runs the acquisition cycle: keeps a live reading fresh and publishes an official sample to the SPI controller periodically.
 */
class TemperatureMonitor
{
public:
  /**
   * @brief Wires the monitor to its collaborators, which must outlive it.
   *
   * @param sensor Source of raw ADC readings.
   * @param converter Converts raw readings to degrees Celsius.
   * @param spi Serves the official sample and the live reading to the ESP32-S3.
   * @param logger Receives each official sample.
   * @param sampleIntervalMs Time between official samples in milliseconds.
   * @param liveIntervalMs Time between live readings in milliseconds.
   */
  TemperatureMonitor(AnalogSensor &sensor, TemperatureConverter &converter, SpiPeripheral &spi,
                     SerialLogger &logger, uint32_t sampleIntervalMs, uint32_t liveIntervalMs);

  /**
   * @brief Starts the logger and the SPI peripheral, then publishes the first live reading and official sample.
   */
  void begin();

  /**
   * @brief Refreshes the live reading and publishes an official sample when their intervals have elapsed; never blocks.
   */
  void update();

private:
  AnalogSensor &sensor;
  TemperatureConverter &converter;
  SpiPeripheral &spi;
  SerialLogger &logger;
  uint32_t sampleIntervalMs;
  uint32_t liveIntervalMs;
  uint32_t lastSampleMs = 0;
  uint32_t lastLiveMs = 0;
  uint16_t sequence = 0;

  /**
   * @brief Reads the sensor and makes it available as the live reading, with sequence 0.
   */
  void publish_live_reading();

  /**
   * @brief Reads the sensor and makes it available as a new official sample with the next sequence number.
   */
  void publish_sample();

  /**
   * @brief Advances the sequence number, skipping 0, which marks live readings.
   *
   * @return The new sequence number.
   */
  uint16_t next_sequence();

  /**
   * @brief Builds a protocol frame with a valid checksum.
   *
   * @param frameSequence Sequence number of the frame (0 for a live reading).
   * @param rawValue ADC value of the reading.
   * @param temperatureC Temperature of the reading in degrees Celsius.
   * @return Frame ready to be served over SPI.
   */
  static SampleFrame build_frame(uint16_t frameSequence, uint16_t rawValue, float temperatureC);
};
