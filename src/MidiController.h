#pragma once

#include <MIDI.h>

class MidiController
{
public:
  void setup();
  void tick();

private:
  void sendEncoderControlChange();
  void sendProgramChange(uint8_t program, uint8_t channel);
  void sendControlChange(uint8_t number, uint8_t value, uint8_t channel);
};