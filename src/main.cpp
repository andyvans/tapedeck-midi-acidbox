#include <Arduino.h>
#include "constants.h"
#include "DeckLight.h"
#include "AudioProc.h"
#include "AudioControls.h"

TaskHandle_t AudioTask;
TaskHandle_t MidiTask;

DeckLight deckLight;
AudioProc audioProc;
AudioControls midiController;

// Task functions
void ProcessAudio(void *parameter);
void ProcessMidi(void *parameter);

void setup()
{
  Serial.begin(115200);
  
  Wire.begin(DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
  
  deckLight.Setup();
  midiController.Setup();

  xTaskCreatePinnedToCore(ProcessAudio, "Audio", 10000, NULL, 1, &AudioTask, 0);
  xTaskCreatePinnedToCore(ProcessMidi, "Midi", 10000, NULL, 1, &MidiTask, 1);
}

void loop()
{
  // All processing in tasks
}

void ProcessAudio(void *parameter)
{
  Serial.print("Audio task running on core ");
  Serial.println(xPortGetCoreID());
  for (;;)
  {
    deckLight.Tick();
    audioProc.Analyse([](){deckLight.Tick();});    
    deckLight.DisplayAudio(audioProc.bandValues);
    vTaskDelay(1);
  }
}

void ProcessMidi(void *parameter)
{
  Serial.print("Midi task running on core ");
  Serial.println(xPortGetCoreID());
  for (;;)
  {
    midiController.Tick();
    vTaskDelay(1);
  }
}
