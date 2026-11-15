#pragma once

#include <Arduino.h>
#include <protocol.h>

/**
 * @brief Answers SPI requests from the ESP32-S3 using the hardware SPI in peripheral mode.
 *
 * The SPI and SS pin-change interrupts must forward to handle_byte_received() and reset_transaction().
 */
class SpiPeripheral
{
public:
  /**
   * @brief Enables the SPI peripheral, its interrupt and the SS pin-change interrupt.
   */
  void begin();

  /**
   * @brief Replaces the official sample, served on ReadSample, atomically with respect to the SPI interrupt.
   *
   * @param sample Frame to serve on the next ReadSample command.
   */
  void set_sample(const SampleFrame &sample);

  /**
   * @brief Replaces the live reading, served on ReadLive, atomically with respect to the SPI interrupt.
   *
   * @param reading Frame to serve on the next ReadLive command.
   */
  void set_live_reading(const SampleFrame &reading);

  /**
   * @brief Handles one byte received from the controller and queues the next byte to send. Called from the SPI ISR.
   *
   * @param received Byte the controller just sent.
   */
  void handle_byte_received(uint8_t received);

  /**
   * @brief Prepares for a new command on every SS edge. Called from the pin-change ISR.
   *
   * Resetting on both edges keeps back-to-back transactions apart even if this ISR runs after SS has fallen again.
   */
  void reset_transaction();

private:
  static constexpr uint8_t idleByte = 0x00;

  SampleFrame officialSample = {};
  SampleFrame liveReading = {};
  uint8_t txBuffer[sizeof(SampleFrame)] = {};
  uint8_t txIndex = sizeof(SampleFrame);
  bool awaitingCommand = true;

  /**
   * @brief Snapshots a frame into the transmit buffer and queues its first byte.
   *
   * @param frame Frame to send for the current command.
   */
  void start_transfer(const SampleFrame &frame);

  /**
   * @brief Queues the next byte of the transmit buffer, or the idle byte past its end.
   */
  void queue_next_byte();
};
