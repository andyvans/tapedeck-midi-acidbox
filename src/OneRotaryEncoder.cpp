#include "OneRotaryEncoder.h"

OneRotaryEncoder::OneRotaryEncoder(int startValue, int pinA, int pinB, int pinSwitch)
    : encoder(pinA, pinB, RotaryEncoder::LatchMode::TWO03), button(pinSwitch)
{
  //button.attachClick([](void *scope) { ((OneRotaryEncoder *) scope)->Clicked();}, this);
  //button.attachDoubleClick([](void *scope) { ((OneRotaryEncoder *) scope)->DoubleClicked();}, this);
  //button.attachLongPressStart([](void *scope) { ((OneRotaryEncoder *) scope)->LongPressed();}, this);
  //encoder.setPosition(startValue);
}

void OneRotaryEncoder::Clicked()
{
  switchState = EncoderSwitchPress::Clicked;
  hasNewSwitchState = true;
}

void OneRotaryEncoder::DoubleClicked()
{
  switchState = EncoderSwitchPress::DoubleClicked;
  hasNewSwitchState = true;
}

void OneRotaryEncoder::LongPressed()
{
  switchState = EncoderSwitchPress::LongPressed;
  hasNewSwitchState = true;
}

EncoderPositionState OneRotaryEncoder::GetPosition()
{
  EncoderPositionState state;
  state.position = lastPosition;
  state.hasNewPosition = hasNewPosition;
  hasNewPosition = false;
  return state;  
}

EncoderSwitchState OneRotaryEncoder::GetSwitchState()
{
  EncoderSwitchState state;
  state.state = switchState;
  state.hasNewState = hasNewSwitchState;
  hasNewSwitchState = false;
  return state;
}

void OneRotaryEncoder::Tick()
{
  encoder.tick();
  button.tick();

  long newPosition = encoder.getPosition() * 4;

  if (newPosition < 0)
  {
    encoder.setPosition(0);
    newPosition = 0;
  }

  if (newPosition > 128)
  {
    encoder.setPosition(128);
    newPosition = 128;
  }

  
  if (lastPosition != newPosition)
  {
    lastPosition = newPosition;     //CalculateAcceleration(lastPosition, newPosition);
    //encoder.setPosition(lastPosition);
#ifndef ENABLE_MIDI
    Serial.print("Encoder: ");
    Serial.println(lastPosition);
#endif
    hasNewPosition = true;
  }
}

int OneRotaryEncoder::CalculateAcceleration(int lastPos, int newPos)
{
  // Accelerate when there was a previous rotation in the same direction.
  // The maximum acceleration is 10 times.
  constexpr float m = 10;

  // At 200ms or slower, there should be no acceleration. (factor 1)
  constexpr float longCutoff = 50;

  // At 5 ms, we want to have maximum acceleration (factor m)
  constexpr float shortCutoff = 5;

  // To derive the calc. constants, compute as follows:
  // On an x(ms) - y(factor) plane resolve a linear formula factor(ms) = a * ms + b;
  // where  f(4)=10 and f(200)=1

  constexpr float a = (m - 1) / (shortCutoff - longCutoff);
  constexpr float b = 1 - longCutoff * a;
  unsigned long ms = encoder.getMillisBetweenRotations();

  if (ms < longCutoff)
  {
    // do some acceleration using factors a and b
    // limit to maximum acceleration
    if (ms < shortCutoff)
    {
      ms = shortCutoff;
    }

    float ticksActual_float = a * ms + b;
    long deltaTicks = (long)ticksActual_float * (newPos - lastPos);
    newPos = newPos + deltaTicks;
    encoder.setPosition(newPos);
  }
  return newPos;
}