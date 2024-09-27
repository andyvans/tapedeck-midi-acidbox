#pragma once

#include <Arduino.h>
#include <OneButton.h>
#include "OneRotaryEncoder.h"
#include "Display.h"

#define ENABLE_MIDI
#define MIDI_CONTROLLER_MAX_DEVICES 20

enum AudioControlMode
{
  Mode0,
  Mode1,
  Mode2,
  Mode3,
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
  Display display;

  void UpdateMidiState();
  void ProcessAudioControl();
  void SendProgramChange(int program, int channel);
  void SendControlChange(uint8_t number, uint8_t value, uint8_t channel);

  const char* GetMidiControlName(uint8_t number);
  const char* GetMidiChannelName(uint8_t channel);

  AudioControlMode midiState = AudioControlMode::Mode0;
  bool leftSwitchState = false;
  bool rightSwitchState = false;

  char textBuffer[64];
  bool controlsInitialised = false;
};