#include <Arduino.h>
#include <FastLED_NeoMatrix.h>
#include <arduinoFFT.h>
#include <EasyButton.h>

// Audio constants
#define AUDIO_IN_PIN 35                            // Signal in on this pin
#define SAMPLES 1024                               // Must be a power of 2
#define SAMPLING_FREQ 40000                        // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AMPLITUDE 15000                            // Depending on your audio source level, you may need to alter this value. Can be used as a 'sensitivity' control.

// LED matrix
#define LED_PIN 5                                  // LED strip data
#define COLOR_ORDER GRB                            // If colours look wrong, play with this
#define CHIPSET WS2812B                            // LED strip type
#define MAX_MILLIAMPS 1000                         // Careful with the amount of power here if running off USB port
const int BRIGHTNESS_SETTINGS[3] = {5, 70, 200};   // 3 Integer array for 3 brightness settings (based on pressing+holding BTN_PIN)
#define LED_VOLTS 5                                // Usually 5 or 12
#define NUM_BANDS 4                                // To change this, you will need to change the bunch of if statements describing the mapping from bins to bands
#define NOISE 500                                  // Used as a crude noise filter, values below this are ignored
const uint8_t kMatrixWidth = 4;                    // Matrix width
const uint8_t kMatrixHeight = 12;                   // Matrix height
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)    // Total number of LEDs
#define BAR_WIDTH (kMatrixWidth / (NUM_BANDS - 1)) // If width >= 8 light 1 LED width per bar, >= 16 light 2 LEDs width bar etc
#define TOP (kMatrixHeight - 0)                    // Don't allow the bars to go offscreen
#define SERPENTINE true                            // Set to false if you're LEDS are connected end to end, true if serpentine

// Button
#define BTN_PIN 4                                  // Connect a push button to this pin to change patterns
#define LONG_PRESS_MS 200                          // Number of ms to count as a long press

// FFT audio processing
unsigned int sampling_period_us;
byte peak[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // The length of these arrays must be >= NUM_BANDS
int oldBarHeights[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int bandValues[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float vReal[SAMPLES];
float vImag[SAMPLES];
unsigned long newTime;
ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, SAMPLES, SAMPLING_FREQ);

// Button variables
int buttonPushCounter = 0;
bool autoChangePatterns = false;
EasyButton modeBtn(BTN_PIN);

// Analog vu meter variables
#define ANALOG_VU_METER_LEFT_PIN 25
#define ANALOG_VU_METER_RIGHT_PIN 26
#define VU_PEAK_SCALAR 1.2
#define VU_MAX 160

// RGB LEDs on deck
#define deckLedCount 5
#define DECK_LED_PIN 27
int activeDeckLed = 0;

CRGB vuLeds[deckLedCount];

// FastLED variables
CRGB leds[NUM_LEDS];
DEFINE_GRADIENT_PALETTE(purple_gp){
    0, 0, 212, 255,    // blue
    255, 179, 0, 255}; // purple
DEFINE_GRADIENT_PALETTE(outrun_gp){
    0, 141, 0, 100,   // purple
    127, 255, 192, 0, // yellow
    255, 0, 5, 255};  // blue
DEFINE_GRADIENT_PALETTE(greenblue_gp){
    0, 0, 255, 60,    // green
    64, 0, 236, 255,  // cyan
    128, 0, 5, 255,   // blue
    192, 0, 236, 255, // cyan
    255, 0, 255, 60}; // green
DEFINE_GRADIENT_PALETTE(redyellow_gp){
    0, 200, 200, 200,    // white
    64, 255, 218, 0,     // yellow
    128, 231, 0, 0,      // red
    192, 255, 218, 0,    // yellow
    255, 200, 200, 200}; // white

CRGBPalette16 purplePal = purple_gp;
CRGBPalette16 outrunPal = outrun_gp;
CRGBPalette16 greenbluePal = greenblue_gp;
CRGBPalette16 heatPal = redyellow_gp;
uint8_t colorTimer = 0;

// FastLED_NeoMatrix - see https://github.com/marcmerlin/FastLED_NeoMatrix
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(
  leds, kMatrixWidth, kMatrixHeight,
  NEO_MATRIX_TOP  + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG +
  NEO_TILE_TOP    + NEO_TILE_LEFT + NEO_TILE_ROWS);

// Function declarations
void rainbowBars(int band, int barHeight);
void purpleBars(int band, int barHeight);
void changingBars(int band, int barHeight);
void centerBars(int band, int barHeight);
void whitePeak(int band);
void outrunPeak(int band);
void waterfall(int band);
void changeMode();
void startAutoMode();
void brightnessButton();
void brightnessOff();

void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setMaxPowerInVoltsAndMilliamps(LED_VOLTS, MAX_MILLIAMPS);
  FastLED.setBrightness(BRIGHTNESS_SETTINGS[1]);
  FastLED.clear();

  FastLED.addLeds<CHIPSET, DECK_LED_PIN, COLOR_ORDER>(vuLeds , deckLedCount).setCorrection(TypicalSMD5050);
  
  modeBtn.begin();
  modeBtn.onPressed(changeMode);
  modeBtn.onPressedFor(LONG_PRESS_MS, brightnessButton);
  modeBtn.onSequence(3, 2000, startAutoMode);
  modeBtn.onSequence(5, 2000, brightnessOff);
  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));
}

