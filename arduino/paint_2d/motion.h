#ifndef __MOTION_H__
#define __MOTION_H__
#include <Arduino.h>
#include "gcode.h"

class Motion {
public:
  Motion(int max_feedrate, int tick_duration, int gear_radius, int stage_width, int stage_height, int stage_offset_top);
  void tick();
  float angle_left();
  float angle_right();
  bool spindle_active();
  bool plan(gcodeCommand command);
  bool idle();
private:
  // currently executed command.
  gcodeCommand _command;

  // Overall settings, set in constructor.
  int _max_feedrate;
  int _tick_duration;
  int _gear_radius;
  int _stage_width;
  int _stage_height;
  int _stage_offset_top;
  float _pull_length;

  // Spindle status
  bool _spindle_active;

  // Two settable feedrates, used by G0 and G1, in mm per second
  float _reposition_feedrate;
  float _move_feedrate;

  // position after the last completed move.
  float _current_x;
  float _current_y;

  // interpolated move position to be achieved during current tick.
  float _interpolated_x;
  float _interpolated_y;

  // end position of the current interpolated move.
  float _target_x;
  float _target_y;

  // duration of and progress within the current interpolated move.
  unsigned long _move_total_ticks;
  unsigned long _move_current_tick;

  // selected feedrate for the current move, in mm per second.
  float _feedrate;

  // For a dwell (rest) command, keep track of how long is remaining.
  unsigned int _dwell_ticks;

  // Whether there is something to be done on the next tick.
  bool _idle;

private:
  void _start_move(float x, float y);
  void _start_dwell(float seconds);
  float _convert_feedrate(float mm_per_minute);
  float _interpolate(float start, float end, float t, float duration);
};

#endif // __MOTION_H__
