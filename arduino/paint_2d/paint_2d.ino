#include <Servo.h>
#include "painter.h"
#include "settings.h"

// Global variables for the servo objects
Servo servo_left;
Servo servo_right;

// Set up the painter object, with function pointers to hide the Servo object.
void write_left(float value) {
  servo_left.write(value);
}

void write_right(float value) {
  servo_right.write(value);
}

void write_led(bool value) {
  digitalWrite(PIN_LED, value);
}

Painter painter(write_left, write_right, write_led);

// setup() creates the servo objects, enables the LED pin, and opens a Serial port.

void setup() {
  servo_left.attach(PIN_SERVO_LEFT);
  servo_right.attach(PIN_SERVO_RIGHT);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);

  Serial.begin(115200);
  Serial.println("Hello World.");
}

// loop() calls tick() once every TICK_MILLISECONDS.
// For initial testing, it also handles user input of movement commands.

unsigned long last_tick = 0;
void loop() {
  unsigned long now = millis();
  if (now - last_tick >= TICK_MILLISECONDS) {
    painter.tick();
    last_tick = now;
  }

  while (Serial.available() > 0 && painter.acceptInput()) {
    painter.input(Serial.read());
  }
}
