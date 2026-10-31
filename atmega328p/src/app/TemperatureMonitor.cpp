#include "TemperatureMonitor.h"

TemperatureMonitor::TemperatureMonitor(AnalogSensor &sensor, TemperatureConverter &converter,
                                       TemperatureDisplay &display, SerialLogger &logger, uint16_t readIntervalMs)
    : sensor(sensor), converter(converter), display(display), logger(logger), readIntervalMs(readIntervalMs)
{
}

void TemperatureMonitor::begin()
{
  logger.begin();
  if (!display.begin())
  {
    halt(F("SSD1306 not found"));
  }
}

void TemperatureMonitor::update()
{
  if (millis() - lastReadMs < readIntervalMs)
    return;
  lastReadMs = millis();
  process_reading();
}

void TemperatureMonitor::process_reading()
{
  uint16_t rawValue = sensor.read_raw();
  float temperatureC = converter.to_celsius(rawValue);
  display.show(temperatureC, rawValue);
  logger.log_reading(temperatureC, rawValue);
}

void TemperatureMonitor::halt(const __FlashStringHelper *message)
{
  logger.log_error(message);
  while (true)
  {
  }
}
