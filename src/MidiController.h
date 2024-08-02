#pragma once

#include <Arduino.h>
#include <OneButton.h>
#include <RotaryEncoder.h>

#define ENABLE_MIDI

class MidiController
{
public:
  MidiController();
  void Setup();
  void Tick();
  
private:
  OneButton drumButton;
  RotaryEncoder drumEncoder;
  int encoderPos = 0;

  static void SendProgramChange(int program, int channel);
  static void SendControlChange(uint8_t number, uint8_t value, uint8_t channel);
};