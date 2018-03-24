#include <Arduino.h>
#include "motion.h"
#include "debug.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// #define PRINT_EVERY_TICK

Motion::Motion(int max_feedrate, int tick_duration, int gear_radius, int stage_width, int stage_height, int stage_offset_top){
  _max_feedrate = max_feedrate;
  _reposition_feedrate = _max_feedrate;
  _move_feedrate = _max_feedrate;
  _tick_duration = tick_duration;
  _gear_radius = gear_radius;
  _stage_width = stage_width;
  _stage_height = stage_height;
  _stage_offset_top = stage_offset_top;

  _pull_length = PI * _gear_radius;


  _current_x = 0;
  _current_y = 0;

  _interpolated_x = 0;
  _interpolated_y = 0;

  _target_x = 0;
  _target_y = 0;

  _dwell_ticks = 0;
  _idle = true;
  _spindle_active = false;

  _move_total_ticks = 0;
  _move_current_tick = 0;
}

// returns true: if the command can be immediately executed, false if it needs
// interpolation.
#define IMMEDIATE true
#define INTERPOLATED false

bool Motion::plan(gcodeCommand command) {
  _command = command;

  Serial.print("ok ");
  Serial.println(command.N);

  switch(command.code) {
    case G0:
      DEBUG_PRINTLN("MOTION: G0");
      // Fast linear reposition.
      // X, Y: End position.
      // F: set repositioning feedrate.

      if (command.hasF && command.F > 0)
        _reposition_feedrate = _convert_feedrate(command.F);

      _feedrate = _reposition_feedrate;
      _start_move(command.hasX ? command.X : _current_x, command.hasY ? command.Y : _current_y);
      return INTERPOLATED;
      break;
    case G1:
      DEBUG_PRINTLN("MOTION: G1");
      // Linear move.
      // X, Y: End position.
      // F: set move feedrate.
      if (command.hasF && command.F > 0)
        _move_feedrate = _convert_feedrate(command.F);

      _feedrate = _move_feedrate;
      _start_move(command.hasX ? command.X : _current_x, command.hasY ? command.Y : _current_y);
      return INTERPOLATED;
      break;
    case G2:
      DEBUG_PRINTLN("MOTION: G2");
      // Circular move clockwise
      // move linearly with current feedrate as not implemented
      DEBUG_PRINTLN("MOTION: G2 not supported, moving linearly.");
      _feedrate = _move_feedrate;
      _start_move(command.hasX ? command.X : _current_x, command.hasY ? command.Y : _current_y);
      return INTERPOLATED;
      break;
    case G3:
      DEBUG_PRINTLN("MOTION: G3");
      // Circular move counter-clockwise
      // move linearly with current feedrate as not implemented
      DEBUG_PRINTLN("MOTION: G3 not supported, moving linearly.");
      _feedrate = _move_feedrate;
      _start_move(command.hasX ? command.X : _current_x, command.hasY ? command.Y : _current_y);
      return INTERPOLATED;
      break;
    case G4:
      DEBUG_PRINTLN("MOTION: G4");
      _start_dwell(command.hasP ? command.P : 0);
      return INTERPOLATED;
      break;
    case G21:
      DEBUG_PRINTLN("MOTION: G21");
      DEBUG_PRINTLN("MOTION: G21 units in mm, no action taken.");
      return IMMEDIATE;
      break;
    case G90:
      DEBUG_PRINTLN("MOTION: G90");
      DEBUG_PRINTLN("MOTION: G90 absolute positioning, no action taken.");
      return IMMEDIATE;
      break;
    case M03:
      DEBUG_PRINTLN("MOTION: M03");
      DEBUG_PRINTLN("MOTION: Spindle on. ignoring S if present.");
      _spindle_active = true;
      return INTERPOLATED;
      break;
    case M05:
      DEBUG_PRINTLN("MOTION: M05");
      DEBUG_PRINTLN("MOTION: Spindle off. ignoring S if present.");
      _spindle_active = false;
      return INTERPOLATED;
      break;
    default:
      DEBUG_PRINTLN("MOTION: Unknown gcode command.");
      return IMMEDIATE;
  }
}

// Convert a given feedrate from mm per minute to mm per second
float Motion::_convert_feedrate(float mm_per_minute) {
  return MAX(0, MIN(mm_per_minute / 60, _max_feedrate));
}

