#ifndef DeckLight_h
#define DeckLight_h

#include "devices.h"
#include <OneButton.h>
#include <FastLED_NeoMatrix.h>

class DeckLight
{
public:
  DeckLight();
  void setup();
  void tick();
  void displayAudio(int bandValues[]);

private:
  void updateBarVuMeter(int bandValues[]);
  void updateAnalogVuMeter();
  void rainbowBars(int band, int barHeight);
  void purpleBars(int band, int barHeight);
  void changingBars(int band, int barHeight);
  void centerBars(int band, int barHeight);
  void whitePeak(int band);
  void outrunPeak(int band);
  void waterfall(int bandValues[], int band);
  
  static void changeTheme();
  static void startAutoMode();  
  static void brightnessOff();
  static void changeBrightness();

  int amplitude = 27000;
  OneButton modeBtn;
  FastLED_NeoMatrix* matrix;

  static int themeIndex;
  static bool autoChangePatterns;
};

#endif