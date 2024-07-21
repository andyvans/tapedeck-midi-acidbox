#pragma once

#include "constants.h"
#include <OneButton.h>
#include <FastLED_NeoMatrix.h>

#define DeckLightLedCount 48
#define DeckLightMatrixWidth 4
#define DeckLightMatrixHeight 12
#define DeckVuLedCount 5

class DeckLight
{
public:
  DeckLight();
  void Setup();
  void Tick();
  void DisplayAudio(int bandValues[]);

private:
  void UpdateBarVuMeter(int bandValues[]);
  void UpdateAnalogVuMeter();
  void RainbowBars(int band, int barHeight);
  void PurpleBars(int band, int barHeight);
  void ChangingBars(int band, int barHeight);
  void CenterBars(int band, int barHeight);
  void WhitePeak(int band);
  void OutrunPeak(int band);
  void Waterfall(int bandValues[], int band);
  
  static void ChangeTheme();
  static void StartAutoMode();  
  static void BrightnessOff();
  static void ChangeBrightness();

  int amplitude = 27000;
  OneButton modeBtn;

  // FastLed globals
  static const int BrightnessSettings[];
  static CRGBPalette16 purplePalette;
  static CRGBPalette16 outRunPalette;
  static CRGBPalette16 greenBluePalette;
  static CRGBPalette16 heatPalette;

  // RGB LED matrix
  FastLED_NeoMatrix* matrix;
  CRGB matrixLeds[DeckLightLedCount];
  static const int BarWidth =  (DeckLightMatrixWidth / (NUM_BANDS - 1));
  int oldBarHeights[NUM_BANDS] = {0};
  byte peak[NUM_BANDS] = {0};
  uint8_t colorTimer = 0;

  // RGB LEDs on deck and VU meters
  CRGB vuLeds[DeckVuLedCount];
  int activeDeckLed = 0;

  // Analog vu meter variables
  const int AnalogVUPeakScalar = 1.2;
  const int AnalogVUMax = 170;
  const int AnalogVUVariance = 10;
  
  // Theme settings
  static int themeIndex;
  static bool autoChangePatterns;
};