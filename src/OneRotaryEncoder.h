#pragma once
#include <functional> // For std::function
#include <OneButton.h>
#include <RotaryEncoder.h>

class OneRotaryEncoder {
public:
    
    OneRotaryEncoder(int startValue, int pinA, int pinB, int pinSwitch);
    void AttachRotate(std::function<void(int)> encoderCallback);
    void AttachClick(callbackFunction switchCallback);
    void AttachClickWithState(std::function<void(int)> clickWithStateCallback);
    void AttachLongPressStart(callbackFunction switchCallback);
    void Tick();
    int GetPosition() { return lastPosition; }

private:
    OneButton button;
    RotaryEncoder encoder;
    int CalculateAcceleration(int oldPos, int newPos);
    std::function<void(int)> encoderCallback;
    std::function<void(int)> clickWithStateCallback;

    static void ButtonStateChange(void* pEncoder);

    int lastPosition;
};