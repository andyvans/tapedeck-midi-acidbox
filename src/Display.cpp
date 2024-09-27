#include "Display.h"

Display::Display() : display(-1)
{
}

void Display::Setup()
{
  initialised = display.begin(SSD1306_SWITCHCAPVCC, 0x0, true, false);
  if (!initialised) return;

  display.setTextColor(SSD1306_WHITE);
  RenderSplash();
  lastMessageUpdate = millis();
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
  
  bool showText = lastMessageUpdate + TextTimeout > millis();
  if (showText) 
  {
    if (renderText)
    {
      RenderMessages();
      renderText = false;
    }
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

void Display::RenderSplash()
{
  display.clearDisplay();  
  display.setCursor(0, 0);
  
  display.setTextSize(2);
  display.println(">> ACIDBOX");
  display.setTextSize(1);
  display.println("     Andy & Karl");
  display.println("         2024");
  display.display();
}
void Display::RenderMessages()
{
  display.clearDisplay();  
  display.setCursor(0, 0);
  display.setTextSize(1);  
  display.println(line1);
  display.println(line2);
  display.println(line3);
  display.println(line4);
  display.display();
}

void Display::RenderScreenSaver()
{
  int now = millis();
  if (now - lastRenderTime < 50) return;
  
  if (currentIteration < display.height() / 2 - 2) {
        display.drawRoundRect(currentIteration, currentIteration, display.width() - 2 * currentIteration, display.height() - 2 * currentIteration, display.height() / 4, WHITE);
        display.display();
        currentIteration += 2;
    } else {
        currentIteration = 0; // Reset for the next cycle
        display.clearDisplay();
    }
}