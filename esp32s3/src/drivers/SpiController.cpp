#include "SpiController.h"

SpiController::SpiController(uint8_t sckPin, uint8_t misoPin, uint8_t mosiPin, uint8_t csPin)
    : sckPin(sckPin), misoPin(misoPin), mosiPin(mosiPin), csPin(csPin), settings(spiClockHz, MSBFIRST, SPI_MODE0)
{
}

void SpiController::begin()
{
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  SPI.begin(sckPin, misoPin, mosiPin);
}

bool SpiController::read_sample(SampleFrame &sample)
{
  uint8_t buffer[sizeof(SampleFrame)];
  transfer_frame(buffer);
  memcpy(&sample, buffer, sizeof(SampleFrame));
  return protocol_is_valid(sample);
}

void SpiController::transfer_frame(uint8_t *buffer)
{
  SPI.beginTransaction(settings);
  digitalWrite(csPin, LOW);
  delayMicroseconds(spiSelectSetupUs);
  transfer_byte(static_cast<uint8_t>(Command::ReadSample));
  for (uint8_t i = 0; i < sizeof(SampleFrame); i++)
  {
    buffer[i] = transfer_byte(0x00);
  }
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
}

uint8_t SpiController::transfer_byte(uint8_t value)
{
  uint8_t received = SPI.transfer(value);
  delayMicroseconds(spiInterByteDelayUs);
  return received;
}
