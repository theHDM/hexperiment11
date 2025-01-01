#pragma once

#include <stdint.h>
#include <array>

#define __BPS 8
using byte_array_t = std::array<uint8_t, __BPS>;

union setting_t {
  bool b;
  uint8_t x;
  int i;
  float f;
  double d;
  byte_array_t w;
};

// using preprocessor macro instead of enum
// so that code will compile more cleanly
#define _settingSize  521
using setting_array = std::array<setting_t, _settingSize>;
setting_array settings;

void load_factory_defaults_to(setting_array& refTo_s) {
  // clear settings
  for (auto& i : refTo_s) { i = 0; }
  
  // define the index of each setting and then set the default
  #define _debug 0
  refTo_s[_debug].b = true;

  #define _currentAnchorX 10
  #define _currentAnchorY 20
  refTo_s[_currentAnchorX].i = 0;
  refTo_s[_currentAnchorY].i = 0;

  #define _currentAnchorPitch 30
  refTo_s[_currentAnchorX].d = 440.0;

  #define _currentTranspose 40
  refTo_s[_currentTranspose].i = 0;

  // hex direction 0 thru 5
  #define _currentAxisA 50
  refTo_s[_currentAxisA].x = 3;
  #define _currentAxisB 60
  refTo_s[_currentAxisB].x = 1;

  #define _currentEquaveIsJI 70
  refTo_s[_currentEquaveIsJI].b = false;
  #define _currentEquaveCents 71
  refTo_s[_currentEquaveCents].f = 1200.0;
  #define _currentEquaveNumerator 72
  refTo_s[_currentEquaveNumerator].i = 2;
  #define _currentEquaveDenominator 73
  refTo_s[_currentEquaveDenominator].i = 1;

  #define SYSTEM_EQ_DIV 0
  #define SYSTEM_JI     1
  #define SYSTEM_LG_SM  2
  #define _currentTuningSystem 100
  refTo_s[_currentTuningSystem].x = SYSTEM_EQ_DIV;

  // for equal divisions only
  #define _currentDivisions 110
  refTo_s[_currentDivisions].i = 12;
  #define _currentEqualStepsA 111
  refTo_s[_currentEqualStepsA].i = 2;
  #define _currentEqualStepsB 112
  refTo_s[_currentEqualStepsB].i = -7;

  // for large/small step only
  #define _currentLargeStepCount 120
  refTo_s[_currentLargeStepCount].i = 5;
  #define _currentSmallStepCount 121
  refTo_s[_currentSmallStepCount].i = 2;
  #define _currentLargeStepsA 122
  refTo_s[_currentLargeStepsA].i = 1;
  #define _currentSmallStepsA 123
  refTo_s[_currentSmallStepsA].i = 0;
  #define _currentLargeStepsB 124
  refTo_s[_currentLargeStepsB].i = -3;
  #define _currentSmallStepsA 125
  refTo_s[_currentSmallStepsB].i = -1;
  #define _currentLgToSmRatio 126
  refTo_s[_currentLgToSmRatio].f = 2.0;
  #define _currentLgSmMode 127
  refTo_s[_currentLgSmMode].i = 1;

  // for just intonation lattice only
  #define _currentJINumeratorA 130
  refTo_s[_currentJINumeratorA].i = 9;
  #define _currentJIDenominatorA 131
  refTo_s[_currentJIDenominatorA].i = 8;
  #define _currentJINumeratorB 132
  refTo_s[_currentJINumeratorA].i = 5;
  #define _currentJIDenominatorB 133
  refTo_s[_currentJIDenominatorA].i = 4;

  #define _scaleLock 99
  refTo_s[_scaleLock].b = false;
  #define _perceptual 98
  refTo_s[_perceptual].b = false;
  #define _paletteBeginsAtKeyCenter 97
  refTo_s[_paletteBeginsAtKeyCenter].b = true;

  #define _animationFPS  150
  // actually frames per 2^20 microseconds. close enough to 30fps
  refTo_s[_animationFPS].i = 32;
  #define RAINBOW_MODE 0
  #define TIERED_COLOR_MODE 1
  #define ALTERNATE_COLOR_MODE 2
  #define _colorMode  160
  refTo_s[_colorMode].x = RAINBOW_MODE;
  #define ANIMATE_NONE 0
  #define ANIMATE_STAR 1 
  #define ANIMATE_SPLASH 2 
  #define ANIMATE_ORBIT 3 
  #define ANIMATE_OCTAVE 4 
  #define ANIMATE_BY_NOTE 5
  #define ANIMATE_BEAMS 6
  #define ANIMATE_SPLASH_REVERSE 7
  #define ANIMATE_STAR_REVERSE 8
  #define _animationType  170
  refTo_s[_animationType].x = ANIMATE_NONE;
  #define BRIGHT_MAX 255
  #define BRIGHT_HIGH 210
  #define BRIGHT_MID 180
  #define BRIGHT_LOW 150
  #define BRIGHT_DIM 110
  #define BRIGHT_DIMMER 70
  #define BRIGHT_OFF 0
  #define _globalBrightness  180
  refTo_s[_globalBrightness].x = BRIGHT_MID;
  #define _rainbowLoopLength  190
  refTo_s[_rainbowLoopLength].f = 30.0f;  // seconds for 360 degrees 

  #define _toggleWheel  200
  refTo_s[_toggleWheel].b = false; // bool; 0 = mod, 1 = pb
  #define _wheelMode  210
  refTo_s[_wheelMode].b = false; // standard vs. fine tune mode
  #define _modSticky  220
  refTo_s[_modSticky].b = false;
  #define _pbSticky  230
  refTo_s[_pbSticky].b = false;
  #define _velSticky  240
  refTo_s[_velSticky].b = false;
  #define _modWheelSpeed  250
  refTo_s[_modWheelSpeed].i = 8;
  #define _pbWheelSpeed  260
  refTo_s[_pbWheelSpeed].i = 8; // scale by x128
  #define _velWheelSpeed  270
  refTo_s[_velWheelSpeed].i = 8;		
  #define _rotaryInvert  300
  refTo_s[_rotaryInvert].b = false; // 0 = off, 1 = on
  #define _screenSaverTimeout  310 
  refTo_s[_screenSaverTimeout].f = 8.0f; // seconds; scale by x1000000 for uS

  // MIDI settings
  #define _midiMode  400
  #define MIDI_STANDARD 0
  #define MIDI_MPE 1
  #define MIDI_2_POINT_OH 2
  #define MIDI_TUNING_TABLE 3
  refTo_s[_midiMode].x = MIDI_STANDARD;
  #define _MPEzoneCount  411
  refTo_s[_MPEzoneCount].x = 1;
  #define _MPEzoneOption  412
  // the sum of two 4-bit values
  // that represent the MPE zone boundaries
  // if this is a one-zone MPE, the bounds
  // are the start and end channel of the 
  // MPE zone. if two-zone, the bounds are
  // the end channel of zone 1 and the begin
  // channel of zone 2. by default, the
  // MPE is a single zone from ch 2 (0x1) to 16 (0xF).
  refTo_s[_MPEzoneOption].x = 0x1F;
  #define _MPEpitchBendSemis  410
  refTo_s[_MPEpitchBendSemis].x = 48; // 2, 12, 24, 48, or 96
  #define MIDID_NONE 0
  #define MIDID_USB 1
  #define MIDID_SER 2
  #define MIDID_BOTH 3
  #define _MIDIoutPorts 420
  refTo_s[_MIDIoutPorts].x = MIDID_USB;
  #define _MIDIprogramChange  440
  refTo_s[_MIDIprogramChange].x = 0; // 1 - 128


  #define SYNTH_OFF 0
  #define SYNTH_MONO 1
  #define SYNTH_ARPEGGIO 2
  #define SYNTH_POLY 3
  #define _synthMode  500
  refTo_s[_synthMode].x = SYNTH_POLY;
  #define WAVEFORM_SINE 0
  #define WAVEFORM_STRINGS 1
  #define WAVEFORM_CLARINET 2
  #define WAVEFORM_HYBRID 7
  #define WAVEFORM_SQUARE 8
  #define WAVEFORM_SAW 9
  #define WAVEFORM_TRIANGLE 10 
  #define _synthWaveform  510
  refTo_s[_synthWaveform].x = WAVEFORM_HYBRID;
  #define AUDIO_NONE 0
  #define AUDIO_PIEZO 1
  #define AUDIO_AJACK 2
  #define AUDIO_BOTH 3
  #define _synthAudioPorts  520
  refTo_s[_synthAudioPorts].x = AUDIO_PIEZO;
}

