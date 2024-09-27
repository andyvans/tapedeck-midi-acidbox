#include "AudioControls.h"
#include <MIDI.h>
#include "constants.h"
#include "AcidBoxMidi.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

AudioControls::AudioControls()
{
  // Set switch pins to digital input
  pinMode(LEFT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(RIGHT_SWITCH_PIN, INPUT_PULLUP);

  // Setup encoders and buttons
  int buttonCount = -1;
  int encoderCount = -1;

  // REWIND button
  // auto button1 = new OneButton(REWIND_BUTTON_PIN, true);
  // button1->attachLongPressStart([](void *scope) {
  //   ((AudioControls*)scope)->SendControlChange(CC_ANY_DELAY_FB, 127, DRUM_MIDI_CHAN);
  // }, this);
  // button1->attachLongPressStop([](void *scope) {
  //   ((AudioControls*)scope)->SendControlChange(CC_ANY_DELAY_FB, 0, DRUM_MIDI_CHAN);
  // }, this);
  // buttons[++buttonCount] = button1;

  // Encoder 1
  auto encoder1 = new OneRotaryEncoder(ROTARY_ENCODER_1_A_PIN, ROTARY_ENCODER_1_B_PIN, 64, 0, 127);
  encoders[++encoderCount] = encoder1;

  // Encoder 2
  auto encoder2 = new OneRotaryEncoder(ROTARY_ENCODER_2_A_PIN, ROTARY_ENCODER_2_B_PIN, ROTARY_ENCODER_2_SW_PIN, 64, 0, 127);
  encoders[++encoderCount] = encoder2;

  // Encoder 3
  auto encoder3 = new OneRotaryEncoder(ROTARY_ENCODER_3_A_PIN, ROTARY_ENCODER_3_B_PIN, ROTARY_ENCODER_3_SW_PIN, 64, 0, 127);
  encoders[++encoderCount] = encoder3;

  // Encoder 4
  auto encoder4 = new OneRotaryEncoder(ROTARY_ENCODER_4_A_PIN, ROTARY_ENCODER_4_B_PIN, ROTARY_ENCODER_4_SW_PIN, 64, 0, 127);
  encoders[++encoderCount] = encoder4;
}

void AudioControls::Setup()
{
  display.Setup();

#ifdef ENABLE_MIDI
  MIDI.begin(MIDI_CHANNEL_OMNI);
#endif
}

void AudioControls::Tick()
{
#ifdef ENABLE_MIDI
  MIDI.read();
#endif

  for (int i = 0; i < MIDI_CONTROLLER_MAX_DEVICES; i++)
  {
    if (buttons[i] != NULL) buttons[i]->tick();
    if (encoders[i] != NULL) encoders[i]->Tick();
  }

  ProcessAudioControl();
  controlsInitialised = true;
  display.Tick();
}

void AudioControls::ProcessAudioControl()
{
  UpdateMidiState();
  auto encoderPos1 = encoders[0]->GetPosition();
  auto encoderPos2 = encoders[1]->GetPosition();
  auto encoderPos3 = encoders[2]->GetPosition();
  auto encoderPos4 = encoders[3]->GetPosition();

  switch (midiState)
  {
  case AudioControlMode::Mode0:
    if (encoderPos1.hasNewPosition) SendControlChange(CC_808_VOLUME, encoderPos1.position, DRUM_MIDI_CHAN);
    if (encoderPos2.hasNewPosition) SendControlChange(CC_303_VOLUME, encoderPos2.position, SYNTH1_MIDI_CHAN);
    if (encoderPos3.hasNewPosition) SendControlChange(CC_303_VOLUME, encoderPos3.position, SYNTH2_MIDI_CHAN);
    if (encoderPos4.hasNewPosition) SendControlChange(CC_ANY_DELAY_FB, encoderPos4.position, GLOBAL_MIDI_CHAN);
    break;
  case AudioControlMode::Mode1:
    if (encoderPos1.hasNewPosition) SendControlChange(CC_808_DISTORTION, encoderPos1.position, DRUM_MIDI_CHAN);
    if (encoderPos2.hasNewPosition) SendControlChange(CC_808_BD_TONE, encoderPos2.position, DRUM_MIDI_CHAN);
    if (encoderPos3.hasNewPosition) SendControlChange(CC_808_PITCH, encoderPos3.position, DRUM_MIDI_CHAN);
    if (encoderPos4.hasNewPosition) SendControlChange(CC_808_CH_TUNE, encoderPos4.position, DRUM_MIDI_CHAN);
    break;
  case AudioControlMode::Mode2:
    if (encoderPos1.hasNewPosition) SendControlChange(CC_ANY_DELAY_TIME, encoderPos1.position, GLOBAL_MIDI_CHAN);
    if (encoderPos2.hasNewPosition) SendControlChange(CC_ANY_DELAY_FB, encoderPos2.position, GLOBAL_MIDI_CHAN);
    if (encoderPos3.hasNewPosition) SendControlChange(CC_ANY_DELAY_LVL, encoderPos3.position, GLOBAL_MIDI_CHAN);
    if (encoderPos4.hasNewPosition) SendControlChange(CC_ANY_REVERB_TIME, encoderPos4.position, GLOBAL_MIDI_CHAN);
    break;
  case AudioControlMode::Mode3:
    if (encoderPos1.hasNewPosition) SendControlChange(CC_ANY_REVERB_TIME, encoderPos1.position, GLOBAL_MIDI_CHAN);
    if (encoderPos2.hasNewPosition) SendControlChange(CC_ANY_REVERB_LVL, encoderPos2.position, GLOBAL_MIDI_CHAN);
    if (encoderPos3.hasNewPosition) SendControlChange(CC_808_NOTE_ATTACK, encoderPos3.position, DRUM_MIDI_CHAN);
    if (encoderPos4.hasNewPosition) SendControlChange(CC_808_OH_LEVEL, encoderPos4.position, DRUM_MIDI_CHAN);
    break;
  }
}

void AudioControls::UpdateMidiState()
{
  leftSwitchState = digitalRead(LEFT_SWITCH_PIN) == HIGH;
  rightSwitchState = digitalRead(RIGHT_SWITCH_PIN) == HIGH;

  auto newMidiState = midiState;
  if (leftSwitchState && rightSwitchState)
  {
    newMidiState = AudioControlMode::Mode3;
  }
  else if (leftSwitchState)
  {
    newMidiState = AudioControlMode::Mode2;
  }
  else if (rightSwitchState)
  {
    newMidiState = AudioControlMode::Mode0;
  }
  else
  {
    newMidiState = AudioControlMode::Mode1;
  }

#ifndef ENABLE_MIDI
  if (newMidiState != midiState)
  {
    Serial.print("Midi state: ");
    Serial.print(newMidiState);
    Serial.println();
  }
#endif
  midiState = newMidiState;
}

void AudioControls::SendProgramChange(int program, int channel)
{
  if (!controlsInitialised) return;

  sprintf(textBuffer, "%s %s", GetMidiChannelName(channel), GetMidiControlName(program));
  display.WriteText(textBuffer);

#ifdef ENABLE_MIDI
  MIDI.sendProgramChange(program, channel);
#else
  Serial.print("Program change:");
  Serial.print(program);
  Serial.print(" ");
  Serial.print(channel);
  Serial.println();
#endif
}

void AudioControls::SendControlChange(uint8_t program, uint8_t midiValue, uint8_t channel)
{
  if (!controlsInitialised) return;

  if (midiValue > 127) midiValue = 127;
  if (midiValue < 0) midiValue = 0;

  sprintf(textBuffer, "%s %s %d", GetMidiChannelName(channel), GetMidiControlName(program), midiValue);
  display.WriteText(textBuffer);

  // Global is uses channel 1
  if (channel == GLOBAL_MIDI_CHAN) channel = 1;

#ifdef ENABLE_MIDI
  MIDI.sendControlChange(program, midiValue, channel);
#else
  Serial.print("Control change:");
  Serial.print(program);
  Serial.print(" ");
  Serial.print(midiValue);
  Serial.print(" ");
  Serial.print(channel);
  Serial.println();
#endif
}

const char *AudioControls::GetMidiChannelName(uint8_t channel)
{
  switch (channel)
  {
  case SYNTH1_MIDI_CHAN:
    return "Synth1";
  case SYNTH2_MIDI_CHAN:
    return "Synth2";
  case DRUM_MIDI_CHAN:
    return "Drums";
  default:
    return "Global";
  }
}

const char *AudioControls::GetMidiControlName(uint8_t number)
{
  switch (number)
  {
  case CC_303_PORTATIME:
    return "Portatime";
  case CC_303_VOLUME:
    return "Volume";
  case CC_303_PORTAMENTO:
    return "Portamento";
  case CC_303_PAN:
    return "Pan";
  case CC_303_WAVEFORM:
    return "Waveform";
  case CC_303_RESO:
    return "Resonance";
  case CC_303_CUTOFF:
    return "Cutoff";
  case CC_303_ATTACK:
    return "Attack";
  case CC_303_DECAY:
    return "Decay";
  case CC_303_ENVMOD_LVL:
    return "Env modlvl";
  case CC_303_ACCENT_LVL:
    return "Accent lvl";
  case CC_303_REVERB_SEND:
    return "Reverb";
  case CC_303_DELAY_SEND:
    return "Delay";
  case CC_303_DISTORTION:
    return "Distortion";
  case CC_303_SATURATOR:
    return "Saturation";

  case CC_808_NOTE_PAN:
    return "Note pan";
  case CC_808_PITCH:
    return "Pitch";
  case CC_808_NOTE_SEL:
    return "Note sel";
  case CC_808_BD_TONE:
    return "BD tone";
  case CC_808_BD_DECAY:
    return "BD decay";
  case CC_808_BD_LEVEL:
    return "BD level";
  case CC_808_SD_TONE:
    return "SD tone";
  case CC_808_SD_SNAP:
    return "SD snap";
  case CC_808_SD_LEVEL:
    return "SD level";
  case CC_808_CH_TUNE:
    return "Ch tune";
  case CC_808_CH_LEVEL:
    return "Ch level";
  case CC_808_OH_TUNE:
    return "OH tune";
  case CC_808_OH_DECAY:
    return "OH decay";
  case CC_808_OH_LEVEL:
    return "OH level";

  case CC_ANY_COMPRESSOR:
    return "Compressor";
  case CC_ANY_DELAY_TIME:
    return "Delay";
  case CC_ANY_DELAY_FB:
    return "Delay FB";
  case CC_ANY_DELAY_LVL:
    return "Delay lvl";
  case CC_ANY_REVERB_TIME:
    return "Reverb Tm";
  case CC_ANY_REVERB_LVL:
    return "Reverb lvl";
  case CC_ANY_RESET_CCS:
    return "Reset CCS";
  case CC_ANY_NOTES_OFF:
    return "Notes Off";
  case CC_ANY_SOUND_OFF:
    return "Sound Off";
  default:
    return "Unknown";
  }
}
