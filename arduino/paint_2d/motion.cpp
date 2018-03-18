#include "motion.h"

Motion::Motion(int feedrate, int tick_duration, int gear_radius, int stage_width, int stage_height, int stage_offset_top){
  _feedrate = feedrate;
  _tick_duration = tick_duration;
  _gear_radius = gear_radius;
  _stage_width = stage_width;
  _stage_height = stage_height;
  _stage_offset_top = stage_offset_top;
}
