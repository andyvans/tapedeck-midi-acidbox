#include "AudioControls.h"
#include <MIDI.h>
#include "constants.h"
#include "AcidBoxMidi.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

const int AudioControls::globalPrograms[] = {CC_ANY_COMPRESSOR, CC_ANY_DELAY_TIME, CC_ANY_DELAY_FB, CC_ANY_DELAY_LVL, CC_ANY_REVERB_TIME, CC_ANY_REVERB_LVL};
const int AudioControls::drum808Programs[] = {CC_808_BD_TONE, CC_808_BD_DECAY, CC_808_BD_LEVEL, CC_808_SD_TONE, CC_808_SD_SNAP, CC_808_SD_LEVEL, CC_808_CH_TONE, CC_808_CH_LEVEL, CC_808_OH_TONE, CC_808_OH_DECAY, CC_808_OH_LEVEL};
const int AudioControls::synth303Programs[] = {CC_303_PORTATIME, CC_303_VOLUME, CC_303_PORTAMENTO, CC_303_PAN, CC_303_WAVEFORM, CC_303_RESO, CC_303_CUTOFF, CC_303_ATTACK, CC_303_DECAY, CC_303_ENVMOD_LVL, CC_303_ACCENT_LVL, CC_303_REVERB_SEND, CC_303_DELAY_SEND, CC_303_DISTORTION, CC_303_SATURATOR};

