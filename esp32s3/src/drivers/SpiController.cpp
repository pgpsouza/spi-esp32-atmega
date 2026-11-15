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
  return request(Command::ReadSample, sample);
}

bool SpiController::read_live(SampleFrame &reading)
{
  return request(Command::ReadLive, reading);
}

bool SpiController::request(Command command, SampleFrame &frame)
{
  uint8_t buffer[sizeof(SampleFrame)];
  transfer_frame(command, buffer);
  memcpy(&frame, buffer, sizeof(SampleFrame));
  return protocol_is_valid(frame);
}

void SpiController::transfer_frame(Command command, uint8_t *buffer)
{
  SPI.beginTransaction(settings);
  digitalWrite(csPin, LOW);
  delayMicroseconds(spiSelectSetupUs);
  transfer_byte(static_cast<uint8_t>(command));
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
