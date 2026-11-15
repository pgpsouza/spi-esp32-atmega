#include "SerialLogger.h"

SerialLogger::SerialLogger(uint32_t baudRate)
    : baudRate(baudRate)
{
}

void SerialLogger::begin()
{
  Serial.begin(baudRate);
}

void SerialLogger::log_sample(uint16_t sequence, float temperatureC)
{
  Serial.print(F("sample seq="));
  Serial.print(sequence);
  Serial.print(F(" temperatureC="));
  Serial.println(temperatureC, 2);
}
