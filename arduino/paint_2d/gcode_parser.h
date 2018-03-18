#ifndef __GCODE_PARSER_H__
#define __GCODE_PARSER_H__

#include "gcode.h";

#define GCODE_BUFFER_SIZE 4

class GcodeParser {
public:
  Gcode();
  bool input(char c);
  bool available();
  gcodeCommand next();
private:
  struct gcodeCommand buffer[GCODE_BUFFER_SIZE];
};

#endif // __GCODE_PARSER_H__
