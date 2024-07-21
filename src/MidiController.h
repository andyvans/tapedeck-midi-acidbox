#pragma once

#include <Arduino.h>

class MidiController
{
public:
  void Setup();
  void Tick();
  
private:
  void sendEncoderControlChange();
  void sendProgramChange(uint8_t program, uint8_t channel);
  void sendControlChange(uint8_t number, uint8_t value, uint8_t channel);
};