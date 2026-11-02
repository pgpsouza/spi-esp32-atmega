#include "SerialLogger.h"

SerialLogger::SerialLogger(uint32_t baudRate)
    : baudRate(baudRate)
{
}

void SerialLogger::begin()
{
  Serial.begin(baudRate);
}

void SerialLogger::log_reading(float temperatureC, uint16_t rawValue)
{
  Serial.print(F("raw="));
  Serial.print(rawValue);
  Serial.print(F(" temperatureC="));
  Serial.println(temperatureC, 1);
}

void SerialLogger::log_error(const __FlashStringHelper *message)
{
  Serial.print(F("error: "));
  Serial.println(message);
}
