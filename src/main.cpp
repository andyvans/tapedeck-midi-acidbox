#include <Arduino.h>
#include "constants.h"
#include "DeckLight.h"
#include "AudioProc.h"
#include "MidiController.h"

DeckLight deckLight;
AudioProc audioProc;
MidiController midiController;

void setup()
{
  Serial.begin(115200);
  
  deckLight.Setup();
}

void loop()
{
  deckLight.Tick();
  audioProc.Analyse();
  
  deckLight.Tick();
  deckLight.DisplayAudio(audioProc.bandValues);
}