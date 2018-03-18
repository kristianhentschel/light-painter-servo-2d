#include <Arduino.h>
#include "gcode_parser.h"
#include "gcode.h"
#include "debug.h"

GcodeParser::GcodeParser() {
  _buffer_first = 0;
  _buffer_next = 0;

  _emptyCommand.code = _INVALID;
  _emptyCommand.hasX = false;
  _emptyCommand.hasY = false;
  _emptyCommand.hasZ = false;
  _emptyCommand.hasP = false;
  _emptyCommand.hasF = false;
  _emptyCommand.hasS = false;
}

bool GcodeParser::available() {
  return (_buffer_first != _buffer_next);
}

gcodeCommand GcodeParser::next() {
  if (_buffer_first != _buffer_next) {
    gcodeCommand command = _buffer[_buffer_first];
    _buffer_first = (_buffer_first + 1) % GCODE_BUFFER_SIZE;
    return command;
  } else {
    DEBUG_PRINTLN("GCODE ERROR: Buffer underflow, check available() first.");
    return _emptyCommand;
  }
}

enum parserState {
  ACCEPT_CODE_LETTER,
  ACCEPT_CODE_DIGIT,
  ACCEPT_PARAMETER_LETTER,
  ACCEPT_DIGIT_OR_SIGN,
  ACCEPT_DIGIT,
  ACCEPT_FRACTIONAL_DIGIT
};

// returns true if a complete command has been parsed.
bool GcodeParser::input(char c) {
  // set the reset flag on reaching an invalid character or completing a command.
  static bool reset = true;
  static bool complete = false;
  static bool complete_parameter = false;
  static bool complete_code = false;
  static char current_letter;
  static int current_integer;
  static float current_sign;
  static float current_float;
  static float current_decimal_multiplier;
  static gcodeCommand command;
  static enum parserState state = ACCEPT_CODE_LETTER;

  if (reset) {
    reset = false;
    complete = false;
    complete_code = false;
    complete_parameter = false;
    state = ACCEPT_CODE_LETTER;
    current_sign = 1;
    current_integer = 0;
    current_float = 0;
    current_decimal_multiplier = 0.1;
    command = _emptyCommand;
  }

  switch(state) {
    case ACCEPT_CODE_LETTER:
      DEBUG_PRINTLN("ACCEPT_CODE_LETTER");
      switch(c) {
        case 'G':
        case 'M':
          current_letter = c;
          state = ACCEPT_CODE_DIGIT;
          break;
        default:
          reset = true;
      }
      break;
    case ACCEPT_CODE_DIGIT:
      DEBUG_PRINTLN("ACCEPT_CODE_DIGIT");
      if (c >= '0' && c <= '9') {
        current_integer = current_integer * 10 + (c - '0');
        state = ACCEPT_CODE_DIGIT;
        break;
      } else {
        complete_code = true;
        state = ACCEPT_PARAMETER_LETTER;
      }
      break;
    case ACCEPT_PARAMETER_LETTER:
      DEBUG_PRINTLN("ACCEPT_PARAMETER_LETTER");
      switch(c) {
        case 'X':
        case 'Y':
        case 'Z':
        case 'F':
        case 'P':
        case 'S':
          current_letter = c;
          state = ACCEPT_DIGIT_OR_SIGN;
          break;
        case ' ':
        case '\r':
          break;
        case '\n':
          complete = true;
          break;
        default:
          DEBUG_PRINTLN("GCODE: Invalid parameter letter.");
          reset = true;
      }
      break;
    case ACCEPT_DIGIT_OR_SIGN:
      DEBUG_PRINTLN("ACCEPT_DIGIT_OR_SIGN");
      if (c == '-') {
        current_sign = -1;
        state = ACCEPT_DIGIT;
      } else if (c >= '0' && c <= '9') {
        current_float = c - '0';
        state = ACCEPT_DIGIT;
      } else if (c != ' '){
        DEBUG_PRINTLN("GCODE: Expected minus sign or digit or space.");
        reset = true;
      }
      break;
    case ACCEPT_DIGIT:
      DEBUG_PRINTLN("ACCEPT_DIGIT");
      if (c >= '0' && c <= '9') {
        current_float = 10 * current_float + (c - '0');
      } else if (c == '.') {
        current_decimal_multiplier = 0.1;
        state = ACCEPT_FRACTIONAL_DIGIT;
      } else if (c == '\n') {
        complete_parameter = true;
        complete = true;
      } else {
        complete_parameter = true;
        state = ACCEPT_PARAMETER_LETTER;
      }
      break;
    case ACCEPT_FRACTIONAL_DIGIT:
      DEBUG_PRINTLN("ACCEPT_FRACTIONAL_DIGIT");
      if (c >= '0' && c <= '9') {
        current_float = current_float + current_decimal_multiplier * (c - '0');
        current_decimal_multiplier /= 10;
      } else if (c == '\n') {
        complete_parameter = true;
        complete = true;
      } else {
        complete_parameter = true;
        state = ACCEPT_PARAMETER_LETTER;
      }
      break;
  }

  // if we have a complete code, match the letter and integer to a code enum value.
  if (complete_code) {
    complete_code = false;
    if (current_letter == 'G') {
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
          reset = true;
      }
    } else if (current_letter == 'M') {
      switch (current_integer) {
        case 3:
          command.code = M03;
          break;
        case 5:
          command.code = M05;
          break;
        default:
          DEBUG_PRINTLN("GCODE: Invalid code number for M.");
          reset = true;
      }
    } else {
      DEBUG_PRINTLN("GCODE: Invalid code letter.");
      reset = true;
    }
  }

  // if a parameter value has been completed, store it in the right place.
  if (complete_parameter) {
    DEBUG_PRINTLN("GCODE: Have complete parameter.");
    switch (current_letter) {
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
    }
    // and reset for the next parameter.
    complete_parameter = false;
    current_sign = 1;
    current_float = 0;
  }

  if (complete) {
    DEBUG_PRINTLN("GCODE: Have complete command.");

    _buffer[_buffer_next] = command;
    _buffer_next = (_buffer_next + 1) % GCODE_BUFFER_SIZE;

    reset = true;
    return true;
  } else {
    return false;
  }
}
