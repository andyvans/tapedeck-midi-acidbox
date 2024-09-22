#include "Display.h"

Display::Display() : display(Width, Height, &Wire) 
{
}

void Display::Setup()
{
  initialised = display.begin(SSD1306_SWITCHCAPVCC, 0x0, true, false);
  if (!initialised) return;

  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);
}

void Display::WriteText(const char *text, int length)
{
  if (!initialised) return;

  // copy all line to one above
  strncpy(line1, line2, sizeof(line1));
  strncpy(line2, line3, sizeof(line2));
  strncpy(line3, line4, sizeof(line3));
  strncpy(line4, text, sizeof(line4));
  
  display.clearDisplay();  
  display.setCursor(0, 0); // Start at top-left corner
  display.println(line1);
  display.println(line2);
  display.println(line3);
  display.println(line4);
  display.display();
}

void Display::Clear()
{
  if (!initialised) return;
  
  display.clearDisplay();
  display.display();
}