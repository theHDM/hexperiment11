#pragma once
#include <Wire.h>
#include <vector>
#include <deque>
#include "hardware/timer.h"

class pinGrid_obj {
  private:
    bool _isEnabled;
    bool _cycle_mux_pins_first;
    bool _readComplete;
    unsigned _keystrokes_retained;
    unsigned _colSize;
    unsigned _colCounter;
    std::vector<bool> _isAnalogPin;
    std::vector<unsigned> _colPins;
    unsigned _muxSize;
    unsigned _muxCounter;
    std::vector<unsigned> _muxPins;
    unsigned _muxMaxValue;
    unsigned _keyCount;
    unsigned _gridCounter;
    struct _keyData {
      bool isAnalog;
      int calibrate_up;
      int calibrate_down;
      std::deque<int> _level;
      std::deque<unsigned long long int> _time;
    };
    std::vector<_keyData> _key;
    void col_pin_on(unsigned c) {
      // this action only needs to happen for digital reads
      if (!(_isAnalogPin[c])) {
        pinMode(_colPins[c], INPUT_PULLUP);
      }
    }
    void col_pin_off(unsigned c) {
      // this action only needs to happen for digital reads
      if (!(_isAnalogPin[c])) {
        pinMode(_colPins[c], INPUT);
      }
    }
    void init_pin_states() {
      for (auto& m : _muxPins) {
        pinMode(m, OUTPUT); 
      }
      for (unsigned i = 0; i < _colSize; ++i) {
        col_pin_on(i);
      }
    }
    void init_key_data() {
      _key.resize(_keyCount);
      for (auto& k : _key) {
        k._level.clear();
        k._time.clear();
      }
      for (unsigned c = 0; c < _colSize; ++c) {
        for (unsigned m = 0; m < _muxMaxValue; ++m) {
          _key[linear_index(c,m)].isAnalog = _isAnalogPin[c];
        }
      }
    }
    void calibrate_by_ref(_keyData& k, int up_level, int down_level) {
      k.calibrate_up = up_level;
      k.calibrate_down = down_level;
    }
    void calibrate_keys(int up_level, int down_level) {
      for (auto& k : _key) {
        if (k.isAnalog) {
          calibrate_by_ref(k, up_level, down_level);
        } else {
          calibrate_by_ref(k, 1, 0);
        }
      }
    }
    bool advanceCol() {
      col_pin_off(_colPins[_colCounter]);
      _colCounter = (++_colCounter) % _colSize;
      col_pin_on(_colPins[_colCounter]);
      return (!(_colCounter));
    }
    bool advanceMux() {
      _muxCounter = (++_muxCounter) % _muxMaxValue;
      for (unsigned b = 0; b < _muxSize; b++) {
        digitalWrite(_muxPins[b], (_muxCounter >> b) & 1);
      }
      return (!(_muxCounter));
    }
    bool keyDown(_keyData &k, unsigned lookback) {
      if (lookback >= k._level.size()) {
        return false;
      }
      return (k._level[k._level.size() - 1 - lookback] < k.calibrate_up);
    }
  public:
    unsigned linear_index(unsigned c, unsigned m) {
      return ((c << _muxSize) | m);
    }
    void resume_background_process() {
      _readComplete = false;
      _gridCounter = 0;
    }
    void setup(std::vector<unsigned> colPins, std::vector<bool> isAnalog,
    std::vector<unsigned> muxPins, bool cycleMuxFirst, unsigned keystrokesToRetain, 
    int default_calibrate_up, int default_calibrate_down) {
      _colPins = colPins;
      _isAnalogPin = isAnalog;
      _muxPins = muxPins;
      _cycle_mux_pins_first = cycleMuxFirst;
      _keystrokes_retained = keystrokesToRetain;  
      init_pin_states();
      _colSize = _colPins.size();
      _muxSize = _muxPins.size();
      _muxMaxValue = (1u << _muxSize);
      _keyCount = (_colSize << _muxSize);
      init_key_data();
      calibrate_keys(default_calibrate_up, default_calibrate_down);
      _colCounter = 0;
      _muxCounter = 0;
      resume_background_process();
    }
    int get_pin_state(unsigned pin, bool analog) {
      if (analog) {
        return analogRead(pin); 
      }
      return digitalRead(pin);
    }
    void poll() {
      if (!(_readComplete)) {
        auto& k = _key[linear_index(_colCounter,_muxCounter)];
        if (k._time.size() >= _keystrokes_retained) {
          k._time.pop_front();
        }
        unsigned long long int t = timer_hw->timerawh;
        t = (t << 32) | timer_hw->timerawl;
        k._time.emplace_back(t);
        if (k._level.size() >= _keystrokes_retained) {
          k._level.pop_front();
        }
        k._level.emplace_back(get_pin_state(
          _colPins[_colCounter], _isAnalogPin[_colCounter]
        ));
        ++_gridCounter;
        if (_cycle_mux_pins_first) {
          if (advanceMux()) { 
            _readComplete = advanceCol();
          }
        } else {
          if (advanceCol()) {
            _readComplete = advanceMux();
          }
        }
      }
    }
    bool is_background_process_complete() {
      return _readComplete;
    }
    void calibrate(unsigned atM, unsigned atC, int up_level, int down_level) {
      auto& k = _key[linear_index(atC, atM)];
      calibrate_by_ref(k, up_level, down_level);
    }
    int read_key_state(unsigned atM, unsigned atC) {
      auto& k = _key[linear_index(atC, atM)];
      return k._level.back();
    }
    unsigned long long int read_time_stamp(unsigned atM, unsigned atC) {
      auto& k = _key[linear_index(atC, atM)];
      return k._time.back();
    }
    int read_keypress(unsigned atM, unsigned atC) {
      // 0 neutral 1 press 2 release 3 hold
      auto& k = _key[linear_index(atC, atM)];
      return (keyDown(k, 1) << 1) | keyDown(k, 0);
    }
    // negative means moving down, positive means moving up
    // expressed in resolution units per millisecond
    double read_velocity(unsigned atM, unsigned atC) {
      auto& k = _key[linear_index(atC, atM)];
      if (k._level.size() < 2) {
        return 0.0;
      }
      return       ( k._level.back() - k._level.front() )
        / (1000.0 * ( k._time.back() - k._time.front() )); 
    }
    // return as percentage from 0 to 100 based on calibration
    double read_pressure(unsigned atM, unsigned atC) {
      auto& k = _key[linear_index(atC, atM)];
      double p = (k.calibrate_up - k._level.back()) / (k.calibrate_up - k.calibrate_down);
      if (p > 1) {
        return 1.0;
      }
      if (p < 0) {
        return 0.0;
      }
      return p;
    }
    unsigned colPinCount() {
      return _colSize;
    }
    unsigned muxPinMaxValue() {
      return _muxMaxValue;
    }
    unsigned buttonCount() {
      return _keyCount;
    }
};
