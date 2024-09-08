#pragma once

// Board settings
#define LED_VOLTS                 5

// Audio processing
#define NUM_BANDS                 4 // Number of bands to process and display

// Define the pins for the devices
#define AUDIO_IN_PIN              1 // Analog audio signal input

#define LED_MATRIX_PIN            4 // LED WS2812 matrix
#define DECK_LED_PIN              5 // Deck LED
#define ANALOG_VU_METER_LEFT_PIN  6 // Analog VU meter left
#define ANALOG_VU_METER_RIGHT_PIN 7 // Analog VU meter right

// Rotary encoder pins
#define ROTARY_ENCODER_1_A_PIN    17
#define ROTARY_ENCODER_1_B_PIN    16
#define ROTARY_ENCODER_1_SW_PIN   15 // Theme change

#define ROTARY_ENCODER_2_A_PIN    8
#define ROTARY_ENCODER_2_B_PIN    3
#define ROTARY_ENCODER_2_SW_PIN   18

#define ROTARY_ENCODER_3_A_PIN    10
#define ROTARY_ENCODER_3_B_PIN    11
#define ROTARY_ENCODER_3_SW_PIN   9

#define ROTARY_ENCODER_4_A_PIN    13
#define ROTARY_ENCODER_4_B_PIN    14
#define ROTARY_ENCODER_4_SW_PIN   12 

// Button pins
//#define RECORD_BUTTON_PIN         21 // Change theme color
//#define REWIND_BUTTON_PIN         47 // Button 1
//#define FAST_FORWARD_BUTTON_PIN   48 // Button 2
//#define PLAY_BUTTON_PIN           35 // Button 3
//#define STOP_BUTTON_PIN           36 // Button 4

#define LEFT_SWITCH_PIN            36 // Left switch
#define RIGHT_SWITCH_PIN           37 // Right switch
