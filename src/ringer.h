#pragma once

#include "pins.h"

class Ringer {
  public:
  void setup();
  void reset();

  // Returns false when it has been making noise for too long
  bool loop();

  private:
  unsigned int count = 0;
};
