#pragma once

#define RINGER_PIN_A 22
#define RINGER_PIN_B 23

class Ringer {
  public:
  void setup();

  // Returns false when it has been making noise for too long
  bool loop();
};
