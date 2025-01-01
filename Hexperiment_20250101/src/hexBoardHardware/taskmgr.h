#pragma once
#include "hardware/timer.h"
#include "hardware/irq.h"       // library of code to let you interrupt code execution to run something of higher priority
#include <functional>

// task manager takes a list of repeat periods and callback functions
// these should run in the background
// set an alarm to run the "on_irq" boolean every "tick" microseconds.
// load in the background functions you want using the "bind" language.

static void on_irq();

class task_mgr_obj {
  private:
    struct task_obj {
      unsigned counter = 0;
      unsigned period;
      std::function<void()> exec_on_trigger;
      void set_period(unsigned arg_period) {
        period = arg_period;
      }
      void set_trigger(std::function<void()> arg_func) {
        exec_on_trigger = arg_func;
      }
      void increment(unsigned add_uS) {
        counter += add_uS;
      }
      void execute() {
        exec_on_trigger();
      }
      bool triggered() {
        if (counter >= period) {
          counter = counter % period;
          return true;
        }
        return false;
      }
    };
    unsigned tick_uS = 0;
    unsigned long long int next_trigger = 0;
    unsigned char alarm_ID = 0; // defined as a "byte" in the RP2040 hardware
    public:
      void init(unsigned arg_uS) {
        tick_uS = arg_uS;
        next_trigger = timer_hw->timerawh;
        next_trigger = (next_trigger << 32) | timer_hw->timerawl;
      }
      std::vector<task_obj> task_list;
      unsigned get_tick_uS() {
        return tick_uS;
      }
      void add_task(unsigned arg_repeat_uS, std::function<void()> arg_on_trigger) {
        task_obj new_task;
        new_task.set_period(arg_repeat_uS);
        new_task.set_trigger(arg_on_trigger);
        task_list.emplace_back(new_task);
      }
      void set_timer() {
        next_trigger += tick_uS;
        timer_hw->alarm[alarm_ID] = next_trigger;
      }
      void begin() {
        hw_set_bits(&timer_hw->inte, 1u << alarm_ID);  // initialize the timer
        irq_set_exclusive_handler(alarm_ID, on_irq);     // function to run every interrupt
        irq_set_enabled(alarm_ID, true);               // ENGAGE!
        set_timer();
      }
      void repeat_timer() {
        hw_clear_bits(&timer_hw->intr, 1u << alarm_ID);
        set_timer();
      }
};

// global definition, required to declare up front
task_mgr_obj task_mgr;

// global routine, required by RP2040 interrupt process
static void on_irq() {
  task_mgr.repeat_timer();
  unsigned t = task_mgr.get_tick_uS();
  for (auto& i : task_mgr.task_list) {
    i.increment(t);
  }
  for (auto& i : task_mgr.task_list) {
    if (i.triggered()) {
        i.execute();
        break;
    }
  }
}
