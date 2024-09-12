#include "Display.h"

Display::Display() : display(Width, Height, &Wire) {}

void Display::Setup()
{
  initialised = display.begin(SSD1306_SWITCHCAPVCC);
  if (!initialised) return;

  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
}

void Display::WriteText(const char *text)
{
  if (!initialised) return;

  display.clearDisplay();  
  display.setCursor(0, 0); // Start at top-left corner
  display.println(text);
  display.display();
}

void Display::Clear()
{
  if (!initialised) return;
  
  display.clearDisplay();
  display.display();
}