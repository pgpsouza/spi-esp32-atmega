#include "AnalogSensor.h"

AnalogSensor::AnalogSensor(uint8_t pin, uint8_t samplesPerReading)
    : pin(pin), samplesPerReading(samplesPerReading)
{
}

uint16_t AnalogSensor::read_raw() const
{
  uint32_t sum = 0;
  for (uint8_t i = 0; i < samplesPerReading; i++)
  {
    sum += analogRead(pin);
  }
  return sum / samplesPerReading;
}
