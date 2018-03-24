#ifndef __GCODE_PARSER_H__
#define __GCODE_PARSER_H__

#include "gcode.h";

#define GCODE_BUFFER_SIZE 4

class GcodeParser {
public:
  GcodeParser();
  bool input(char c);
  bool available();
  gcodeCommand next();
  bool acceptInput();
private:
  // A circular buffer of parsed commands, re-use the structs.
  gcodeCommand _buffer[GCODE_BUFFER_SIZE];
  unsigned int _buffer_first;
  unsigned int _buffer_next;
  unsigned int _buffer_items;

  gcodeCommand _emptyCommand;
};

#endif // __GCODE_PARSER_H__
