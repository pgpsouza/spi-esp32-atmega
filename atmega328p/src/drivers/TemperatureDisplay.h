#pragma once

#include <Adafruit_SSD1306.h>

/**
 * @brief Shows temperature readings on an SSD1306 OLED connected over I2C.
 */
class TemperatureDisplay
{
public:
  /**
   * @brief Configures the display geometry and I2C address.
   *
   * @param width Display width in pixels.
   * @param height Display height in pixels.
   * @param i2cAddress I2C address of the SSD1306.
   */
  TemperatureDisplay(uint8_t width, uint8_t height, uint8_t i2cAddress);

  /**
   * @brief Initializes the OLED and allocates its frame buffer.
   *
   * @return true if the display answered on the I2C bus.
   */
  bool begin();

  /**
   * @brief Draws the temperature in large digits and the raw ADC value below it.
   *
   * @param temperatureC Temperature in degrees Celsius.
   * @param rawValue ADC value the temperature was computed from.
   */
  void show(float temperatureC, uint16_t rawValue);

private:
  static constexpr int8_t resetPin = -1;
  static constexpr char degreeSymbol = static_cast<char>(0xF8);

  Adafruit_SSD1306 oled;
  uint8_t i2cAddress;

  /**
   * @brief Draws the screen title on the top line.
   */
  void draw_title();

  /**
   * @brief Draws the temperature in large digits, followed by the unit.
   *
   * @param temperatureC Temperature in degrees Celsius.
   */
  void draw_temperature(float temperatureC);

  /**
   * @brief Draws the raw ADC value on the bottom line.
   *
   * @param rawValue ADC value to display.
   */
  void draw_raw_value(uint16_t rawValue);
};
