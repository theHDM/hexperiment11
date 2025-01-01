#include "src/RTOS_timer.h"            // helpful utility functions
#include "src/hexBoardHardware.h" // API to work with hexBoard hardware
#include "src/hexBoardLayout.h" // API to work with hexBoard hardware

#include "src/settings.h"
#include <GEM_u8g2.h>           // library of code to create menu objects on the B&W display
// Create an instance of the U8g2 graphics library.
U8G2_SH1107_SEEED_128X128_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);


void read_key(button_t& b) {
  b.key_state    = pinGrid.read_keypress(b.atMux, b.atCol);
  b.key_velocity = pinGrid.read_velocity(b.atMux, b.atCol);
  b.key_pressure = pinGrid.read_pressure(b.atMux, b.atCol);
}

void process_note(music_key_t& n) {
  switch (n.key_state) {
    case button_press:
      //tryMIDInoteOn(h);
      //trySynthNoteOn(h);
      break;
    case button_release:
      //tryMIDInoteOff(h);
      //trySynthNoteOff(h);
      break; 
  }
}

void process_command(other_cmd_t& c) {
  switch (c.key_state) {
    case button_press:
      switch (c.cmd) {
        //case CMDB + 3:
          //toggleWheel = !toggleWheel;
        //  break;
        default:
          // the rest should all be taken care of within the wheelDef structure
          break;
      }
    default: // inactive
      break;
  }
}

void process_all_keys() {
  // only do this if the pingrid object is free
  if (!(pinGrid.is_background_process_complete())) return;
  // read in the new pin state completely first
  for (auto& h : hexBoard.keys)     read_key(h);
  for (auto& h : hexBoard.commands) read_key(h);
  // then release pingrid object
  pinGrid.resume_background_process();
  // if you are in play mode
  for (auto& h : hexBoard.keys) {
    debug_key(h); // if needed
    process_note(h);
  }
  for (auto& h : hexBoard.commands) {
    debug_key(h); // if needed
    process_command(h);
  }
  // otherwise make the keys do something else
}

//  a global variable used to control the timing of setup functions between cores
int setup_phase = 0;

void setup() {
  hardware_setup();           //  set up the keyboard, rotary, and audio functions based on config constants.
    setup_phase = 1;        //  change the setup flag to let the other core know to start the background processes
}


void loop() { // need to add soft loop timing.
  process_all_keys();             //  every loop. interpret button press actions, play MIDI / synth notes
  delay(2000);
}

void setup1() {
  while (setup_phase < 1) {}    //  wait until the hardware objects have been set up in the other core
  hardware_start_background_process();   //  begin the background loops on this core
}
void loop1() {
  //  dedicate this core to computing the synth audio and running the background processes.
  if (audioOut.roomToWrite()) {
    //uint8_t temp = ((getTheCurrentTime() >> 12) & 0b00010000);
    //audioOut.write(temp /* synth.next_sample() */ );   //  write synth samples tnto the audio buffer.
  }
  //  and respond to all hardware task manager interrupts when they are called.
}
