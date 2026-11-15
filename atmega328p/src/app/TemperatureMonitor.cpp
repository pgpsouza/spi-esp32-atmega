#include "TemperatureMonitor.h"

TemperatureMonitor::TemperatureMonitor(AnalogSensor &sensor, TemperatureConverter &converter, SpiPeripheral &spi,
                                       SerialLogger &logger, uint32_t sampleIntervalMs)
    : sensor(sensor), converter(converter), spi(spi), logger(logger), sampleIntervalMs(sampleIntervalMs)
{
}

void TemperatureMonitor::begin()
{
  logger.begin();
  spi.begin();
  publish_sample();
}

void TemperatureMonitor::update()
{
  if (millis() - lastSampleMs >= sampleIntervalMs)
    publish_sample();
}

void TemperatureMonitor::publish_sample()
{
  lastSampleMs = millis();
  uint16_t rawValue = sensor.read_raw();
  float temperatureC = converter.to_celsius(rawValue);
  SampleFrame sample = build_sample(rawValue, temperatureC);
  spi.set_sample(sample);
  logger.log_sample(sample.sequence, temperatureC);
}

SampleFrame TemperatureMonitor::build_sample(uint16_t rawValue, float temperatureC)
{
  sequence = sequence == UINT16_MAX ? 1 : sequence + 1;
  SampleFrame sample = {};
  sample.version = protocolVersion;
  sample.sequence = sequence;
  sample.temperatureCentiC = static_cast<int16_t>(lroundf(temperatureC * 100.0f));
  sample.rawAdc = rawValue;
  sample.checksum = protocol_checksum(sample);
  return sample;
}
