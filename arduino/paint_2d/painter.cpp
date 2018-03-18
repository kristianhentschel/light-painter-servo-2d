#include <Arduino.h>
#include "painter.h"
#include "settings.h"
#include "motion.h"
#include "gcode.h"
#include "gcode_parser.h"
#include "debug.h"

Painter::Painter(void (*write_left)(float), void (*write_right)(float)) {
  _write_left = write_left;
  _write_right = write_right;
  _motion = new Motion(MAX_FEEDRATE, TICK_MILLISECONDS, GEAR_RADIUS, STAGE_WIDTH, STAGE_HEIGHT, STAGE_OFFSET_TOP);
  _gcode_parser = new GcodeParser();
}

void Painter::input(char c) {
  DEBUG_PRINT("INPUT: ");
  DEBUG_PRINT(c);
  DEBUG_PRINTLN();

  if (_gcode_parser->input(c)) {
    DEBUG_PRINTLN('command parsed');
  }
}

void Painter::tick() {
  // Parse buffered gcodes, until one that needs interpolation is encountered.
  while (_gcode_parser->available() && _motion->plan(_gcode_parser->next()))
    ;

  // Run the motion planning interpolation. This is a no-op if no movements are
  // scheduled.
  _motion->tick();

  // Send the computed angles to the servo motors.
  _write_left(_motion->angle_left());
  _write_right(_motion->angle_right());
}
