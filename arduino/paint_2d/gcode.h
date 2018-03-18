typedef enum {
  G0,   // reposition TODO is it?
  G1,   // linear interpolation
  G2,   // circular interpolation clockwise
  G3,   // circular interpolation anti-clockwise
  G21,  // units are millimetres
  G90,  // absolute positioning
  M03,  // spindle on
  M05,  // spindle off
} gcodeCode;

typedef struct _gcode_command {
  gcodeCode code ;
  float x, y, s, p, f;
} *gcodeCommand;
