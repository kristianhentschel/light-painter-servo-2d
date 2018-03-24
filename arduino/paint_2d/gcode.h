#ifndef __GCODE_H__
#define __GCODE_H__

typedef enum {
  _INVALID,
  G0,   // reposition TODO is it?
  G1,   // linear interpolation
  G2,   // circular interpolation clockwise
  G3,   // circular interpolation anti-clockwise
  G4,   // dwell
  G21,  // units are millimetres
  G90,  // absolute positioning
  M03,  // spindle on
  M05,  // spindle off
} gcodeCode;

typedef struct {
  gcodeCode code;
  int N;
  float X, Y, Z, S, P, F;
  bool hasX, hasY, hasZ, hasS, hasP, hasF;
} gcodeCommand;

#endif // __GCODE_H__
