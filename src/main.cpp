#include <Arduino.h>
#include "constants.h"
#include "DeckLight.h"
#include "AudioProc.h"
#include "AudioControls.h"

DeckLight deckLight;
AudioProc audioProc;
AudioControls midiController;

void setup()
{
  Serial.begin(115200);
  
  deckLight.Setup();
  midiController.Setup();
}

void processEvents()
{
  deckLight.Tick();
  midiController.Tick();
}

void loop()
{
  processEvents();
  audioProc.Analyse(processEvents);
  
  processEvents();
  deckLight.DisplayAudio(audioProc.bandValues);
}
