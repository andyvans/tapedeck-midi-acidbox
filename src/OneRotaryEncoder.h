#pragma once
#include <functional> // For std::function
#include <OneButton.h>
#include <RotaryEncoder.h>

enum EncoderSwitchState {
    None,
    Clicked,
    DoubleClicked,
    LongPressed
};

class OneRotaryEncoder {
public:
    
    OneRotaryEncoder(int startValue, int pinA, int pinB, int pinSwitch);
    void Tick();
    int GetPosition();
    EncoderSwitchState GetSwitchState();

private:
    OneButton button;
    RotaryEncoder encoder;

    int lastPosition = -1;
    EncoderSwitchState switchState = EncoderSwitchState::None;

    void Clicked();
    void DoubleClicked();
    void LongPressed();
    int CalculateAcceleration(int oldPos, int newPos);
};
