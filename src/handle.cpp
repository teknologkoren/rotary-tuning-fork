#include <Arduino.h>
#include "config.h"
#include "handle.h"
#include "pins.h"

bool debounce(int pin, handle_state_t state, int &d_var) {
  if (digitalRead(pin) == (int) state) {
    d_var += 1;
    if (d_var > DEBOUNCE_THRESHOLD) {
      d_var = 0;
      return true;
    }
  } else {
    d_var = 0;
  }
  return false;
}

void Handle::setup() {
  pinMode(RECIEVER_SWITCH_PIN, INPUT_PULLUP);
}

handle_state_t Handle::loop() {
  if (settled_state == HandleState::DOWN) {
    if (debounce(RECIEVER_SWITCH_PIN, HandleState::UP, debounce_var)) {
      settled_state = HandleState::UP;
    }
  } else if (settled_state == HandleState::UP) {
    if (debounce(RECIEVER_SWITCH_PIN, HandleState::DOWN, debounce_var)) {
      settled_state = HandleState::DOWN;
    }
  }

  return settled_state;
}
