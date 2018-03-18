#ifndef __PAINTER_H__
#define __PAINTER_H__

#include <Arduino.h>

#include "motion.h"
#include "gcode_parser.h"

class Painter {
public:
  Painter(void (*write_left)(float), void (*write_right)(float));
  void tick();
  void input(char c);
private:
  void (*_write_left)(float);
  void (*_write_right)(float);
  Motion *_motion;
  GcodeParser *_gcode_parser;
};

#endif // __PAINTER_H__
