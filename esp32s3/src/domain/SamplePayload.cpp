#include "SamplePayload.h"

#include <stdio.h>

void SamplePayload::format(const SampleFrame &sample, char *buffer, size_t size)
{
  float temperatureC = sample.temperatureCentiC / 100.0f;
  snprintf(buffer, size, "{\"sequence\":%u,\"temperature\":%.2f,\"raw\":%u}", sample.sequence, temperatureC, sample.rawAdc);
}
