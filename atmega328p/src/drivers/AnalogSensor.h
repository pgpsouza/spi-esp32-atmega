#pragma once

#include <Arduino.h>

/**
 * @brief Reads an analog input of the MCU, averaging samples to reduce noise.
 */
class AnalogSensor
{
public:
  static constexpr uint16_t adcMaxValue = 1023;

  /**
   * @brief Binds the sensor to an analog pin.
   *
   * @param pin Analog pin the sensor output is connected to.
   * @param samplesPerReading Number of ADC samples averaged in each reading.
   */
  AnalogSensor(uint8_t pin, uint8_t samplesPerReading);

  /**
   * @brief Reads the sensor, averaging samplesPerReading ADC samples.
   *
   * @return Averaged ADC value, from 0 to adcMaxValue.
   */
  uint16_t read_raw() const;

private:
  uint8_t pin;
  uint8_t samplesPerReading;
};
