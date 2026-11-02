#pragma once

#include <stdint.h>

/**
 * @brief Converts raw ADC values to temperature using a linear calibration range.
 */
class TemperatureConverter
{
public:
  /**
   * @brief Defines the calibration: 0 maps to temperatureAtAdcMinC and adcMaxValue to temperatureAtAdcMaxC.
   *
   * @param adcMaxValue Largest value the ADC can return.
   * @param temperatureAtAdcMinC Temperature in degrees Celsius at ADC value 0.
   * @param temperatureAtAdcMaxC Temperature in degrees Celsius at adcMaxValue.
   */
  TemperatureConverter(uint16_t adcMaxValue, float temperatureAtAdcMinC, float temperatureAtAdcMaxC);

  /**
   * @brief Converts a raw ADC value to temperature.
   *
   * @param rawValue ADC value, from 0 to adcMaxValue.
   * @return Temperature in degrees Celsius.
   */
  float to_celsius(uint16_t rawValue) const;

private:
  uint16_t adcMaxValue;
  float temperatureAtAdcMinC;
  float temperatureAtAdcMaxC;
};
