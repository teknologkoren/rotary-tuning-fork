#include <Arduino.h>
#include "config.h"
#include "handle.h"
#include "pins.h"
#include "serial_input.h"

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
  if (HAS_HARDWARE) {
    // If we have hardware, read the state of the reciever switch.
    if (settled_state == HandleState::DOWN) {
      if (debounce(RECIEVER_SWITCH_PIN, HandleState::UP, debounce_var)) {
        settled_state = HandleState::UP;
      }
    } else if (settled_state == HandleState::UP) {
      if (debounce(RECIEVER_SWITCH_PIN, HandleState::DOWN, debounce_var)) {
        settled_state = HandleState::DOWN;
      }
    }
  } else {
    // Simulate picking up and putting down the handle by typing 'u' or 'd'
    // over serial.
    if (hasCharInput()) {
      char ch = popCharInput();
      if (ch == 'u') {
        settled_state = HandleState::UP;
        Serial.println("\nHandle up!");
      }
      if (ch == 'd') {
        settled_state = HandleState::DOWN;
        Serial.println("\nHandle down!");
      }
    }

    // Discard numbers if handle is down.
    if (settled_state == HandleState::DOWN) {
      while (hasDigitInput()) {
        popDigitInput();
      }
    }
  }

  return settled_state;
}