#include "LittleFS.h"       // code to use flash drive space as a file system -- not implemented yet, as of May 2024
bool fileSystemExists;

void setupFileSystem() {
  Serial.begin(115200);     // Set serial to make uploads work without bootsel button
  LittleFSConfig cfg;       // Configure file system defaults
  cfg.setAutoFormat(true);  // Formats file system if it cannot be mounted.
  LittleFS.setConfig(cfg);
  fileSystemExists = LittleFS.begin();         // Mounts file system.
  if (!fileSystemExists) {
    sendToLog("An Error has occurred while mounting LittleFS");
  }
}

bool load_settings() {
  if (!fileSystemExists) {
    return false;
  }
  File f = LittleFS.open("settings.dat","r+");
  if (!f) {
    sendToLog("An Error has occurred while loading settings.dat");
    return false;
  }
  int p = 0;
  while (f.available()) {
    byte_array_t b;
    for (int i = 0; i < __BPS; ++i) {
      b[i] = f.read();
    }
    settings[p].w = b;
    ++p;
  }
  sendToLog("settings loaded");
  f.close();
  return true;
}

void save_settings() {
  if (fileSystemExists) {
    File f = LittleFS.open("settings.dat","w+");
    if (!f) {
      sendToLog("An Error has occurred while saving settings.dat");        
    }
    for (int i = 0; i < _settingSize; ++i) {
      for (int j = 0; j < __BPS; ++j) {
        f.write((settings[i].w)[j]);
      }
    }
    sendToLog("settings saved");
    f.close();
  }  
}

void restore_default_settings() {
  load_factory_defaults_to(settings);
  if (Hardware_Version == HARDWARE_V1_2) {
      settings[_MIDIoutPorts].x = MIDID_BOTH;
      settings[_synthAudioPorts].x = AUDIO_BOTH;
      settings[_globalBrightness].x = BRIGHT_DIM;
      settings[_rotaryInvert].b = true;
  }
  sendToLog("default settings loaded");
  save_settings();
}
