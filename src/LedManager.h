
class LedManager
{
public:
void setup();
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
void updateBarVuMeter(int bandValues[]);
void changeTheme();
void changeBrightness();
void updateAnalogVuMeter();

private:
  int amplitude = 27000;
  int themeIndex = 0;
  bool autoChangePatterns = false;
  
};