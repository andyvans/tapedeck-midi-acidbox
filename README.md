# 1971 Technics tape deck with machine generated music & RGB graphics

This project takes a 1971 Technics RS-273US tape deck and removes almost all the electronics and replaces it with more modern tech. At the heart is an ESP32 S3 board that generates music using AcidBox - an ESP32 headless acid combo of tb303 + tb303 + tr808 like synthesizers. A second ESP32 S3 board analyses the audio and displays the graphics and sends MIDI commands to the AcidBox.

## What was done

- Two ESP32 S3 and a single DAC board were added.
- The original motor was been removed. All gears and belts have been removed.
- The potentiometers have been replaced with rotary encoders.
- All tape deck controls mechanisms were removed and microswitches installed.
- A 12x4 RGB matrix has been added inside the tape slot. An opaque plastic sheet has been added to the front of the tape slot to diffuse the light.
- The original VU meters have been retained and are driven by the ESP32 S3 board via 18K resistors.
- RGB LEDs have been added into the display behind the arrow lights.
- RGB LEDs have been added into the analog VU meters.
- An OLED has been added inside to show the MIDI effects that are being applied to the music.
- A small motor from a CD drive has been added to made the tape counter spin.
- The unit is powered by a USB-C power supply. A USB-C plug has been added to the back of the unit to allow for easy power connection.
- The line-out plugs are used to provide audio out to a stereo system.
- The line-in plugs are used to provide audio in to render VU meters of incoming audio. The switch between the VU meters is used to determine which audio source to use. This is used if render your own music on the tape deck.

## How it works

The ESP32 S3 boards perform two functions:

- One generates music using code from [https://github.com/andyvans/AcidBox](https://github.com/andyvans/AcidBox). In addition, it listens to serial pin for MIDI commands (and some buttons) to control the synthesizers and volume.

- One analyses the audio using FFT and displays the results on an 12x4 RGB matrix (using code from this repository), controls LEDS, OLED, original analog VU meters and rotary encoders. It also sends MIDI commands to AcidBox.

## Controls

- The tape deck buttons (play, stop, pause, rewind, fast forward, record) now trigger microswitches. These changes the MIDI sequence being played.
- Color themes drive the colours of the RGB LEDs. This is controlled by the button of a rotary encoder.
- The rotary encoders change the music sequences by sending MIDI messages to control the synthesizers.
- The MIDI changes are displayed on the OLED.
- The VU meters show the audio levels of the incoming audio from the synthesizers.
- The input switch determines which audio source to use for the VU meters.

## Credits

[Copych](https://github.com/copych) has done a really great job pulling together code from various sources to refine and create [AcidBox](https://github.com/andyvans/AcidBox).

## Watch demo on YouTube

[![Video](https://img.youtube.com/vi/i5UVzaoIb8w/maxresdefault.jpg)](https://www.youtube.com/watch?v=i5UVzaoIb8w)

## Inside

![Inside](/images/inside.jpg)

## In progress

![In progress](/images/wip-1.jpg)
