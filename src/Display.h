#pragma once

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "constants.h"

class Display 
{
public:
    Display();
    void Setup();
    void WriteText(const char* text);
    void Clear();

private:
    Adafruit_SSD1306 display;
    bool initialised;
    const int Width = 128;
    const int Height = 64;
};