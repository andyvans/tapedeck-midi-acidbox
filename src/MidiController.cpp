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

#define SYNTH1_MIDI_CHAN 1
#define SYNTH2_MIDI_CHAN 2

#define DRUM_MIDI_CHAN 10

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

MidiController::MidiController() :
  drumButton(ROTARY_ENCODER_1_SW_PIN),
  drumEncoder(ROTARY_ENCODER_1_A_PIN, ROTARY_ENCODER_1_B_PIN)
{
  drumButton.attachClick([]() {
    SendProgramChange(0, DRUM_MIDI_CHAN);
  });
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

  drumButton.tick();
  drumEncoder.tick();

  int newPos = drumEncoder.getPosition();
  if (newPos != encoderPos)
  {
    encoderPos = newPos;
    SendControlChange(CC_808_VOLUME, encoderPos, DRUM_MIDI_CHAN);
    drumEncoder.setPosition(0);
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

#ifdef DEBUG
  Serial.print("Control change:");
  Serial.print(number);
  Serial.print(" ");
  Serial.print(value);
  Serial.print(" ");
  Serial.print(channel);
  Serial.println();

  // printf("Control change: %d %d %d\n", number, value, channel);
#endif

#ifdef ENABLE_MIDI
  MIDI.sendControlChange(number, value, channel);
#endif
}

