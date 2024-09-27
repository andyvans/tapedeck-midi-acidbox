#pragma once

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "constants.h"

class Display 
{
public:
    Display();
    void Setup();
    void Tick();
    void WriteText(const char* text);
    void Clear();

private:
    void RenderSplash();
    void RenderMessages();
    void RenderScreenSaver();

    Adafruit_SSD1306 display;
    bool initialised;
    const int Width = 128;
    const int Height = 64;
    
    char line1[20];
    char line2[20];
    char line3[20];
    char line4[20];
    ulong lastMessageUpdate;
    bool renderText;

    const int TextTimeout = 10000;

    // Animation
    int currentIteration = 0;
    int lastRenderTime = 0;
};