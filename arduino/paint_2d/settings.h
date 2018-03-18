#ifndef __SETTINGS_H__
#define __SETTINGS_H__

// ## Debugging
//
// Comment out to disable diagnostic prints to the Serial port.

#define DEBUG

// ## Pins

#define PIN_LED 11
#define PIN_SERVO_LEFT 2
#define PIN_SERVO_RIGHT 3

// ## Movement
//
// * The MAX_FEEDRATE limites the target movement speed, in mm per second.
// * The TICK_MILLISECONDS is the duration of a tick in ms. It effectively
//   controls the interpolation distance between positions.
// * The GEAR_RADIUS is a proxy for the circumference of the gear, controlling
//   the amount of movement a 180 degree rotation can give.
//
// The machine should be set up with both gears in the zero position, fully
// coiled up. The target should then be in a central position, at the top of the
// stage.

#define MAX_FEEDRATE 10
#define TICK_MILLISECONDS 10
#define GEAR_RADIUS 45

// ## Stage Layout
//
// * The origin is at the bottom left.
// * The STAGE_WIDTH is the distance between the two idlers at the top.
// * The STAGE_OFFSET_TOP is the height of the area between the top of the
//   idlers, and the top of the stage, that is be used.
// * The STAGE_HEIGHT is the height of the active stage area only.
//
// Therefore, x values range from 0 to STAGE_WIDTH, and y values from 0 to
// STAGE_HEIGHT.
//
// The diagram below illustrates this:
//
// ```
//    y
//    ^
// ---O----------------O---
// SOT|\              /|
// ---|-\-----------/--|---
// ^  |  \        /    |
// SH | <---- SW ----> |
// v  |    \ /         |
// ---0-----*------------> x
//    ^(0, 0)
// ```

#define STAGE_WIDTH 120
#define STAGE_HEIGHT 100
#define STAGE_OFFSET_TOP 10

#endif // __SETTINGS_H__