AudioControls::AudioControls()
{
  // Set initial programs for encoders
  encoderGlobalProgram[0] = CC_808_VOLUME;
  encoderGlobalProgram[1] = CC_303_VOLUME;
  encoderGlobalProgram[2] = CC_303_VOLUME;
  encoderGlobalProgram[3] = CC_ANY_COMPRESSOR;

  encoderDrumProgram[0] = CC_808_PITCH;  
  encoderDrumProgram[1] = CC_808_BD_TONE;
  encoderDrumProgram[2] = CC_808_CH_TONE;
  encoderDrumProgram[3] = CC_808_NOTE_PAN;
  
  encoderSynth1Program[0] = CC_303_PORTATIME;
  encoderSynth1Program[1] = CC_303_PORTAMENTO;
  encoderSynth1Program[2] = CC_303_PAN;
  encoderSynth1Program[3] = CC_303_WAVEFORM;

  encoderSynth2Program[0] = CC_303_PORTATIME;
  encoderSynth2Program[1] = CC_303_PORTAMENTO;
  encoderSynth2Program[2] = CC_303_PAN;
  encoderSynth2Program[3] = CC_303_WAVEFORM;  

  // Set switch pins to digital input
  pinMode(LEFT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(RIGHT_SWITCH_PIN, INPUT_PULLUP);

  // Setup encoders and buttons
  int buttonCount = -1;
  int encoderCount = -1;

  // Encoder switch 1 is used for light theme
  
  // Encoder switch 2  - changes programs
  auto button2 = new OneButton(ROTARY_ENCODER_2_SW_PIN, true);
  button2->attachClick([](void *scope) 
  {
    ((AudioControls*)scope)->UpdateDynamicEncoder(2);
  }, this);  
  buttons[++buttonCount] = button2;

  // Encoder switch 3  - changes programs
  auto button3 = new OneButton(ROTARY_ENCODER_3_SW_PIN, true);
  button3->attachClick([](void *scope) 
  {
    ((AudioControls*)scope)->UpdateDynamicEncoder(3);
  }, this);  
  buttons[++buttonCount] = button3;

  // Encoder switch 4  - changes programs
  auto button4 = new OneButton(ROTARY_ENCODER_4_SW_PIN, true);
  button4->attachClick([](void *scope) 
  {
    ((AudioControls*)scope)->UpdateDynamicEncoder(4);
  }, this);  
  buttons[++buttonCount] = button4;

  // Encoder 1
  auto encoder1 = new OneRotaryEncoder(ROTARY_ENCODER_1_A_PIN, ROTARY_ENCODER_1_B_PIN, 64, 0, 127);
  encoders[++encoderCount] = encoder1;

  // Encoder 2
  auto encoder2 = new OneRotaryEncoder(ROTARY_ENCODER_2_A_PIN, ROTARY_ENCODER_2_B_PIN, 64, 0, 127);
  encoders[++encoderCount] = encoder2;

  // Encoder 3
  auto encoder3 = new OneRotaryEncoder(ROTARY_ENCODER_3_A_PIN, ROTARY_ENCODER_3_B_PIN, 64, 0, 127);
  encoders[++encoderCount] = encoder3;

  // Encoder 4 - initial value 0
  auto encoder4 = new OneRotaryEncoder(ROTARY_ENCODER_4_A_PIN, ROTARY_ENCODER_4_B_PIN, 0, 0, 127);
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

void AudioControls::UpdateDynamicEncoder(int encoder)
{
  int i = encoder - 1;
  // Update the encoder value based on the current midi state
  switch (midiState)
  {
  case AudioControlMode::ModeGlobal:
    if (i != 3) return; // To keep volume controls, only the last encoder can change the global program
    encoderGlobalProgram[i] = SelectNextProgram(encoderGlobalProgram[i], globalPrograms, sizeof(globalPrograms) / sizeof(int));
    sprintf(textBuffer, "%s %s", GetMidiChannelName(GLOBAL_MIDI_CHAN), GetMidiControlName(encoderGlobalProgram[i]));
    display.WriteText(textBuffer);
    break;
  case AudioControlMode::ModeDrum:
    encoderDrumProgram[i] = SelectNextProgram(encoderDrumProgram[i], drum808Programs, sizeof(drum808Programs) / sizeof(int));
    sprintf(textBuffer, "%s %s", GetMidiChannelName(DRUM_MIDI_CHAN), GetMidiControlName(encoderDrumProgram[i]));
    display.WriteText(textBuffer);
    break;
  case AudioControlMode::ModeSynth1:
    encoderSynth1Program[i] = SelectNextProgram(encoderSynth1Program[i], synth303Programs, sizeof(synth303Programs) / sizeof(int));
    sprintf(textBuffer, "%s %s", GetMidiChannelName(SYNTH1_MIDI_CHAN), GetMidiControlName(encoderSynth1Program[i]));
    display.WriteText(textBuffer);
    break;    
  case AudioControlMode::ModeSynth2:
    encoderSynth2Program[i] = SelectNextProgram(encoderSynth2Program[i], synth303Programs, sizeof(synth303Programs) / sizeof(int));
    sprintf(textBuffer, "%s %s", GetMidiChannelName(SYNTH2_MIDI_CHAN), GetMidiControlName(encoderSynth2Program[i]));
    display.WriteText(textBuffer);
    break;
  }
}

int AudioControls::SelectNextProgram(int currentProgram, const int *programs, int programCount)
{  
  for (int i = 0; i < programCount; i++)
  {
    if (currentProgram == programs[i])
    {
      return programs[(i + 1) % programCount];
    }
  }
  return programs[0];
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
  case AudioControlMode::ModeGlobal:
    if (encoderPos1.hasNewPosition) SendControlChange(encoderGlobalProgram[0], encoderPos1.position, DRUM_MIDI_CHAN);
    if (encoderPos2.hasNewPosition) SendControlChange(encoderGlobalProgram[1], encoderPos2.position, SYNTH1_MIDI_CHAN);
    if (encoderPos3.hasNewPosition) SendControlChange(encoderGlobalProgram[2], encoderPos3.position, SYNTH2_MIDI_CHAN);
    if (encoderPos4.hasNewPosition) SendControlChange(encoderGlobalProgram[3], encoderPos4.position, GLOBAL_MIDI_CHAN);
    break;
  case AudioControlMode::ModeDrum:
    if (encoderPos1.hasNewPosition) SendControlChange(encoderDrumProgram[0], encoderPos1.position, DRUM_MIDI_CHAN);
    if (encoderPos2.hasNewPosition) SendControlChange(encoderDrumProgram[1], encoderPos2.position, DRUM_MIDI_CHAN);
    if (encoderPos3.hasNewPosition) SendControlChange(encoderDrumProgram[2], encoderPos3.position, DRUM_MIDI_CHAN);
    if (encoderPos4.hasNewPosition) SendControlChange(encoderDrumProgram[3], encoderPos4.position, DRUM_MIDI_CHAN);
    break;
  case AudioControlMode::ModeSynth1:
    if (encoderPos1.hasNewPosition) SendControlChange(encoderSynth1Program[0], encoderPos1.position, SYNTH1_MIDI_CHAN);
    if (encoderPos2.hasNewPosition) SendControlChange(encoderSynth1Program[1], encoderPos2.position, SYNTH1_MIDI_CHAN);
    if (encoderPos3.hasNewPosition) SendControlChange(encoderSynth1Program[2], encoderPos3.position, SYNTH1_MIDI_CHAN);
    if (encoderPos4.hasNewPosition) SendControlChange(encoderSynth1Program[3], encoderPos4.position, SYNTH1_MIDI_CHAN);
    break;
  case AudioControlMode::ModeSynth2:
    if (encoderPos1.hasNewPosition) SendControlChange(encoderSynth2Program[0], encoderPos1.position, SYNTH2_MIDI_CHAN);
    if (encoderPos2.hasNewPosition) SendControlChange(encoderSynth2Program[1], encoderPos2.position, SYNTH2_MIDI_CHAN);
    if (encoderPos3.hasNewPosition) SendControlChange(encoderSynth2Program[2], encoderPos3.position, SYNTH2_MIDI_CHAN);
    if (encoderPos4.hasNewPosition) SendControlChange(encoderSynth2Program[3], encoderPos4.position, SYNTH2_MIDI_CHAN);
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
    newMidiState = AudioControlMode::ModeSynth2;
  }
  else if (leftSwitchState)
  {
    newMidiState = AudioControlMode::ModeSynth1;
  }
  else if (rightSwitchState)
  {
    newMidiState = AudioControlMode::ModeGlobal;
  }
  else
  {
    newMidiState = AudioControlMode::ModeDrum;
  }

  if (newMidiState != midiState)
  {    
    display.WriteText(GetMidiChannelName(newMidiState));
#ifndef ENABLE_MIDI
    Serial.print("Midi state: ");
    Serial.print(newMidiState);
    Serial.println();
#endif
  }
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
    return "Drum";
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
    return "Env mod lvl";
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
    return "Bass tone";
  case CC_808_BD_DECAY:
    return "Bass decay";
  case CC_808_BD_LEVEL:
    return "Bass level";
  case CC_808_SD_TONE:
    return "Snare tone";
  case CC_808_SD_SNAP:
    return "Snare snap";
  case CC_808_SD_LEVEL:
    return "Snare level";
  case CC_808_CH_TONE:
    return "Cl hat tone";
  case CC_808_CH_LEVEL:
    return "Cl hat lvl";
  case CC_808_OH_TONE:
    return "Op hat tone";
  case CC_808_OH_DECAY:
    return "Op hat dcy";
  case CC_808_OH_LEVEL:
    return "Op hat lvl";

  case CC_ANY_COMPRESSOR:
    return "Compressor";
  case CC_ANY_DELAY_TIME:
    return "Delay";
  case CC_ANY_DELAY_FB:
    return "Delay FB";
  case CC_ANY_DELAY_LVL:
    return "Delay lvl";
  case CC_ANY_REVERB_TIME:
    return "Reverb Time";
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
