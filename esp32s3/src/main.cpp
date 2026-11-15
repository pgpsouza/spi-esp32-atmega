#include <Arduino.h>
#include <WiFi.h>

#include "app/GatewayApp.h"
#include "secrets.h"

constexpr uint8_t spiSckPin = 12;
constexpr uint8_t spiMisoPin = 13;
constexpr uint8_t spiMosiPin = 11;
constexpr uint8_t spiCsPin = 10;
constexpr uint8_t i2cSdaPin = 8;
constexpr uint8_t i2cSclPin = 9;
constexpr uint8_t screenWidth = 128;
constexpr uint8_t screenHeight = 64;
constexpr uint8_t oledAddress = 0x3C;
constexpr uint32_t pollIntervalMs = 1000;
constexpr uint32_t displayIntervalMs = 1000;
constexpr uint32_t mqttRetryIntervalMs = 5000;
constexpr uint32_t serialBaudRate = 115200;
constexpr char mqttClientId[] = "esp32s3-spi-gateway";

// The DevKitC-1 RGB LED is on GPIO48 (board v1.0) or GPIO38 (v1.1); neither pin is used otherwise.
constexpr uint8_t rgbLedPins[] = {38, 48};

SpiController spi(spiSckPin, spiMisoPin, spiMosiPin, spiCsPin);
WifiConnection wifi(wifiSsid, wifiPassword);
WiFiClient networkClient;
MqttPublisher mqtt(networkClient, mqttBrokerHost, mqttBrokerPort, mqttClientId, mqttUser, mqttPassword, mqttRetryIntervalMs);
TemperatureDisplay display(screenWidth, screenHeight, i2cSdaPin, i2cSclPin, oledAddress);
SerialLogger logger(serialBaudRate);
GatewayApp gateway(spi, wifi, mqtt, display, logger, mqttTopic, pollIntervalMs, displayIntervalMs);

/**
 * @brief Turns off the onboard RGB LED, which some boards light with a random color at power-up.
 */
void turn_off_rgb_led()
{
  for (uint8_t pin : rgbLedPins)
  {
    neopixelWrite(pin, 0, 0, 0);
  }
}

/**
 * @brief Arduino entry point: turns off the RGB LED and starts the gateway.
 */
void setup()
{
  turn_off_rgb_led();
  gateway.begin();
}

/**
 * @brief Arduino main loop: services connections, polls the ATmega328P, publishes new samples and refreshes the display.
 */
void loop()
{
  gateway.update();
}
