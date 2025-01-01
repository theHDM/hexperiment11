#pragma once

#include "hardware/timer.h"
unsigned long long int getTheCurrentTime() {
  unsigned long long int temp = timer_hw->timerawh;
  return (temp << 32) | timer_hw->timerawl;
}

/*
unsigned long long int runTime = 0;                // Program loop consistent variable for time in microseconds since power on
unsigned long long int lapTime = 0;                // Used to keep track of how long each loop takes. Useful for rate-limiting.
unsigned long long int loopTime = 0;               // Used to check speed of the loop
void timing_measure_lap() {
  lapTime = runTime - loopTime;
  loopTime = runTime;                                 // Update previousTime variable to give us a reference point for next loop
  runTime = getTheCurrentTime();   // Store the current time in a uniform variable for this program loop
}
*/

class softTimer {
  private:
    unsigned long long int startTime;
    unsigned long long int delay_uS;
    bool running;
    bool finishNow;
  public:
    softTimer() {
      startTime = 0;
      delay_uS = 0;
      running = false;
      finishNow = false;
    };
    void start(unsigned long long int _delay_uS, unsigned long long int _defer_uS) {
      startTime = getTheCurrentTime() + _defer_uS;
      delay_uS = _delay_uS;
      running = true;
      finishNow = false;
    };
    void stop() {
      running = false;
      finishNow = false;
    }
    void repeat() {
      startTime = startTime + delay_uS;
      running = true;
      finishNow = false;  
    }
    void restart() {
      start(delay_uS, 0);
    }
    void finish() {
      finishNow = true;
    }
    bool justFinished() {
      if (running && (finishNow || (getElapsed() >= delay_uS))) {
        stop();
        return true;
      } // else {
      return false;  
    }
    bool isRunning() {
      return running;
    }
    bool ifDone_thenRepeat() {
      if (justFinished()) {
          repeat();
          return true;
      }
      return false;
    }    
    unsigned long long int getStartTime() {
      return startTime;  
    }
    unsigned long long int getElapsed() {
      unsigned long long int temp = getTheCurrentTime();
      return (temp < startTime ? 0 : temp - startTime);
    }
    unsigned long long int getRemaining() {
      if (running) {
        unsigned long long int temp = getElapsed();
        if (finishNow || (temp >= delay_uS)) {
          return 0;
        } else {
          return (delay_uS - temp);
        }
      } else {
        return 0;
      }  
    }
    unsigned long long int getDelay()  {
      return delay_uS;
    }
};

unsigned long long int runTime = 0;                // Program loop consistent variable for time in microseconds since power on
unsigned long long int lapTime = 0;                // Used to keep track of how long each loop takes. Useful for rate-limiting.
unsigned long long int loopTime = 0;               // Used to check speed of the loop
void timing_measure_lap() {
  lapTime = runTime - loopTime;
  loopTime = runTime;                                 // Update previousTime variable to give us a reference point for next loop
  runTime = getTheCurrentTime();   // Store the current time in a uniform variable for this program loop
}