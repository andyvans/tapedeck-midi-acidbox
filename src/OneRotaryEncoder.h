#pragma once
#include <functional> // For std::function
#include <OneButton.h>
#include <RotaryEncoder.h>

class OneRotaryEncoder {
public:
    
    OneRotaryEncoder(int pinA, int pinB, int pinSwitch);
    void AttachRotate(std::function<void(bool, int)> encoderCallback);
    void AttachClick(callbackFunction switchCallback);
    void AttachClickWithState(std::function<void(bool, int)> clickWithStateCallback);
    void AttachLongPressStart(callbackFunction switchCallback);
    void Tick();

private:
    OneButton button;
    RotaryEncoder encoder;
    int CalculateAcceleration(int newPos);
    std::function<void(bool, int)> encoderCallback;
    std::function<void(bool, int)> clickWithStateCallback;

    static void ButtonStateChange(void* pEncoder);

    int lastPosition;
    bool buttonState;
};