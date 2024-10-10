# 1971 Technics tape deck with machine generated music & RGB graphics

This project takes a 1971 Technics RS-273US tape deck and removes almost all the electronics and replaces it with more modern tech.

## How it works

- The motors have been removed, the potentiometers have been replaced with rotary encoders.
- All tape deck controls mechanisms were remove and microswitches installed.
- A 12x4 RGB matrix has been added inside the tape slot. An opaque plastic sheet has been added to the front of the tape slot to diffuse the light.
- RGB LEDs have been added into the display behind the arrow lights
- RGB LEDs have been added into the analog VU meters.
- Color themes drive all the RGB LEDs.
- An OLED has been added inside to show the MIDI effects that are being applied to the music.
- A small motor from a CD drive has been added to made the tape counter spin.
- MIDI messages are sent to the music generator to change the music sequences. This is controlled by the rotary encoders and displayed on the OLED.
- The unit is powered by a USB-C power supply. A USB-C plug has been added to the back of the unit to allow for easy power connection.
- The line-out plugs are used to provide audio out to a stereo system.
- The line-in plugs are used to provide audio in to render VU meters of incoming audio. The switch between the VU meters is used to determine which audio source to use. This is used if render your own music on the tape deck.

This includes two ESP32 S3 boards:

- One generates music using code from [https://github.com/andyvans/AcidBox](https://github.com/andyvans/AcidBox). It listens to serial pin for MIDI commands (and some buttons) to change the generated music, sequences and volume.

- One analyses the audio using FFT and displays the results on an 12x4 RGB matrix. In addition it controls LEDS, OLED, original analog VU meters and rotary encoders. It also sends MIDI commands to the music generator.

## Watch demo on YouTube

[![Video](https://img.youtube.com/vi/i5UVzaoIb8w/maxresdefault.jpg)](https://www.youtube.com/watch?v=i5UVzaoIb8w)

## Inside

![Inside](/images/inside.jpg)

## In progress

![In progress](/images/wip-1.jpg)