// start a move action.
//
// calculate how long it should take at current feed rate.
// reset the tick counter
//
void Motion::_start_move(float x, float y) {
  _target_x = MIN(MAX(0, x), _stage_width);
  _target_y = MIN(MAX(0, y), _stage_height);

  // calculate the distance (mm) and duration (ms) of the move
  float distance = sqrt(pow(_target_x - _current_x, 2) + pow(_target_y - _current_y, 2));
  float duration = distance / _feedrate * 1000.0;

  // convert duration to number of ticks
  _move_current_tick = 0;
  _move_total_ticks = ceil(duration / _tick_duration);

  _idle = false;

  DEBUG_PRINT("MOTION: Starting move from ");
  DEBUG_PRINT(_current_x);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(_current_y);
  DEBUG_PRINT(" to ");
  DEBUG_PRINT(_target_x);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(_target_y);
  DEBUG_PRINT(", duration = ");
  DEBUG_PRINT(duration);
  DEBUG_PRINT("ms, ticks = ");
  DEBUG_PRINTLN(_move_total_ticks);
}

// start a dwell action.
//
// calculate the number of ticks to wait.
void Motion::_start_dwell(float seconds) {
  // tick duration is in milliseconds.
  _dwell_ticks = (seconds * 1000.0) / _tick_duration;

  _idle = false;

  DEBUG_PRINT("MOTION: Starting to dwell for ");
  DEBUG_PRINT(seconds);
  DEBUG_PRINTLN(" seconds.");
}

float Motion::_interpolate(float start, float end, float t, float duration) {
  return start + (end - start) * (t / duration);
}

// Tick the motion planning.
//
// Dwell, or advance the current interpolated target position.
void Motion::tick() {
  // _dwell_ticks is set by a G4 command, do nothing for a while.
  if (_dwell_ticks > 0) {
    _dwell_ticks--;

    if (_dwell_ticks == 0) {
      _idle = true;
    }

    #ifdef PRINT_EVERY_TICK
    DEBUG_PRINT("MOTION: Dwell ticks remaining: ");
    DEBUG_PRINTLN(_dwell_ticks);
    #endif

    return;
  }

  // If there is a current linearly interpolated move, continue it.
  // actual actuator signals are created on demand using current, target position and tick progress.
  if (_move_current_tick >= _move_total_ticks) {
    // complete the move if no more ticks are needed to achieve it.
    _current_x = _target_x;
    _current_y = _target_y;
    _move_total_ticks = 0;
    _move_current_tick = 0;

    _idle = true;
  } else {
    // move the interpolated position along for the current tick.
    _move_current_tick++;

    _interpolated_x = _interpolate(_current_x, _target_x, _move_current_tick, _move_total_ticks);
    _interpolated_y = _interpolate(_current_y, _target_y, _move_current_tick, _move_total_ticks);

    #ifdef PRINT_EVERY_TICK
    DEBUG_PRINT("MOTION: Move tick ");
    DEBUG_PRINT(_move_current_tick);
    DEBUG_PRINT(" x = ");
    DEBUG_PRINT(_interpolated_x);
    DEBUG_PRINT(", y = ");
    DEBUG_PRINTLN(_interpolated_y);
    #endif
  }
}

// Return true if the motion planner is ready to accept the next command.
bool Motion::idle() {
  return _idle;
}

float Motion::angle_left() {
  float length = sqrt(pow(_stage_height - _interpolated_y, 2) + pow(_interpolated_x, 2));
  float angle = (_pull_length - length) / (_pull_length) * 180;
  #ifdef PRINT_EVERY_TICK
  DEBUG_PRINT("MOTION: left angle =\t\t\t");
  DEBUG_PRINTLN(angle);
  #endif
  return MAX(0, MIN(angle, 180));
}

float Motion::angle_right() {
  float length = sqrt(pow(_stage_height - _interpolated_y, 2) + pow(_stage_width - _interpolated_x, 2));
  float angle = (_pull_length - length) / (_pull_length) * 180;
  #ifdef PRINT_EVERY_TICK
  DEBUG_PRINT("MOTION: right angle =\t");
  DEBUG_PRINTLN(angle);
  #endif
  return MAX(0, MIN(angle, 180));
}
