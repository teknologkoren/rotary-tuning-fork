#pragma once

#include <Arduino.h>

typedef enum class HandleState : uint8_t {
  DOWN = LOW,
  UP = HIGH
} handle_state_t;

class Handle {
  public:
  void setup();
  handle_state_t loop();

  private:
  handle_state_t settled_state = HandleState::DOWN;
  int debounce_var = 0;
};