void loop()
{
  // Don't clear screen if waterfall pattern, be sure to change this is you change the patterns / order
  if (buttonPushCounter != 5) FastLED.clear();

  modeBtn.read();

  // Sample the audio pin
  for (int i = 0; i < SAMPLES; i++)
  {
    newTime = micros();
    vReal[i] = analogRead(AUDIO_IN_PIN); // A conversion takes about 9.7uS on an ESP32
    vImag[i] = 0;
    while ((micros() - newTime) < sampling_period_us)
    { 
      /* chill */
    }
  }

  analyseAudio();
  updateBarVuMeter();
  updateAnalogVuMeter();

  // Decay peak
  EVERY_N_MILLISECONDS(60)
  {
    for (byte band = 0; band < NUM_BANDS; band++)
    {
      if (peak[band] > 0) peak[band] -= 1;
    }
    colorTimer++;
  }

  // Used in some of the patterns
  EVERY_N_MILLISECONDS(10)
  {
    colorTimer++;
  }

  EVERY_N_SECONDS(10)
  {
    if (autoChangePatterns)
      buttonPushCounter = (buttonPushCounter + 1) % 6;
  }

  FastLED.show();
}

void analyseAudio()
{
  // Reset bandValues[]
  for (int i = 0; i < NUM_BANDS; i++)
  {
    bandValues[i] = 0;
  }
  
  // Compute FFT
  FFT.dcRemoval();
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward); // Weigh data
  FFT.compute(FFTDirection::Forward);                       // Compute FFT
  FFT.complexToMagnitude();                                 // Compute magnitudes

  // Analyse FFT results
  for (int i = 2; i < (SAMPLES / 2); i++)
  {
    // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
    if (vReal[i] > NOISE)
    {
      // 4 bands
      if (i <= 13)
        bandValues[0] += (int)vReal[i];
      if (i > 13 && i <= 27)
        bandValues[1] += (int)vReal[i];
      if (i > 27 && i <= 55)
        bandValues[2] += (int)vReal[i];
      if (i > 55 && i <= 255)
        bandValues[3] += (int)vReal[i]*0.8;
    }
  }
}

