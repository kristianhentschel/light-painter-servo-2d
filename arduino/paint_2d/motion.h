#ifndef __MOTION_H__
#define __MOTION_H__

class Motion {
public:
  Motion(int feedrate, int tick_duration, int gear_radius, int stage_width, int stage_height, int stage_offset_top);
private:
  float _current_x;
  float _current_y;

  int _feedrate;
  int _tick_duration;
  int _gear_radius;
  int _stage_width;
  int _stage_height;
  int _stage_offset_top;
};

#endif // __MOTION_H__
