#include "TemperatureMonitor.h"

TemperatureMonitor::TemperatureMonitor(AnalogSensor &sensor, TemperatureConverter &converter, TemperatureDisplay &display,
                                       SpiPeripheral &spi, SerialLogger &logger, uint16_t displayIntervalMs, uint32_t sampleIntervalMs)
    : sensor(sensor), converter(converter), display(display), spi(spi), logger(logger),
      displayIntervalMs(displayIntervalMs), sampleIntervalMs(sampleIntervalMs)
{
}

void TemperatureMonitor::begin()
{
  logger.begin();
  if (!display.begin())
  {
    halt(F("SSD1306 not found"));
  }
  spi.begin();
  publish_sample();
}

void TemperatureMonitor::update()
{
  if (has_elapsed(lastDisplayMs, displayIntervalMs))
    refresh_display();
  if (has_elapsed(lastSampleMs, sampleIntervalMs))
    publish_sample();
}

void TemperatureMonitor::refresh_display()
{
  lastDisplayMs = millis();
  uint16_t rawValue = sensor.read_raw();
  float temperatureC = converter.to_celsius(rawValue);
  display.show(temperatureC, rawValue);
  logger.log_reading(temperatureC, rawValue);
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

bool TemperatureMonitor::has_elapsed(uint32_t sinceMs, uint32_t intervalMs)
{
  return millis() - sinceMs >= intervalMs;
}

void TemperatureMonitor::halt(const __FlashStringHelper *message)
{
  logger.log_error(message);
  while (true)
  {
  }
}
