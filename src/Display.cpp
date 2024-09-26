#include "Display.h"

Display::Display() : display(4) 
{
}

void Display::Setup()
{
  initialised = display.begin(SSD1306_SWITCHCAPVCC, 0x0, true, false);
  if (!initialised) return;

  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);
}

void Display::WriteText(const char *text)
{
  // copy all line to one above
  strncpy(line1, line2, sizeof(line1));
  strncpy(line2, line3, sizeof(line2));
  strncpy(line3, line4, sizeof(line3));
  strncpy(line4, text, sizeof(line4));
  lastMessageUpdate = millis();
  renderText = true;
}

void Display::Tick()
{
  if (!initialised) return;
  
  bool showText = true;//lastMessageUpdate + 5000 > millis();
  if (showText && renderText) 
  {
    RenderMessages();
    renderText = false;
  }
  else 
  {
    RenderScreenSaver();
  }
}

void Display::Clear()
{
  if (!initialised) return;
  
  display.clearDisplay();
  display.display();
}

void Display::RenderMessages()
{
  display.clearDisplay();  
  display.setCursor(0, 0); // Start at top-left corner
  display.println(line1);
  display.println(line2);
  display.println(line3);
  display.println(line4);
  display.display();
}

void Display::RenderScreenSaver()
{
  display.clearDisplay();

  for (int16_t i=0; i<display.height()/2-2; i+=2) 
  {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, WHITE);
    display.display();
    delay(1);
  }
}