void updateBarVuMeter()
{
  // Process the FFT data into bar heights
  for (byte band = 0; band < NUM_BANDS; band++)
  {
    // Scale the bars for the display
    int barHeight = bandValues[band] / AMPLITUDE;

    // Small amount of averaging between frames
    barHeight = ((oldBarHeights[band] * 1) + barHeight) / 2;

    if (barHeight > TOP)
      barHeight = TOP;

    // Move peak up
    if (barHeight > peak[band])
    {
      peak[band] = min(TOP, barHeight);
    }

    // Draw bars
    switch (buttonPushCounter)
    {
    case 0:
      rainbowBars(band, barHeight);
      break;
    case 1:
      // No bars on this one
      break;
    case 2:
      purpleBars(band, barHeight);
      break;
    case 3:
      centerBars(band, barHeight);
      break;
    case 4:
      changingBars(band, barHeight);
      break;
    case 5:
      waterfall(band);
      break;
    }

    // Draw peaks
    switch (buttonPushCounter)
    {
    case 0:
      whitePeak(band);
      break;
    case 1:
      outrunPeak(band);
      break;
    case 2:
      whitePeak(band);
      break;
    case 3:
      // No peaks
      break;
    case 4:
      // No peaks
      break;
    case 5:
      // No peaks
      break;
    }

    // Save oldBarHeights for averaging later
    oldBarHeights[band] = barHeight;
  }
}

void updateAnalogVuMeter()
{
  // Update analog vu meter. Average the peaks and map the output to the meter
  int vuPeak = 0;
  for (byte band = 0; band < NUM_BANDS; band++)
    vuPeak += oldBarHeights[band];
  vuPeak = (vuPeak / NUM_BANDS) * VU_PEAK_SCALAR;
  int vuUnits = map(vuPeak, 0, kMatrixHeight, 0, 255);
  // Limit the output to protect the analog VU meters
  vuUnits = min(vuUnits, VU_MAX);
  analogWrite(ANALOG_VU_METER_LEFT_PIN, vuUnits);
  analogWrite(ANALOG_VU_METER_RIGHT_PIN, vuUnits);

  // Update the desk leds    
  switch (buttonPushCounter)
  {
  case 0:
    vuLeds[0] = CHSV(vuUnits, 255, 255);
    vuLeds[1] = CHSV(vuUnits, 255, 255);
    vuLeds[2] = CHSV(vuUnits, 255, 255);
    vuLeds[3] = CHSV(vuUnits, 255, 255);
    vuLeds[4] = CHSV(vuUnits, 255, 255);
    break;
  case 1:
    vuLeds[0] = ColorFromPalette(outrunPal, vuUnits);
    vuLeds[1] = ColorFromPalette(outrunPal, vuUnits);
    vuLeds[2] = ColorFromPalette(outrunPal, vuUnits);
    vuLeds[3] = ColorFromPalette(outrunPal, vuUnits);
    vuLeds[4] = ColorFromPalette(outrunPal, vuUnits);
    break;
  case 2:
    vuLeds[0] = ColorFromPalette(purplePal, vuUnits);
    vuLeds[1] = ColorFromPalette(purplePal, vuUnits);
    vuLeds[2] = ColorFromPalette(purplePal, vuUnits);
    vuLeds[3] = ColorFromPalette(purplePal, vuUnits);
    vuLeds[4] = ColorFromPalette(purplePal, vuUnits);
    break;
  case 3:
    vuLeds[0] = ColorFromPalette(heatPal, vuUnits);
    vuLeds[1] = ColorFromPalette(heatPal, vuUnits);
    vuLeds[2] = ColorFromPalette(heatPal, vuUnits);
    vuLeds[3] = ColorFromPalette(heatPal, vuUnits);
    vuLeds[4] = ColorFromPalette(heatPal, vuUnits);
    break;
  case 4:
    vuLeds[0] = CHSV(vuUnits, 255, 255);
    vuLeds[1] = CHSV(vuUnits, 255, 255);
    vuLeds[2] = CHSV(vuUnits, 255, 255);
    vuLeds[3] = CHSV(vuUnits, 255, 255);
    vuLeds[4] = CHSV(vuUnits, 255, 255);
    break;
  case 5:
    vuLeds[0] = CHSV(vuUnits, 255, 255);
    vuLeds[1] = CHSV(vuUnits, 255, 255);
    vuLeds[2] = CHSV(vuUnits, 255, 255);
    vuLeds[3] = CHSV(vuUnits, 255, 255);
    vuLeds[4] = CHSV(vuUnits, 255, 255);
    break;
  }

  EVERY_N_MILLISECONDS(500)
  {
    activeDeckLed = (activeDeckLed + 1) % 3;
  }
  
  // Draw the active deck led as white
  vuLeds[activeDeckLed] = CRGB::White;
}

