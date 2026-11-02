#pragma once

#include "../debug/SerialLogger.h"
#include "../domain/TemperatureConverter.h"
#include "../drivers/AnalogSensor.h"
#include "../drivers/TemperatureDisplay.h"

/**
 * @brief Runs the acquisition cycle: read the sensor, convert to temperature, display and log.
 */
class TemperatureMonitor
{
public:
  /**
   * @brief Wires the monitor to its collaborators, which must outlive it.
   *
   * @param sensor Source of raw ADC readings.
   * @param converter Converts raw readings to degrees Celsius.
   * @param display Shows each reading.
   * @param logger Receives each reading and initialization errors.
   * @param readIntervalMs Time between readings in milliseconds.
   */
  TemperatureMonitor(AnalogSensor &sensor, TemperatureConverter &converter,
                     TemperatureDisplay &display, SerialLogger &logger, uint16_t readIntervalMs);

  /**
   * @brief Starts the logger and the display, halting if the display does not answer.
   */
  void begin();

  /**
   * @brief Processes a reading once readIntervalMs has elapsed; returns immediately otherwise.
   */
  void update();

private:
  AnalogSensor &sensor;
  TemperatureConverter &converter;
  TemperatureDisplay &display;
  SerialLogger &logger;
  uint16_t readIntervalMs;
  uint32_t lastReadMs = 0;

  /**
   * @brief Reads the sensor, converts the value, then displays and logs it.
   */
  void process_reading();

  /**
   * @brief Logs an error and stops execution.
   *
   * @param message Error text, wrapped with F().
   */
  void halt(const __FlashStringHelper *message);
};
