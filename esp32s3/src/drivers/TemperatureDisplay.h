#pragma once

#include <Adafruit_SSD1306.h>

/**
 * @brief Shows the latest temperature received from the ATmega328P on an SSD1306 OLED connected over I2C.
 */
class TemperatureDisplay
{
public:
  /**
   * @brief Configures the display geometry, I2C pins and address.
   *
   * @param width Display width in pixels.
   * @param height Display height in pixels.
   * @param sdaPin I2C data pin.
   * @param sclPin I2C clock pin.
   * @param i2cAddress I2C address of the SSD1306.
   */
  TemperatureDisplay(uint8_t width, uint8_t height, uint8_t sdaPin, uint8_t sclPin, uint8_t i2cAddress);

  /**
   * @brief Starts the I2C bus, initializes the OLED and allocates its frame buffer.
   *
   * @return true if the display answered on the I2C bus.
   */
  bool begin();

  /**
   * @brief Draws the temperature in large digits and the time left until the next sample.
   *
   * @param temperatureC Temperature in degrees Celsius.
   * @param secondsToNextSample Seconds until the ATmega328P makes the next sample available.
   */
  void show_temperature(float temperatureC, uint8_t secondsToNextSample);

  /**
   * @brief Draws a placeholder while no valid sample has been received.
   */
  void show_waiting();

private:
  static constexpr int8_t resetPin = -1;
  static constexpr char degreeSymbol = static_cast<char>(0xF8);

  Adafruit_SSD1306 oled;
  uint8_t sdaPin;
  uint8_t sclPin;
  uint8_t i2cAddress;

  /**
   * @brief Clears the frame buffer and draws the screen title on the top line.
   */
  void draw_title();

  /**
   * @brief Draws a value in large digits followed by the Celsius unit.
   *
   * @param value Text of the value, e.g. "23.5" or "--.-".
   */
  void draw_value(const char *value);

  /**
   * @brief Draws a status line at the bottom of the screen.
   *
   * @param text Status text, e.g. "Next in 23 s".
   */
  void draw_status(const char *text);
};
