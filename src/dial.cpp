#include <Arduino.h>
#include "config.h"
#include "dial.h"
#include "serial_input.h"

bool Dial::debounce(int pin, int state, int &d_var) {
  if (digitalRead(pin) == state) {
    d_var += 1;
    if (d_var > DEBOUNCE_THRESHOLD) {
      d_var = 0;
      d_var_2 = 0;
      return true;
    }
  } else {
    d_var = 0;
  }
  return false;
}

void Dial::setup() {
  pinMode(PULSES_PIN, INPUT_PULLUP);
  pinMode(ENABLE_PIN, INPUT_PULLUP);
}

void Dial::reset() {
  state = 0;
  n_of_pulses = 0;
  d_var = 0;
  d_var_2 = 0;
}

int Dial::get_state() {
  return state;
}

int Dial::loop() {
#if HAS_HARDWARE

  // If the ESP32 is connected to the rotary phone hardware, read the dial.
  switch(state) {
    case 0:
      n_of_pulses = 0;
      if (debounce(ENABLE_PIN, LOW, d_var)) {
        state = 1;
      }
    break;
    case 1:
      if (debounce(PULSES_PIN, HIGH, d_var)) {
        n_of_pulses += 1;
        state = 2;
      }

      if (debounce(ENABLE_PIN, HIGH, d_var_2)) { // Dial came back
        state = 0;
        Serial.print(n_of_pulses - 1);
        return n_of_pulses - 1;
      }
    break;
    case 2:
      if (debounce(PULSES_PIN, LOW, d_var)) {
        state = 1;
      }

      if (debounce(ENABLE_PIN, HIGH, d_var_2)) {
        state = 0;
        Serial.print(n_of_pulses - 1);
        return n_of_pulses - 1;
      }
    break;
  }
  delay(1);

#else

  // If the ESP32 is not connected to the rotary phone hardware, mock the dial
  // with serial input.
  if (hasDigitInput()) {
    return popDigitInput();
  }
  return -1;

#endif
}
