#include "TemperatureMonitor.h"

TemperatureMonitor::TemperatureMonitor(AnalogSensor &sensor, TemperatureConverter &converter, SpiPeripheral &spi,
                                       SerialLogger &logger, uint32_t sampleIntervalMs, uint32_t liveIntervalMs)
    : sensor(sensor), converter(converter), spi(spi), logger(logger),
      sampleIntervalMs(sampleIntervalMs), liveIntervalMs(liveIntervalMs)
{
}

void TemperatureMonitor::begin()
{
  logger.begin();
  spi.begin();
  publish_live_reading();
  publish_sample();
}

void TemperatureMonitor::update()
{
  if (millis() - lastLiveMs >= liveIntervalMs)
    publish_live_reading();
  if (millis() - lastSampleMs >= sampleIntervalMs)
    publish_sample();
}

void TemperatureMonitor::publish_live_reading()
{
  lastLiveMs = millis();
  uint16_t rawValue = sensor.read_raw();
  float temperatureC = converter.to_celsius(rawValue);
  spi.set_live_reading(build_frame(0, rawValue, temperatureC));
}

void TemperatureMonitor::publish_sample()
{
  lastSampleMs = millis();
  uint16_t rawValue = sensor.read_raw();
  float temperatureC = converter.to_celsius(rawValue);
  SampleFrame sample = build_frame(next_sequence(), rawValue, temperatureC);
  spi.set_sample(sample);
  logger.log_sample(sample.sequence, temperatureC);
}

uint16_t TemperatureMonitor::next_sequence()
{
  sequence = sequence == UINT16_MAX ? 1 : sequence + 1;
  return sequence;
}

SampleFrame TemperatureMonitor::build_frame(uint16_t frameSequence, uint16_t rawValue, float temperatureC)
{
  SampleFrame frame = {};
  frame.version = protocolVersion;
  frame.sequence = frameSequence;
  frame.temperatureCentiC = static_cast<int16_t>(lroundf(temperatureC * 100.0f));
  frame.rawAdc = rawValue;
  frame.checksum = protocol_checksum(frame);
  return frame;
}
