#include "TemperatureConverter.h"

TemperatureConverter::TemperatureConverter(uint16_t adcMaxValue, float temperatureAtAdcMinC, float temperatureAtAdcMaxC)
    : adcMaxValue(adcMaxValue), temperatureAtAdcMinC(temperatureAtAdcMinC), temperatureAtAdcMaxC(temperatureAtAdcMaxC)
{
}

float TemperatureConverter::to_celsius(uint16_t rawValue) const
{
  return temperatureAtAdcMinC + (temperatureAtAdcMaxC - temperatureAtAdcMinC) * rawValue / adcMaxValue;
}
