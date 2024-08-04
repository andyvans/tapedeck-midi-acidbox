#include "MidiController.h"
#include <MIDI.h>
#include "constants.h"
#include "AcidBoxMidi.h"

// MIDI channels for keyup and keydown events
#define KEY_DOWN_CHANNEL 1
#define KEY_UP_CHANNEL 2

// MIDI value ranges
#define MIN_MIDI_VAL 0
#define MAX_MIDI_VAL 127

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

MidiController::MidiController()
{
  int buttonCount = -1;
  int encoderCount = -1;

  // Button 1
  auto button1 = new OneButton(BUTTON_1_PIN, true);
  button1-> attachClick([]() {
    SendControlChange(CC_808_VOLUME, 0, DRUM_MIDI_CHAN);
  });
  buttons[++buttonCount] = button1;

  // Encoder 1
  auto encoder1 = new OneRotaryEncoder(ROTARY_ENCODER_1_A_PIN, ROTARY_ENCODER_1_B_PIN, ROTARY_ENCODER_1_SW_PIN);
  encoder1->AttachRotate([](int pos) {
    SendControlChange(CC_808_VOLUME, pos, DRUM_MIDI_CHAN);
  });
  encoder1->AttachClickWithState([](bool state, int pos) {
    SendControlChange(CC_808_VOLUME, state? 0 : pos, DRUM_MIDI_CHAN);
  });
  encoders[++encoderCount] = encoder1;

  // Encoder 2
  auto encoder2 = new OneRotaryEncoder(ROTARY_ENCODER_2_A_PIN, ROTARY_ENCODER_2_B_PIN, ROTARY_ENCODER_2_SW_PIN);
  encoder2->AttachRotate([](int pos) {
    SendControlChange(CC_303_VOLUME, pos, SYNTH1_MIDI_CHAN);
  });
  encoder2->AttachClickWithState([](bool state, int pos) {
    SendControlChange(CC_303_VOLUME, state? 0 : pos, SYNTH1_MIDI_CHAN);
  });
  encoders[++encoderCount] = encoder2;

  // Encoder 3
  auto encoder3 = new OneRotaryEncoder(ROTARY_ENCODER_2_A_PIN, ROTARY_ENCODER_2_B_PIN, ROTARY_ENCODER_2_SW_PIN);
  encoder3->AttachRotate([](int pos) {
    SendControlChange(CC_303_VOLUME, pos, SYNTH2_MIDI_CHAN);
  });
  encoder3->AttachClickWithState([](bool state, int pos) {
    SendControlChange(CC_303_VOLUME, state? 0 : pos, SYNTH2_MIDI_CHAN);
  });
  encoders[++encoderCount] = encoder3;
}

void MidiController::Setup()
{
  #ifdef ENABLE_MIDI
  MIDI.begin(MIDI_CHANNEL_OMNI);
  #endif
}

void MidiController::Tick()
{
#ifdef ENABLE_MIDI
  MIDI.read();
#endif

  for (int i = 0; i < MIDI_CONTROLLER_MAX_DEVICES; i++)
  {
    if (buttons[i] != NULL)
    {
      buttons[i]->tick();
    }

    if (encoders[i] != NULL)
    {
      encoders[i]->Tick();
    }
  }
}

void MidiController::SendProgramChange(int program, int channel)
{
#ifdef DEBUG
  Serial.print("Program change: ");
  Serial.print(program);
  Serial.print(" ");
  Serial.println(channel);
#endif

#ifdef ENABLE_MIDI
  MIDI.sendProgramChange(program, channel);
#endif
}

void MidiController::SendControlChange(uint8_t number, uint8_t value, uint8_t channel)
{
  // Midi values are 0-127, so we need to constrain the value to that range
  int midiValue = constrain(value, 0, 127);

#ifdef DEBUG
  Serial.print("Control change:");
  Serial.print(number);
  Serial.print(" ");
  Serial.print(midiValue);
  Serial.print(" ");
  Serial.print(channel);
  Serial.println();
#endif

#ifdef ENABLE_MIDI
  MIDI.sendControlChange(number, midiValue, channel);
#endif
}

