#include "input.h"
#include <Arduino.h>

void Input::setup() {
  dial.setup();
}

void Input::reset() {
  dial.reset();
  timing_out = false;
  current_number = 0;
  last_digit_time = 0;
}

int Input::loop() {
  int new_digit = dial.loop();

  if (new_digit != -1) {
    // We got a new digit!
    current_number = current_number*10 + new_digit;

    // Record the current time, so we can time out if the user is done.
    timing_out = true;
    last_digit_time = millis();
  }

  // If the user starts to dial another digit, stop timing out.
  if (dial.get_state() != 0) {
    timing_out = false;
  }

  // If the user has not started dialing another digit for an amount of time,
  // return the entered number
  if (timing_out && (millis() - last_digit_time > 2000)) {
    int result = current_number;
    reset();
    return result;
  }
  return -1;
}