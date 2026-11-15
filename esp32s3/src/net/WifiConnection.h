#pragma once

#include <Arduino.h>

/**
 * @brief Keeps the ESP32-S3 connected to a Wi-Fi network as a station.
 */
class WifiConnection
{
public:
  /**
   * @brief Stores the network credentials.
   *
   * @param ssid Network name.
   * @param password Network password.
   */
  WifiConnection(const char *ssid, const char *password);

  /**
   * @brief Starts connecting in the background, with automatic reconnection after drops.
   */
  void begin();

  /**
   * @brief Reports the connection state.
   *
   * @return true if connected and holding an IP address.
   */
  bool is_connected() const;

private:
  const char *ssid;
  const char *password;
};
