#pragma once
#include "hexBoardLayout/wiringMap.h"
#include "hexBoardLayout/buttonGrid.h"
//#include "hexBoardLayout/tuningSystem.h"
//#include "hexBoardLayout/library.h"

// THIS IS THE OBJECT WHERE ALL THE MAGIC HAPPENS
button_grid_t hexBoard;



// need to outline dependencies
/*
  This section of the code contains broad
  procedures for assigning musical notes
  and related values to each button
  of the hex grid.

// run this if the layout, key, or transposition changes, but not if color or scale changes
void assignPitches() {     
  sendToLog("assignPitch was called:");
  for (auto& h : hexBoard.keys) {
    // steps is the distance from C (the anchor note)
    // the stepsToMIDI function needs distance from A4
    // it also needs to reflect any transposition, but
    // NOT the key of the scale.

    float N = 60; //stepsToMIDI(current.pitchRelToA4(h.stepsFromC));
    
    if (N < 0 || N >= 128) {
      h.note = 255;
      h.bend = 0;
      h.pitch = 255.0;
      h.frequency = 0.0;
    } else {
      // h.note and h.bend may soon be collapsed into
      // a floating h.note value, with h.bend calculated
      // situationally.
      // also might have to reflect global tuning (i.e.
      // what if the KBM file defines A = 432 or A = 441
      h.pitch = N;
      h.note = ((N >= 127) ? 127 : round(N));
      // skip for now // h.bend = (ldexp(N - h.note, 13) / MPEpitchBendSemis);
      h.frequency = MIDItoFreq(N);
    }
  }
  sendToLog("assignPitches complete.");
}
void applyScale() {
  sendToLog("applyScale was called:");
  for (auto& h : hexBoard.keys) {
    if (current.scale().tuning == ALL_TUNINGS) {
      // i.e. "no scale"
      h.inScale = 1;
    } else {
      uint degree = current.keyDegree(h.stepsFromC); 
      if (degree == 0) {
        h.inScale = 1;    // the root is always in the scale
      } else {
        uint tempSum = 0;
        uint iterator = 0;
        while (degree > tempSum) {
          tempSum += current.scale().pattern[iterator];
          iterator++;
        }  // add the steps in the scale, and you're in scale
        h.inScale = (tempSum == degree);   // if the note lands on one of those sums
      }
    }
  }
  // for now // setLEDcolorCodes();
  sendToLog("applyScale complete.");
}
*/

void assign_pitches(Tunings::Tuning t, key_layout l) {
  for (auto& k : hexBoard.keys) {

    k.midiNote = 0;
    k.midiBend = 0;
    k.frequency = 0.0;

    // output h.midinote, h.midibend,
    // h frequencies
    // if in the list of overrides, apply it
  }
  // sort by number of steps
  // count up and down from root note in kbm
  // until 127 or 0 reached
  // everything else is off the tuning table
  // k.midiTuningTable = xyz;
}

void apply_layout(Tunings::Tuning t, key_layout l) {
  // start by clearing out the key and command vectors
  hexBoard.keys.clear();
  hexBoard.commands.clear();  
  // then repopulate the key and command vectors using
  // the backup button data and the given tuning and layout

  // in this version, first we figure out which buttons are notes / commands
  const std::vector<unsigned> assignCmd = {0,20,40,60,80,100,120};
  for (auto& b : hexBoard.button_data) {
    auto findCmd = std::find(assignCmd.begin(),assignCmd.end(),b.pixel);
    bool isCmd = (findCmd != assignCmd.end());
    hexBoard.pixel_is_cmd[b.pixel] = isCmd; // add to the lookup map
    if (isCmd) {
      other_cmd_t tempCmd(b);
      tempCmd.cmd = findCmd - assignCmd.begin();
      hexBoard.pixel_to_index[tempCmd.pixel] = hexBoard.commands.size();
      hexBoard.commands.emplace_back(tempCmd);
    } else {
      music_key_t tempKey(b);
      hexBoard.pixel_to_index[tempKey.pixel] = hexBoard.keys.size();
      hexBoard.keys.emplace_back(tempKey);
    }
  }
  // then the music note keys get populated based on the layout
  for (auto& k : hexBoard.keys) {
    hex_t d = k.coord - l.root_location;
    int m = t.keyboardMapping.middleNote 
      + (d.x * l.steps_per_2x + d.y * l.steps_per_2y) / 2;
    k.scaleDegree = t.scalePositionForMidiNote(m);
    k.scaleEquave = t.equaveForMidiNote(m);
  }
  // crude defaults for commands until this feature is improved
}

/*
void applyLayout() {       // call this function when the layout changes
  sendToLog("buildLayout was called:");
  // if the layout is supposed to be developed procedurally,
  // i.e. an isomorphic regular layout,
  // then run the procedure here.



  hex_t middleC = hexBoard.button_at_pixel(current.layout().hexMiddleC).coord;
  // in orthogonal coordinates, a single hex distance = 2 steps, either
  // +/- 2X, or +/- 1X +/- 1Y. keep the scale vector doubled so that
  // integer values are not lost. we might change this when steps are
  // changed to floating cents values.
  hex_t doubleStepVector = { current.layout().acrossSteps,
    (2 * current.layout().dnLeftSteps) + current.layout().acrossSteps};
  for (auto& h : hexBoard.keys) {
    h.stepsFromC = dot_product(doubleStepVector, h.coord - middleC) / 2;
    // h.centsFromAnchor in the future
    // will also require defining template<typename T> hex_t<T> to allow
    // floating values of step-vectors.
  }
  applyScale();        // when layout changes, have to re-apply scale and re-apply LEDs
  assignPitches();     // same with pitches
  sendToLog("buildLayout complete.");
}
*/

// when you run the setup routine, you should then have
// arrays of key definitions that are ready to be loaded
// with instructions and states.
// run this once at startup.
void button_grid_setup() {
  // can make this hardware dependent in the future
  std::vector<key_identification> config_hexboard_layout = config_hexboard_layout_1_2;
  for (auto& eachRow : config_hexboard_layout) {
    switch_t tempSwitch(
      eachRow.multiplexer_value,
      eachRow.column_pin_index,
      eachRow.switch_type
    );
    switch (tempSwitch.type) {
      case unused_pin:
        break;
      case hardwired:
        hexBoard.hardwired_switches.emplace_back(tempSwitch);
        break;
      default:
        button_t tempButton(
          tempSwitch,
          {eachRow.hex_coordinate_x,eachRow.hex_coordinate_y},
          eachRow.associated_pixel
        );
        hexBoard.coord_to_pixel[tempButton.coord] = tempButton.pixel; // add to the lookup map
        hexBoard.button_data.emplace_back(tempButton);
        break;
    }
  }
}