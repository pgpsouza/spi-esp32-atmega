#include <Arduino.h>
#include <WiFi.h>

#include "app/GatewayApp.h"
#include "secrets.h"

constexpr uint8_t spiSckPin = 12;
constexpr uint8_t spiMisoPin = 13;
constexpr uint8_t spiMosiPin = 11;
constexpr uint8_t spiCsPin = 10;
constexpr uint32_t pollIntervalMs = 1000;
constexpr uint32_t mqttRetryIntervalMs = 5000;
constexpr uint32_t serialBaudRate = 115200;
constexpr char mqttClientId[] = "esp32s3-spi-gateway";

SpiController spi(spiSckPin, spiMisoPin, spiMosiPin, spiCsPin);
WifiConnection wifi(wifiSsid, wifiPassword);
WiFiClient networkClient;
MqttPublisher mqtt(networkClient, mqttBrokerHost, mqttBrokerPort, mqttClientId, mqttUser, mqttPassword, mqttRetryIntervalMs);
SerialLogger logger(serialBaudRate);
GatewayApp gateway(spi, wifi, mqtt, logger, mqttTopic, pollIntervalMs);

/**
 * @brief Arduino entry point: starts the gateway.
 */
void setup()
{
  gateway.begin();
}

/**
 * @brief Arduino main loop: services connections, polls the ATmega328P and publishes new samples.
 */
void loop()
{
  gateway.update();
}
