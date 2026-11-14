#include "SerialLogger.h"

SerialLogger::SerialLogger(uint32_t baudRate)
    : baudRate(baudRate)
{
}

void SerialLogger::begin()
{
  Serial.begin(baudRate);
}

void SerialLogger::log_connection(const char *service, bool connected)
{
  Serial.printf("%s %s\n", service, connected ? "connected" : "disconnected");
}

void SerialLogger::log_sample(const SampleFrame &sample)
{
  Serial.printf("sample seq=%u temperatureC=%.2f raw=%u\n", sample.sequence, sample.temperatureCentiC / 100.0f, sample.rawAdc);
}

void SerialLogger::log_published(const char *topic, const char *payload)
{
  Serial.printf("published %s %s\n", topic, payload);
}

void SerialLogger::log_error(const char *message)
{
  Serial.printf("error: %s\n", message);
}
