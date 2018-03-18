#ifndef __GCODE_PARSER_H__
#define __GCODE_PARSER_H__

#include "gcode.h";

#define GCODE_BUFFER_SIZE 4

class GcodeParser {
public:
  Gcode();
  bool input(char c);
  bool available();
  gcodeCommand* next();
private:
  // A circular buffer of parsed commands, re-use the structs.
  gcodeCommand _buffer[GCODE_BUFFER_SIZE];
  int _buffer_size = 0;
  int _buffer_start = 0;
  int _buffer_end = 0;
};

#endif // __GCODE_PARSER_H__
