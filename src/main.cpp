#include <Arduino.h>
#include <arduinoFFT.h>
#include "devices.h"
#include "DeckLight.h"

// Audio constants
#define SAMPLES 1024                               // Must be a power of 2
#define SAMPLING_FREQ 40000                        // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AMPLITUDE 27000                            // Depending on your audio source level, you may need to alter this value. Can be used as a 'sensitivity' control.
#define NUM_BANDS 4                                // To change this, you will need to change the bunch of if statements describing the mapping from bins to bands
#define NOISE 500                                  // Used as a crude noise filter, values below this are ignored

// FFT audio processing
unsigned int sampling_period_us;
int bandValues[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float vReal[SAMPLES];
float vImag[SAMPLES];
unsigned long newTime;
ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, SAMPLES, SAMPLING_FREQ);

DeckLight deckLight;

void analyseAudio();

void setup()
{
  Serial.begin(115200);
  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));
  deckLight.setup();
}

void loop()
{
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
  deckLight.tick();
  deckLight.displayAudio(bandValues);
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
        bandValues[3] += (int)vReal[i]*0.5;
    }
  }
}

