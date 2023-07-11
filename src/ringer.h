#pragma once

#include "pins.h"

class Ringer {
  public:
  void setup();

  // Returns false when it has been making noise for too long
  bool loop();
};
