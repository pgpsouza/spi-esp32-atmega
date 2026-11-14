#include "SpiPeripheral.h"

#include <string.h>

void SpiPeripheral::begin()
{
  pinMode(MISO, OUTPUT);
  SPDR = idleByte;
  SPCR = _BV(SPE) | _BV(SPIE);
  PCMSK0 |= _BV(PCINT2);
  PCICR |= _BV(PCIE0);
}

void SpiPeripheral::set_sample(const SampleFrame &sample)
{
  noInterrupts();
  currentSample = sample;
  interrupts();
}

void SpiPeripheral::handle_byte_received(uint8_t received)
{
  if (!awaitingCommand)
  {
    queue_next_byte();
    return;
  }
  awaitingCommand = false;
  if (received == static_cast<uint8_t>(Command::ReadSample))
  {
    start_sample_transfer();
  }
}

void SpiPeripheral::handle_deselect()
{
  awaitingCommand = true;
  txIndex = sizeof(txBuffer);
  SPDR = idleByte;
}

void SpiPeripheral::start_sample_transfer()
{
  memcpy(txBuffer, &currentSample, sizeof(txBuffer));
  txIndex = 0;
  queue_next_byte();
}

void SpiPeripheral::queue_next_byte()
{
  SPDR = txIndex < sizeof(txBuffer) ? txBuffer[txIndex++] : idleByte;
}
