#include <Arduino.h>

#include "app/TemperatureMonitor.h"

constexpr uint8_t sensorPin = A0;
constexpr uint8_t samplesPerReading = 8;
constexpr uint8_t screenWidth = 128;
constexpr uint8_t screenHeight = 64;
constexpr uint8_t oledAddress = 0x3C;
constexpr uint16_t readIntervalMs = 500;
constexpr uint32_t serialBaudRate = 115200;

// Placeholder calibration: the ADC range maps linearly to this range until the real sensor curve is defined.
constexpr float temperatureAtAdcMinC = 0.0f;
constexpr float temperatureAtAdcMaxC = 100.0f;

AnalogSensor sensor(sensorPin, samplesPerReading);
TemperatureConverter converter(AnalogSensor::adcMaxValue, temperatureAtAdcMinC, temperatureAtAdcMaxC);
TemperatureDisplay display(screenWidth, screenHeight, oledAddress);
SerialLogger logger(serialBaudRate);
TemperatureMonitor monitor(sensor, converter, display, logger, readIntervalMs);

/**
 * @brief Arduino entry point: starts the temperature monitor.
 */
void setup()
{
  monitor.begin();
}

/**
 * @brief Arduino main loop: gives the monitor a chance to process a reading.
 */
void loop()
{
  monitor.update();
}
