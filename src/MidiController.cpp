#include "MidiController.h"
#include <MIDI.h>
#include "constants.h"
#include "AcidBoxMidi.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

MidiController::MidiController()
{
  // Set switch pins to digital input
  pinMode(LEFT_SWITCH_PIN, INPUT);
  pinMode(RIGHT_SWITCH_PIN, INPUT);

  // Setup encoders and buttons
  int buttonCount = -1;
  int encoderCount = -1;

  // REWIND button
  // auto button1 = new OneButton(REWIND_BUTTON_PIN, true);
  // button1-> attachClick([]() {
  //   SendControlChange(CC_808_VOLUME, 0, DRUM_MIDI_CHAN);
  // });
  // buttons[++buttonCount] = button1;

  // // FAST FORWARD button
  // auto button2 = new OneButton(FAST_FORWARD_BUTTON_PIN, true);
  // button2-> attachClick([]() {
  //   SendControlChange(CC_303_VOLUME, 0, SYNTH1_MIDI_CHAN);
  // });
  // buttons[++buttonCount] = button2;

  // // PLAY button
  // auto button3 = new OneButton(PLAY_BUTTON_PIN, true);
  // button3-> attachClick([]() {
  //   SendControlChange(CC_303_VOLUME, 0, SYNTH2_MIDI_CHAN);
  // });
  // buttons[++buttonCount] = button3;

  // // STOP button
  // auto button4 = new OneButton(STOP_BUTTON_PIN, true);
  // button4-> attachClick([]() {
  //   SendControlChange(CC_303_VOLUME, 0, SYNTH2_MIDI_CHAN);
  // });
  // buttons[++buttonCount] = button4;

  // Encoder 1
  auto encoder1 = new OneRotaryEncoder(255, ROTARY_ENCODER_1_A_PIN, ROTARY_ENCODER_1_B_PIN, ROTARY_ENCODER_1_SW_PIN);
  // encoder1->AttachRotate([](int pos) {
  //   SendControlChange(CC_808_VOLUME, pos, DRUM_MIDI_CHAN);
  // });
  // encoder1->AttachClick([]() {
  //   SendControlChange(CC_808_VOLUME, 0, DRUM_MIDI_CHAN);
  // });
  // encoder1->AttachLongPressStart([]() {
  //   SendControlChange(CC_808_VOLUME, 255, DRUM_MIDI_CHAN);
  // });
  encoders[++encoderCount] = encoder1;

  // Encoder 2
  auto encoder2 = new OneRotaryEncoder(255, ROTARY_ENCODER_2_A_PIN, ROTARY_ENCODER_2_B_PIN, ROTARY_ENCODER_2_SW_PIN);
  // encoder2->AttachRotate([](int pos) {
  //   SendControlChange(CC_303_VOLUME, pos, SYNTH1_MIDI_CHAN);
  // });
  // encoder2->AttachClick([]() {
  //   SendControlChange(CC_303_VOLUME, 0, SYNTH1_MIDI_CHAN);
  // });
  // encoder2->AttachLongPressStart([]() {
  //   SendControlChange(CC_303_VOLUME, 255, SYNTH1_MIDI_CHAN);
  // });
  encoders[++encoderCount] = encoder2;

  // Encoder 3
  auto encoder3 = new OneRotaryEncoder(255, ROTARY_ENCODER_3_A_PIN, ROTARY_ENCODER_3_B_PIN, ROTARY_ENCODER_3_SW_PIN);
  // encoder3->AttachRotate([](int pos) {
  //   SendControlChange(CC_303_VOLUME, pos, SYNTH2_MIDI_CHAN);
  // });
  // encoder3->AttachClick([]() {
  //   SendControlChange(CC_303_VOLUME, 0, SYNTH2_MIDI_CHAN);
  // });
  // encoder3->AttachLongPressStart([]() {
  //   SendControlChange(CC_303_VOLUME, 255, SYNTH2_MIDI_CHAN);
  // });
  encoders[++encoderCount] = encoder3;

  // Encoder 4
  auto encoder4 = new OneRotaryEncoder(255, ROTARY_ENCODER_4_A_PIN, ROTARY_ENCODER_4_B_PIN, ROTARY_ENCODER_4_SW_PIN);
  // encoder4->AttachRotate([](int pos) {    
  //   SendControlChange(CC_ANY_REVERB_TIME, pos, SYNTH1_MIDI_CHAN);
  // });
  // encoder4->AttachClick([]() {
  //   SendControlChange(CC_ANY_REVERB_TIME, 0, SYNTH1_MIDI_CHAN);
  // });
  // encoder4->AttachLongPressStart([]() {
  //   SendControlChange(CC_ANY_REVERB_TIME, 255, SYNTH1_MIDI_CHAN);
  // });
  encoders[++encoderCount] = encoder4;
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
    if (buttons[i] != NULL) buttons[i]->tick();
    if (encoders[i] != NULL) encoders[i]->Tick();
  }

  ProcessAudioControl();
}

void MidiController::ProcessAudioControl()
{
  MidiControlState midiState = ReadSwitchStates();

  auto encoderSwitch1 = encoders[0]->GetSwitchState();
  auto encoderSwitch2 = encoders[1]->GetSwitchState();
  auto encoderSwitch3 = encoders[2]->GetSwitchState();
  auto encoderSwitch4 = encoders[3]->GetSwitchState();

  auto encoderPos1 = encoders[0]->GetPosition();
  auto encoderPos2 = encoders[1]->GetPosition();

  switch (midiState)
  {
  case MidiControlState::Control0:
    if (encoderSwitch1 == EncoderSwitchState::Clicked)
    {
      SendControlChange(CC_808_VOLUME, 0, DRUM_MIDI_CHAN);
    }
    else if (encoderSwitch1 == EncoderSwitchState::LongPressed)
    {
      SendControlChange(CC_303_VOLUME, 255, SYNTH1_MIDI_CHAN);
    }

    if (encoderPos1 != -1)
    {
      SendControlChange(CC_808_VOLUME, encoderPos1, DRUM_MIDI_CHAN);
    }
    if (encoderPos2 != -1)  
    {
      SendControlChange(CC_303_VOLUME, encoderPos2, SYNTH1_MIDI_CHAN);
    }
    break;
  case MidiControlState::Control1:
    break;
  }
}

MidiControlState MidiController::ReadSwitchStates()
{
  bool leftSwitchState = digitalRead(LEFT_SWITCH_PIN) == HIGH;
  bool rightSwitchState = digitalRead(RIGHT_SWITCH_PIN) == HIGH;

  if (leftSwitchState && rightSwitchState)
  {
    return MidiControlState::Control3;
  }
  else if (leftSwitchState)
  {
    return MidiControlState::Control1;
  }
  else if (rightSwitchState)
  {
    return MidiControlState::Control2;
  }
  else
  {
    return MidiControlState::Control0;
  }
}

void MidiController::SendProgramChange(int program, int channel)
{
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

void MidiController::SendControlChange(uint8_t program, uint8_t value, uint8_t channel)
{
  // Midi values are 0-127. Rotary encoder values are 0-255.
  int midiValue = map(value, 0, 255, 0, 127);

#ifdef ENABLE_MIDI
  MIDI.sendControlChange(number, midiValue, channel);
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

