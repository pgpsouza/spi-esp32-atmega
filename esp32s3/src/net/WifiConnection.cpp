#include "WifiConnection.h"

#include <WiFi.h>

WifiConnection::WifiConnection(const char *ssid, const char *password)
    : ssid(ssid), password(password)
{
}

void WifiConnection::begin()
{
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
}

bool WifiConnection::is_connected() const
{
  return WiFi.status() == WL_CONNECTED;
}
