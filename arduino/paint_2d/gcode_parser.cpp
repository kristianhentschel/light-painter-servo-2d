#include <Arduino.h>
#include "gcode_parser.h"
#include "gcode.h"
#include "debug.h"

GcodeParser::GcodeParser() {
  _buffer_first = 0;
  _buffer_next = 0;
  _buffer_items = 0;

  _emptyCommand.code = _INVALID;
  _emptyCommand.hasX = false;
  _emptyCommand.hasY = false;
  _emptyCommand.hasZ = false;
  _emptyCommand.hasP = false;
  _emptyCommand.hasF = false;
  _emptyCommand.hasS = false;
  _emptyCommand.N = -1;
}

bool GcodeParser::available() {
  return _buffer_items > 0;
}

bool GcodeParser::acceptInput() {
  // is there space in the buffer.
  return _buffer_items < GCODE_BUFFER_SIZE;
}

gcodeCommand GcodeParser::next() {
  if (_buffer_items > 0) {
    gcodeCommand command = _buffer[_buffer_first];
    _buffer_first = (_buffer_first + 1) % GCODE_BUFFER_SIZE;
    _buffer_items -= 1;
    return command;
  } else {
    DEBUG_PRINTLN("GCODE ERROR: Buffer underflow, check available() first.");
    return _emptyCommand;
  }
}

enum parserState {
  LETTER,
  INTEGER,
  DECIMAL
};

#define IS_LETTER(x) ((((x) >= 'A') && (x) <= 'Z') || ((x) == '*'))
#define IS_DIGIT(x) ((x) >= '0' && (x) <= '9')

// returns true if a complete command has been parsed.
// A g-code command consists of several sections, starting with a single letter
// or * symbol. Space separators are optional and ignored.
bool GcodeParser::input(char c) {
  static bool reset = true;
  static enum parserState state;
  static bool complete;
  static bool complete_component;
  static char current_letter;
  static char last_letter;
  static short current_sign;
  static int current_integer;
  static float current_float;
  static float current_decimal_multiplier;
  static gcodeCommand command;

  if (reset) {
    reset = false;
    complete = false;
    complete_component = false;
    state = LETTER;
    current_sign = 1;
    current_integer = 0;
    current_float = 0.0;
    current_decimal_multiplier = 0.1;
    current_letter = '\0';
    last_letter = '\0';
    command = _emptyCommand;
  }

  if (IS_LETTER(c)) {
    if (state != LETTER) {
      last_letter = current_letter;
      complete_component = true;
    }
    current_letter = c;
    state = INTEGER;
  } else if (c == '-') {
    current_sign = -1;
  } else if (c == '.') {
    state = DECIMAL;
  } else if (IS_DIGIT(c)) {
    if (state == INTEGER) {
      current_integer = current_integer * 10 + (c - '0');
      current_float = (float)current_integer;
    } else if (state == DECIMAL) {
      current_float = current_float + current_decimal_multiplier * (c - '0');
      current_decimal_multiplier = current_decimal_multiplier / 10;
    }
  } else if (c == '\n') {
    if (state != LETTER) {
      last_letter = current_letter;
      complete_component = true;
    }
    complete = true;
  } else {
    if (state != LETTER) {
      last_letter = current_letter;
      complete_component = true;
    }
    state = LETTER;
  }

  // if we have a complete code, match the letter and integer to a code enum value.
  if (complete_component) {

    DEBUG_PRINT(last_letter);
    DEBUG_PRINTLN(current_float);

    complete_component = false;
    switch (last_letter) {
      case 'G':
        switch (current_integer) {
          case 0:
            command.code = G0;
            break;
          case 1:
            command.code = G1;
            break;
          case 2:
            command.code = G2;
            break;
          case 3:
            command.code = G3;
            break;
          case 4:
            command.code = G4;
            break;
          case 90:
            command.code = G90;
            break;
          case 21:
            command.code = G21;
            break;
          default:
            DEBUG_PRINTLN("GCODE: Invalid code number for G.");
        }
        break;
      case 'M':
        switch (current_integer) {
          case 3:
            command.code = M03;
            break;
          case 5:
            command.code = M05;
            break;
          default:
            DEBUG_PRINTLN("GCODE: Invalid code number for M.");
        }
        break;
      case 'X':
        command.hasX = true;
        command.X = current_sign * current_float;
        break;
      case 'Y':
        command.hasY = true;
        command.Y = current_sign * current_float;
        break;
      case 'Z':
        command.hasZ = true;
        command.Z = current_sign * current_float;
        break;
      case 'F':
        command.hasF = true;
        command.F = current_sign * current_float;
        break;
      case 'P':
        command.hasP = true;
        command.P = current_sign * current_float;
        break;
      case 'S':
        command.hasS = true;
        command.S = current_sign * current_float;
        break;
      case 'N':
        command.N = current_sign * current_integer;
        break;
      case '*':
        DEBUG_PRINT("GCODE: Checksum (ignored) *");
        DEBUG_PRINTLN(current_sign * current_integer);
        break;
      default:
        DEBUG_PRINT("GCODE: Unknown component with letter ");
        DEBUG_PRINT(last_letter);
        DEBUG_PRINTLN(".");
    }
    current_sign = 1;
    current_integer = 0;
    current_float = 0;
    current_decimal_multiplier = 0.1;
  }

  if (complete && !reset) {
    DEBUG_PRINTLN("GCODE: Have complete command.");

    if (_buffer_items >= GCODE_BUFFER_SIZE) {
      DEBUG_PRINTLN("GCODE ERROR: Buffer overflow, check acceptInput() before attempting to parse input. Command dropped.");
    } else {
      _buffer[_buffer_next] = command;
      _buffer_next = (_buffer_next + 1) % GCODE_BUFFER_SIZE;
      _buffer_items += 1;
    }

    reset = true;
    return true;
  } else {
    return false;
  }
}
