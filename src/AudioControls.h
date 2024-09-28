#pragma once

#include <Arduino.h>
#include <OneButton.h>
#include "OneRotaryEncoder.h"
#include "Display.h"
#include "AcidBoxMidi.h"

#define ENABLE_MIDI
#define MIDI_CONTROLLER_MAX_DEVICES 20

enum AudioControlMode
{
  ModeGlobal = GLOBAL_MIDI_CHAN,
  ModeDrum = DRUM_MIDI_CHAN,
  ModeSynth1 = SYNTH1_MIDI_CHAN,
  ModeSynth2 = SYNTH2_MIDI_CHAN,
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
  void UpdateDynamicEncoder(int encoder);
  int SelectNextProgram(int currentProgram, const int *programs, int programCount);

  const char* GetMidiControlName(uint8_t number);
  const char* GetMidiChannelName(uint8_t channel);

  AudioControlMode midiState = AudioControlMode::ModeGlobal;
  bool leftSwitchState = false;
  bool rightSwitchState = false;

  int encoderGlobalProgram[4];
  int encoderDrumProgram[4];
  int encoderSynth1Program[4];  
  int encoderSynth2Program[4];

  static const int globalPrograms[];
  static const int drum808Programs[];
  static const int synth303Programs[];

  char textBuffer[64];
  bool controlsInitialised = false;
};