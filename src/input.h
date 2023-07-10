#include "dial.h"

class Input {
  public:
  void setup();
  int loop();
  void reset();

  private:
  Dial dial;

  // 0 - waiting for number, 1 - got a number, goes back to 0 after timeout
  bool timing_out = false;
  int current_number = 0;
  unsigned long last_digit_time = 0;
};