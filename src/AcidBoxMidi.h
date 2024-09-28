#pragma once

// Channels
#define SYNTH1_MIDI_CHAN 1
#define SYNTH2_MIDI_CHAN 2
#define DRUM_MIDI_CHAN 10
#define GLOBAL_MIDI_CHAN 3  // Not a real channel, used for global controls

#define GM_MIDI
//#define VINTAGE_MIDI

#ifdef GM_MIDI
// 303 Synths MIDI CC
#define CC_303_PORTATIME    5   // affects gliding time
#define CC_303_VOLUME       7   // mix volume
#define CC_303_PORTAMENTO   65  // gliding on/off
#define CC_303_PAN          10  // pano
#define CC_303_WAVEFORM     70  // Blend between square and saw
#define CC_303_RESO         71
#define CC_303_CUTOFF       74
#define CC_303_ATTACK       73
#define CC_303_DECAY        72
#define CC_303_ENVMOD_LVL   75
#define CC_303_ACCENT_LVL   76
#define CC_303_REVERB_SEND  91
#define CC_303_DELAY_SEND   92
#define CC_303_DISTORTION   94
#define CC_303_OVERDRIVE    95
#define CC_303_SATURATOR    128

// 808 Drums MIDI CC
#define CC_808_VOLUME       7
#define CC_808_NOTE_PAN     8
#define CC_808_PAN          10
#define CC_808_RESO         71
#define CC_808_CUTOFF       74
#define CC_808_REVERB_SEND  91
#define CC_808_DELAY_SEND   92
#define CC_808_DISTORTION   94  // BitCrusher
#define CC_808_PITCH        89  
#define CC_808_NOTE_SEL     90  // select note, all the following CC note modifiers will be applied to this sample as it was RPN or NRPN
#define CC_808_NOTE_ATTACK  73
#define CC_808_NOTE_DECAY   72
#define CC_808_BD_TONE      21  // Specific per drum control
#define CC_808_BD_DECAY     23  // Bass Drum envelope decay
#define CC_808_BD_LEVEL     24  // Bass Drum mix level
#define CC_808_SD_TONE      25  // Snare Drum tone control
#define CC_808_SD_SNAP      26  // Snare Drum envelope decay
#define CC_808_SD_LEVEL     29  // Snare Drum mix level
#define CC_808_CH_TONE      61  // Closed Hat tone control
#define CC_808_CH_LEVEL     63  // Closed Hat mix level
#define CC_808_OH_TONE      80  // Open Hat tone control
#define CC_808_OH_DECAY     81  // Open Hat envelope decay
#define CC_808_OH_LEVEL     82  // Open Hat mix level

// Global 
#define CC_ANY_COMPRESSOR   93
#define CC_ANY_DELAY_TIME   84  // delay time
#define CC_ANY_DELAY_FB     85  // delay feedback level
#define CC_ANY_DELAY_LVL    86  // delay mix level
#define CC_ANY_REVERB_TIME  87  // reverb time
#define CC_ANY_REVERB_LVL   88  // reverb mix level
#define CC_ANY_RESET_CCS    121
#define CC_ANY_NOTES_OFF    123
#define CC_ANY_SOUND_OFF    120
#endif


#ifdef VINTAGE_MIDI
// 303 Synths MIDI CC
#define CC_303_PORTATIME    5
#define CC_303_VOLUME       11
#define CC_303_PORTAMENTO   65
#define CC_303_PAN          10
#define CC_303_WAVEFORM     70
#define CC_303_RESO         71
#define CC_303_CUTOFF       74
#define CC_303_ATTACK       73
#define CC_303_DECAY        75
#define CC_303_ENVMOD_LVL   12
#define CC_303_ACCENT_LVL   76
#define CC_303_REVERB_SEND  91
#define CC_303_DELAY_SEND   19
#define CC_303_DISTORTION   17
#define CC_303_SATURATOR    95

// 808 Drums MIDI CC
#define CC_808_VOLUME       7
#define CC_808_NOTE_PAN     8
#define CC_808_PAN          10
#define CC_808_RESO         71
#define CC_808_CUTOFF       74
#define CC_808_REVERB_SEND  91
#define CC_808_DELAY_SEND   92
#define CC_808_DISTORTION   94
#define CC_808_PITCH        89
#define CC_808_NOTE_SEL     90  // select note, all the following CC note modifiers will be applied to this sample as it was RPN or NRPN
#define CC_808_NOTE_ATTACK  73 // universal for all drums
#define CC_808_NOTE_DECAY   72
#define CC_808_BD_TONE      21  // Specific per drum control
#define CC_808_BD_DECAY     23
#define CC_808_BD_LEVEL     24
#define CC_808_SD_TONE      25
#define CC_808_SD_SNAP      26
#define CC_808_SD_LEVEL     29
#define CC_808_CH_TUNE      61
#define CC_808_CH_LEVEL     63
#define CC_808_OH_TUNE      80
#define CC_808_OH_DECAY     81
#define CC_808_OH_LEVEL     82

#define CC_ANY_COMPRESSOR   93
#define CC_ANY_DELAY_TIME   84
#define CC_ANY_DELAY_FB     85
#define CC_ANY_DELAY_LVL    86
#define CC_ANY_REVERB_TIME  87
#define CC_ANY_REVERB_LVL   88
#define CC_ANY_RESET_CCS    121
#define CC_ANY_NOTES_OFF    123
#define CC_ANY_SOUND_OFF    120

#endif