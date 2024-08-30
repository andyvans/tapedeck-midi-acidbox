#pragma once

#include <Arduino.h>
#include <OneButton.h>
#include "OneRotaryEncoder.h"

//#define ENABLE_MIDI
#define MIDI_CONTROLLER_MAX_DEVICES 20

enum AudioControlMode
{
  Mode1,
  Mode2,
  Mode3,
  Mode4,
};

class AudioControls
{
public:
  AudioControls();
  void Setup();
  void Tick();
  
private:  
  OneButton* buttons[MIDI_CONTROLLER_MAX_DEVICES] = {};
  OneRotaryEncoder* encoders[MIDI_CONTROLLER_MAX_DEVICES] = {};

  AudioControlMode GetControlSet();

  void ProcessAudioControl();

  void SendProgramChange(int program, int channel);
  void SendControlChange(uint8_t number, uint8_t value, uint8_t channel);
};