#include "TemperatureDisplay.h"

TemperatureDisplay::TemperatureDisplay(uint8_t width, uint8_t height, uint8_t i2cAddress)
    : oled(width, height, &Wire, resetPin), i2cAddress(i2cAddress)
{
}

bool TemperatureDisplay::begin()
{
  if (!oled.begin(SSD1306_SWITCHCAPVCC, i2cAddress))
  {
    return false;
  }
  oled.cp437(true);
  oled.setTextColor(SSD1306_WHITE);
  return true;
}

void TemperatureDisplay::show(float temperatureC, uint16_t rawValue)
{
  oled.clearDisplay();
  draw_title();
  draw_temperature(temperatureC);
  draw_raw_value(rawValue);
  oled.display();
}

void TemperatureDisplay::draw_title()
{
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print(F("Temperature"));
}

void TemperatureDisplay::draw_temperature(float temperatureC)
{
  oled.setTextSize(3);
  oled.setCursor(0, 20);
  oled.print(temperatureC, 1);
  oled.print(degreeSymbol);
  oled.print('C');
}

void TemperatureDisplay::draw_raw_value(uint16_t rawValue)
{
  oled.setTextSize(1);
  oled.setCursor(0, 56);
  oled.print(F("ADC: "));
  oled.print(rawValue);
}
