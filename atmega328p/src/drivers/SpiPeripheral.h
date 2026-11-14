#pragma once

#include <Arduino.h>
#include <protocol.h>

/**
 * @brief Answers SPI requests from the ESP32-S3 using the hardware SPI in peripheral mode.
 *
 * The SPI and SS pin-change interrupts must forward to handle_byte_received() and handle_deselect().
 */
class SpiPeripheral
{
public:
  /**
   * @brief Enables the SPI peripheral, its interrupt and the SS pin-change interrupt.
   */
  void begin();

  /**
   * @brief Replaces the sample served to the controller, atomically with respect to the SPI interrupt.
   *
   * @param sample Frame to serve on the next ReadSample command.
   */
  void set_sample(const SampleFrame &sample);

  /**
   * @brief Handles one byte received from the controller and queues the next byte to send. Called from the SPI ISR.
   *
   * @param received Byte the controller just sent.
   */
  void handle_byte_received(uint8_t received);

  /**
   * @brief Resets the transaction state when the controller releases SS. Called from the pin-change ISR.
   */
  void handle_deselect();

private:
  static constexpr uint8_t idleByte = 0x00;

  SampleFrame currentSample = {};
  uint8_t txBuffer[sizeof(SampleFrame)] = {};
  uint8_t txIndex = sizeof(SampleFrame);
  bool awaitingCommand = true;

  /**
   * @brief Snapshots the current sample into the transmit buffer and queues its first byte.
   */
  void start_sample_transfer();

  /**
   * @brief Queues the next byte of the transmit buffer, or the idle byte past its end.
   */
  void queue_next_byte();
};
