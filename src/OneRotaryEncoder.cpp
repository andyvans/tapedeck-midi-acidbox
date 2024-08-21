#include "OneRotaryEncoder.h"

OneRotaryEncoder::OneRotaryEncoder(int startValue, int pinA, int pinB, int pinSwitch)
    : encoder(pinA, pinB), button(pinSwitch)
{
  encoder.setPosition(startValue);
}

void OneRotaryEncoder::AttachRotate(std::function<void(int)> callback)
{
  encoderCallback = callback;
}

void OneRotaryEncoder::AttachClick(callbackFunction callback)
{
  button.attachClick(callback);
}

void OneRotaryEncoder::AttachLongPressStart(callbackFunction callback)
{
  button.attachLongPressStart(callback);
}

void OneRotaryEncoder::AttachClickWithState(std::function<void(int)> callback)
{
  clickWithStateCallback = callback;
  button.attachClick(ButtonStateChange, this);
}

void OneRotaryEncoder::ButtonStateChange(void *pEncoder)
{
  OneRotaryEncoder *encoder = (OneRotaryEncoder *)pEncoder;
  encoder->clickWithStateCallback(encoder->lastPosition);
}

void OneRotaryEncoder::Tick()
{
  encoder.tick();
  int newPosition = encoder.getPosition();
  if (lastPosition != newPosition)
  {
    lastPosition = newPosition; // CalculateAcceleration(lastPosition, newPosition);
    if (encoderCallback != nullptr)
    {
      encoderCallback(lastPosition);
    }
  }
  button.tick();
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