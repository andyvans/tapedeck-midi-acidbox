#include "OneRotaryEncoder.h"

OneRotaryEncoder::OneRotaryEncoder(int pinA, int pinB, int pinSwitch)
    : encoder(pinA, pinB), button(pinSwitch)
{
}

void OneRotaryEncoder::AttachRotate(std::function<void(int)> callback)
{
  encoderCallback = callback;
}

void OneRotaryEncoder::AttachClick(callbackFunction callback)
{
  button.attachClick(callback);
}

void OneRotaryEncoder::AttachClickWithState(std::function<void(bool, int)> callback)
{
  clickWithStateCallback = callback;
  button.attachClick(ButtonStateChange, this);
}

void OneRotaryEncoder::ButtonStateChange(void *pEncoder)
{
  OneRotaryEncoder *encoder = (OneRotaryEncoder *)pEncoder;
  encoder->buttonState = !encoder->buttonState;
  encoder->clickWithStateCallback(encoder->buttonState, encoder->lastPosition);
}

void OneRotaryEncoder::AttachLongPressStart(callbackFunction callback)
{
  button.attachLongPressStart(callback);
}

void OneRotaryEncoder::Tick()
{
  encoder.tick();
  button.tick();

  int newPos = encoder.getPosition();
  if (lastPosition != newPos)
  {
    lastPosition = CalculateAcceleration(newPos);
    if (encoderCallback)
    {
      encoderCallback(newPos);
    }
  }
}

int OneRotaryEncoder::CalculateAcceleration(int newPos)
{
  // the maximum acceleration is 10 times.
  constexpr float m = 10;

  // at 200ms or slower, there should be no acceleration. (factor 1)
  constexpr float longCutoff = 50;

  // at 5 ms, we want to have maximum acceleration (factor m)
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
    long deltaTicks = (long)ticksActual_float * (newPos - lastPosition);

    newPos = newPos + deltaTicks;
    encoder.setPosition(newPos);
  }
  return newPos;
}