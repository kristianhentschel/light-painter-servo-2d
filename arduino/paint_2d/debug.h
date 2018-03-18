#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "settings.h"

#ifdef DEBUG
  #define DEBUG_PRINT Serial.print
  #define DEBUG_PRINTLN Serial.println
#else
  #define DEBUG_PRINT(x) (x)
  #define DEBUG_PRINTLN(x) (x)
#endif

#endif // __DEBUG_H__
