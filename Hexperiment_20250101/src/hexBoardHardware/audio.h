#pragma once
#include <vector>
#include <algorithm>

struct ringBuffer_obj {
  // stores a vector of bytes
  std::vector<unsigned char> buffer;
  unsigned indexRead;
  unsigned indexWrite;
  unsigned capacity;
  unsigned spaceLeft;
  void init(unsigned capacity) {
    this->capacity = capacity;
    this->spaceLeft = capacity;
    this->indexRead = 0;
    this->indexWrite = 0;
    buffer.resize(capacity);
  }
  void write(unsigned char element) {
    buffer[indexWrite] = element;
    indexWrite = (indexWrite + 1) % capacity;
    --spaceLeft;
  }
  unsigned char read() {
    unsigned char element = buffer[indexRead];
    indexRead = (indexRead + 1) % capacity;
    ++spaceLeft;
    return element;
  }
};

#include <Wire.h>
#include "hardware/pwm.h"       // library of code to access the processor's built in pulse wave modulation features

class audioOut_obj {
  private:
    std::vector<unsigned> pwmPins;
    ringBuffer_obj buffer;
    unsigned sample_rate;
    void enable(unsigned pin) {
      auto n = std::find(pwmPins.begin(), pwmPins.end(), pin);
      if (n == pwmPins.end()) {
        pwmPins.emplace_back(pin);
        gpio_set_function(pin, GPIO_FUNC_PWM);      // set that pin as PWM
        unsigned slice = pwm_gpio_to_slice_num(pin);
        pwm_set_phase_correct(slice, true);           // phase correct sounds better
        pwm_set_wrap(slice, 254);                     // 0 - 254 allows 0 - 255 level
        pwm_set_clkdiv(slice, 1.0f);                  // run at full clock speed
        pwm_set_gpio_level(pin, 0);        // initialize at zero to prevent whining sound
        pwm_set_enabled(slice, true);                 // ENGAGE!
      }
    }
    void disable(unsigned pin) {
      auto e = remove(pwmPins.begin(), pwmPins.end(), pin);
      pwmPins.erase(e, pwmPins.end());
    }
  public:
    void setup(unsigned arg_sample_rate) {
      sample_rate = arg_sample_rate;
      buffer.init(1024);
    }
    void set_pin(unsigned pin, bool activate) {
      if (activate) {
        enable(pin);
      } else {
        disable(pin);
      }
    }
    void poll() {
      unsigned char lvl = 0;
      if (buffer.capacity - buffer.spaceLeft) {
        lvl = buffer.read();
      }
      for (auto& pin : pwmPins) {
        pwm_set_gpio_level(pin, lvl);      
      }
    }
    unsigned roomToWrite() {
      return buffer.spaceLeft;
    }
    void write(unsigned char element) {
      buffer.write(element);
    }
};

/*
  void recalculateEQ() {
    // EQ   8    7    6    5    4    3    2    1    0
    // dB  +4   +3   +1.5  0   -2   -4.5 -8   -14  -inf
    // dB = 20*ln(EQ)/ln(10).
    // NOTE this is NOT real EQ, it's adjusting the volume
    // only based on the note frequency, not its harmonics
    //                                       EQ  MIDI notes
          if (_freq <     8.0) {_eq = 0;} // -inf below  0 (C -1)
    else if (_freq <    60.0) {_eq = 8;} // +4dB 0 to 34 (Bb1)
    else if (_freq <  1500.0) {_eq = 7;} // +3dB   to 90 (F#6)
    else if (_freq <  2000.0) {_eq = 6;} // +1.5   to 95 (B 6)
    else if (_freq <  2500.0) {_eq = 5;} //  0dB   to 99 (Eb7)
    else if (_freq <  3000.0) {_eq = 4;} // -2dB  to 102 (F#7)
    else if (_freq <  3500.0) {_eq = 3;} // -4.5  to 104 (Ab7)
    else if (_freq <  4000.0) {_eq = 4;} // -2dB  to 107 (B 7)
    else if (_freq <  4500.0) {_eq = 5;} //  0dB  to 109 (C#8)
    else if (_freq <  5000.0) {_eq = 6;} // +1.5  to 111 (Eb8)
    else if (_freq < 13290.0) {_eq = 7;} // +3dB  to 127.99 (G#9-1c)
    else                      {_eq = 0;}
  }
  uint isoTwoTwentySix(float f) {
    //
    //  a very crude implementation of ISO 226
    //  equal loudness curves
    //    Hz dB  Amp ~ sqrt(10^(dB/10))
    //    200  0  8
    //    800 -3  6   
    //   1500  0  8
    //   3250 -6  4
    //   5000  0  8
    //
    if ((f < 8.0) || (f > 12500.0)) {   // really crude low- and high-pass
      return 0;
    } else {
      if (EQUAL_LOUDNESS_ADJUST) {
        if ((f <= 200.0) || (f >= 5000.0)) {
          return 8;
        } else {
          if (f < 1500.0) {
            return 6 + 2 * (float)(abs(f-800) / 700);
          } else {
            return 4 + 4 * (float)(abs(f-3250) / 1750);
          }
        }
      } else {
        return 8;
      }
    }
  }
*/