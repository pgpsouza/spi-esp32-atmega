#pragma once

#include <stdint.h>

// Copy this file to secrets.h (ignored by Git) and fill in the real values.
// The broker IP and topic are announced 5 minutes before the presentation: edit secrets.h and upload again.

constexpr char wifiSsid[] = "your-ssid";
constexpr char wifiPassword[] = "your-password";

constexpr char mqttBrokerHost[] = "192.168.0.10";
constexpr uint16_t mqttBrokerPort = 1883;
constexpr char mqttTopic[] = "ifes/temperature";

// Leave empty when the broker does not require authentication.
constexpr char mqttUser[] = "";
constexpr char mqttPassword[] = "";