void changeMode()
{
  Serial.println("Button pressed");
  if (FastLED.getBrightness() == 0)
    FastLED.setBrightness(BRIGHTNESS_SETTINGS[0]); // Re-enable if lights are "off"
  autoChangePatterns = false;
  buttonPushCounter = (buttonPushCounter + 1) % 6;
}

void startAutoMode()
{
  autoChangePatterns = true;
}

void brightnessButton()
{
  if (FastLED.getBrightness() == BRIGHTNESS_SETTINGS[2])
    FastLED.setBrightness(BRIGHTNESS_SETTINGS[0]);
  else if (FastLED.getBrightness() == BRIGHTNESS_SETTINGS[0])
    FastLED.setBrightness(BRIGHTNESS_SETTINGS[1]);
  else if (FastLED.getBrightness() == BRIGHTNESS_SETTINGS[1])
    FastLED.setBrightness(BRIGHTNESS_SETTINGS[2]);
  else if (FastLED.getBrightness() == 0)
    FastLED.setBrightness(BRIGHTNESS_SETTINGS[0]); // Re-enable if lights are "off"
}

void brightnessOff()
{
  FastLED.setBrightness(0); // Lights out
}

// Patterns
void rainbowBars(int band, int barHeight) {
  int xStart = BAR_WIDTH * band;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    for (int y = TOP; y >= TOP - barHeight; y--) {
      matrix->drawPixel(x, y, CHSV((x / BAR_WIDTH) * (255 / NUM_BANDS), 255, 255));
    }
  }
}

void purpleBars(int band, int barHeight) {
  int xStart = BAR_WIDTH * band;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    for (int y = TOP; y >= TOP - barHeight; y--) {
      matrix->drawPixel(x, y, ColorFromPalette(purplePal, y * (255 / (barHeight + 1))));
    }
  }
}

void changingBars(int band, int barHeight) {
  int xStart = BAR_WIDTH * band;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    for (int y = TOP; y >= TOP - barHeight; y--) {
      matrix->drawPixel(x, y, CHSV(y * (255 / kMatrixHeight) + colorTimer, 255, 255));
    }
  }
}

void centerBars(int band, int barHeight) {
  int xStart = BAR_WIDTH * band;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    if (barHeight % 2 == 0) barHeight--;
    int yStart = ((kMatrixHeight - barHeight) / 2 );
    for (int y = yStart; y <= (yStart + barHeight); y++) {
      int colorIndex = constrain((y - yStart) * (255 / barHeight), 0, 255);
      matrix->drawPixel(x, y, ColorFromPalette(heatPal, colorIndex));
    }
  }
}

void whitePeak(int band) {
  int xStart = BAR_WIDTH * band;
  int peakHeight = TOP - peak[band] - 1;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    matrix->drawPixel(x, peakHeight, CHSV(0,0,255));
  }
}

void outrunPeak(int band) {
  int xStart = BAR_WIDTH * band;
  int peakHeight = TOP - peak[band] - 1;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    matrix->drawPixel(x, peakHeight, ColorFromPalette(outrunPal, peakHeight * (255 / kMatrixHeight)));
  }
}

void waterfall(int band) {
  int xStart = BAR_WIDTH * band;
  double highestBandValue = 60000; // Set this to calibrate your waterfall

  // Draw bottom line
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    matrix->drawPixel(x, 0, CHSV(constrain(map(bandValues[band],0,highestBandValue,160,0),0,160), 255, 255));
  }

  // Move screen up starting at 2nd row from top
  if (band == NUM_BANDS - 1){
    for (int y = kMatrixHeight - 2; y >= 0; y--) {
      for (int x = 0; x < kMatrixWidth; x++) {
        int pixelIndexY = matrix->XY(x, y + 1);
        int pixelIndex = matrix->XY(x, y);
        leds[pixelIndexY] = leds[pixelIndex];
      }
    }
  }
}
