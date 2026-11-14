#pragma once

#include <stddef.h>
#include <protocol.h>

/**
 * @brief Formats a sample received over SPI as the MQTT message body.
 */
class SamplePayload
{
public:
  /**
   * @brief Writes the sample as JSON, e.g. {"sequence":12,"temperature":23.45,"raw":481}.
   *
   * @param sample Valid frame received from the ATmega328P.
   * @param buffer Destination for the null-terminated text; 64 bytes fit any frame.
   * @param size Size of buffer in bytes.
   */
  static void format(const SampleFrame &sample, char *buffer, size_t size);
};
