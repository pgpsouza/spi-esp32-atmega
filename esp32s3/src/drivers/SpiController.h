#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <protocol.h>

/**
 * @brief Reads samples and live readings from the ATmega328P over SPI, acting as the bus controller.
 */
class SpiController
{
public:
  /**
   * @brief Configures the SPI pins; chip select is driven manually to control the timing the peripheral needs.
   *
   * @param sckPin Clock pin.
   * @param misoPin Data input pin (from the peripheral, through the voltage divider).
   * @param mosiPin Data output pin.
   * @param csPin Chip select pin, active low.
   */
  SpiController(uint8_t sckPin, uint8_t misoPin, uint8_t mosiPin, uint8_t csPin);

  /**
   * @brief Starts the SPI bus and releases chip select.
   */
  void begin();

  /**
   * @brief Requests the latest official sample from the peripheral.
   *
   * @param sample Receives the frame read from the bus.
   * @return true if the frame has the expected version and a valid checksum.
   */
  bool read_sample(SampleFrame &sample);

  /**
   * @brief Requests the live reading from the peripheral; its sequence number is always 0.
   *
   * @param reading Receives the frame read from the bus.
   * @return true if the frame has the expected version and a valid checksum.
   */
  bool read_live(SampleFrame &reading);

private:
  uint8_t sckPin;
  uint8_t misoPin;
  uint8_t mosiPin;
  uint8_t csPin;
  SPISettings settings;

  /**
   * @brief Sends a command and validates the frame the peripheral answers with.
   *
   * @param command Command to send.
   * @param frame Receives the frame read from the bus.
   * @return true if the frame has the expected version and a valid checksum.
   */
  bool request(Command command, SampleFrame &frame);

  /**
   * @brief Sends a command and clocks in the frame bytes, inside one chip select window.
   *
   * @param command Command to send.
   * @param buffer Receives the frame bytes.
   */
  void transfer_frame(Command command, uint8_t *buffer);

  /**
   * @brief Exchanges one byte, then waits for the peripheral to queue its next byte.
   *
   * @param value Byte to send.
   * @return Byte received.
   */
  uint8_t transfer_byte(uint8_t value);
};
