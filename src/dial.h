#pragma once

#include "pins.h"

#define DEBOUNCE_THRESHOLD 3

class Dial {
  public:
  void setup();
  int loop(); // Returns a number if one was entered, otherwise -1.
  void reset();
  int get_state();

  private:
  int state = 0;
  int n_of_pulses = 0;

  // Debounce variables
  int d_var = 0;
  int d_var_2 = 0;

  bool debounce(int pin, int state, int &d_var);
};
