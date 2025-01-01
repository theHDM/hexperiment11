#pragma once
#include <vector>

// Hardware pin constants
// If you rewire the HexBoard then change these pin values
const std::vector<unsigned> muxPins = {4,5,2,3}; // 8bit 4bit 2bit 1bit
const std::vector<unsigned> colPins = {6,7,8,9,10,11,12,13,14,15};
// 1 if analog (firmware 2.0), 0 if digital
const std::vector<bool> analogPins  = {0,0,0,0,0, 0, 0, 0, 0, 0};
const int default_analog_calibration_up = 480;
const int default_analog_calibration_down = 280;
const unsigned keyboard_pin_reset_period_in_uS = 16;
const unsigned keyboard_reads_to_retain        = 10;
#include "hexBoardHardware/keys.h"
pinGrid_obj pinGrid;

const unsigned rotaryPinA = 20;
const unsigned rotaryPinB = 21;
const unsigned rotaryPinC = 24;
const unsigned rotary_pin_fire_period_in_uS = 768;
#include "hexBoardHardware/rotary.h"
rotary_obj rotary;

const unsigned piezoPin = 23;
const unsigned audioJackPin = 25;
const unsigned target_audio_sample_rate_in_Hz = 31'250;
#include "hexBoardHardware/audio.h"
audioOut_obj audioOut;

const unsigned ledCount = 140;
const unsigned ledPin = 22;
const unsigned OLED_sdaPin = 16;
const unsigned OLED_sclPin = 17;
const unsigned target_LED_frame_rate_in_Hz = 60;
// #include "hexBoardHardware/LED.h"

constexpr unsigned target_audio_sample_halfperiod_in_uS = 500'000 / target_audio_sample_rate_in_Hz;
constexpr unsigned hardware_tick_period_in_uS = 
  ( target_audio_sample_halfperiod_in_uS > keyboard_pin_reset_period_in_uS
  ? target_audio_sample_halfperiod_in_uS : keyboard_pin_reset_period_in_uS ); 
constexpr unsigned actual_audio_sample_period_in_uS = 2 * hardware_tick_period_in_uS;
constexpr unsigned actual_audio_sample_rate_in_Hz  = 1'000'000 / actual_audio_sample_period_in_uS;
#include "hexBoardHardware/taskmgr.h"

// global, call this on setup() i.e. the 1st core
void hardware_setup() {
  // note that the audio pins can be set by the user, and that routine can live outside
  // of the setup. in other words, it is safe to run audioOut in the background even
  // if there are no audio outputs selected.
  audioOut.setup(actual_audio_sample_rate_in_Hz);
  // based on settings, enable whichever audio out puns
  audioOut.set_pin(piezoPin, true);
  //  rotary should define a pin A and B. pin C is the center click button if it exists
  rotary.setup(rotaryPinA,rotaryPinB,rotaryPinC);
  //  first T/F: are the column pins analog?
  //  second T/F: iterate thru the multiplex pins before the column pins?
  pinGrid.setup(colPins, analogPins, muxPins, true, keyboard_reads_to_retain, 
    default_analog_calibration_up, default_analog_calibration_down
  );
}

// global, call this on setup1() i.e. the 2nd core
void hardware_start_background_process() {
    task_mgr.init(hardware_tick_period_in_uS);
    // audio sample update - highest priority (stable period needed)
    task_mgr.add_task(actual_audio_sample_period_in_uS, std::bind(&audioOut_obj::poll, &audioOut));
    // rotary knob - 2nd highest priority (input drop risk)
    task_mgr.add_task(rotary_pin_fire_period_in_uS,     std::bind(&rotary_obj::poll,   &rotary));
    // keyboard - lowest priority (timing requirements are loose)
    task_mgr.add_task(keyboard_pin_reset_period_in_uS,  std::bind(&pinGrid_obj::poll,  &pinGrid));
    // start receiving input and processing audio output
    task_mgr.begin();
}