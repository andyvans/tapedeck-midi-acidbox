# 1971 Technics tape deck with machine generated music & RGB graphics

This project takes a 1971 Technics RS-273US tape deck and removes almost all the electronics and replaces it with more modern tech.

The motors have been removed, the potentiometers have been replaced with rotary encoders. All tape deck controls have microswitches added to them.

RGB LEDs have been added into the display and into the analog VU meters.

An OLED has been added inside to show the MIDI effects that are being applied to the music.

This includes two ESP32 S3 boards:

- One generates music using code from [https://github.com/andyvans/AcidBox](https://github.com/andyvans/AcidBox). It listens to serial for MIDI commands and some buttons to change music sequences.

- One analyses the audio using FFT and displays the results on an 12x4 RGB matrix. In addition it controls LEDS, OLED, original analog VU meters and rotary encoders. It also sends MIDI commands to the music generator.

## Watch demo on YouTube

[![Video](https://img.youtube.com/vi/i5UVzaoIb8w/maxresdefault.jpg)](https://www.youtube.com/watch?v=i5UVzaoIb8w)
