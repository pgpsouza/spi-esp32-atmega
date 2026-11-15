#include <Arduino.h>

#include "app/TemperatureMonitor.h"

constexpr uint8_t sensorPin = A0;
constexpr uint8_t samplesPerReading = 8;
constexpr uint32_t serialBaudRate = 115200;

// Placeholder calibration: the ADC range maps linearly to this range until the real sensor curve is defined.
constexpr float temperatureAtAdcMinC = 0.0f;
constexpr float temperatureAtAdcMaxC = 100.0f;

AnalogSensor sensor(sensorPin, samplesPerReading);
TemperatureConverter converter(AnalogSensor::adcMaxValue, temperatureAtAdcMinC, temperatureAtAdcMaxC);
SpiPeripheral spi;
SerialLogger logger(serialBaudRate);
TemperatureMonitor monitor(sensor, converter, spi, logger, samplePeriodMs);

/**
 * @brief SPI transfer-complete interrupt: forwards the received byte to the SPI peripheral.
 */
ISR(SPI_STC_vect)
{
  spi.handle_byte_received(SPDR);
}

/**
 * @brief Pin-change interrupt of port B: resets the SPI transaction when the controller releases SS (D10).
 */
ISR(PCINT0_vect)
{
  if (PINB & _BV(PINB2))
    spi.handle_deselect();
}

/**
 * @brief Arduino entry point: starts the temperature monitor.
 */
void setup()
{
  monitor.begin();
}

/**
 * @brief Arduino main loop: gives the monitor a chance to publish a sample.
 */
void loop()
{
  monitor.update();
}
