#pragma once

#include <stdint.h>

// SPI contract between the ESP32-S3 (controller) and the ATmega328P (peripheral).
// Multi-byte fields are little-endian, the native order of both MCUs.

constexpr uint8_t protocolVersion = 1;

constexpr uint32_t spiClockHz = 500000;
constexpr uint8_t spiSelectSetupUs = 10;
constexpr uint8_t spiInterByteDelayUs = 40;

enum class Command : uint8_t
{
  ReadSample = 0xA1,
};

struct __attribute__((packed)) SampleFrame
{
  uint8_t version;
  uint16_t sequence;
  int16_t temperatureCentiC;
  uint16_t rawAdc;
  uint8_t checksum;
};

/**
 * @brief Computes the XOR of every frame byte except the checksum itself.
 *
 * @param frame Frame to compute the checksum for.
 * @return Checksum value.
 */
inline uint8_t protocol_checksum(const SampleFrame &frame)
{
  const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&frame);
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < sizeof(SampleFrame) - 1; i++)
  {
    checksum ^= bytes[i];
  }
  return checksum;
}

/**
 * @brief Checks the protocol version and the checksum of a received frame.
 *
 * @param frame Frame received over SPI.
 * @return true if the frame can be trusted.
 */
inline bool protocol_is_valid(const SampleFrame &frame)
{
  return frame.version == protocolVersion && frame.checksum == protocol_checksum(frame);
}
