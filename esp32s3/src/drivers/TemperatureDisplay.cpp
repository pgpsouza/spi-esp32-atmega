#include "TemperatureDisplay.h"

TemperatureDisplay::TemperatureDisplay(uint8_t width, uint8_t height, uint8_t sdaPin, uint8_t sclPin, uint8_t i2cAddress)
    : oled(width, height, &Wire, resetPin), sdaPin(sdaPin), sclPin(sclPin), i2cAddress(i2cAddress)
{
}

bool TemperatureDisplay::begin()
{
  Wire.begin(sdaPin, sclPin);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, i2cAddress, true, false))
  {
    return false;
  }
  oled.cp437(true);
  oled.setTextColor(SSD1306_WHITE);
  return true;
}

void TemperatureDisplay::show_temperature(float temperatureC, uint8_t secondsToNextSample)
{
  char value[8];
  char status[24];
  snprintf(value, sizeof(value), "%.1f", temperatureC);
  snprintf(status, sizeof(status), "Next in %u s", secondsToNextSample);
  draw_title();
  draw_value(value);
  draw_status(status);
  oled.display();
}

void TemperatureDisplay::show_waiting()
{
  draw_title();
  draw_value("--.-");
  draw_status("Waiting for ATmega");
  oled.display();
}

void TemperatureDisplay::draw_title()
{
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print(F("Temperature"));
}

void TemperatureDisplay::draw_value(const char *value)
{
  oled.setTextSize(3);
  oled.setCursor(0, 20);
  oled.print(value);
  oled.print(degreeSymbol);
  oled.print('C');
}

void TemperatureDisplay::draw_status(const char *text)
{
  oled.setTextSize(1);
  oled.setCursor(0, 56);
  oled.print(text);
}
