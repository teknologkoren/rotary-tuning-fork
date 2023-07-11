#include <Arduino.h>
#include "DYPlayerArduino.h"

class Player {
  public:
  Player(): player(&Serial2) {}

  void setup();

  // Returns true if still playing
  bool loop();

  void stop();

  void reset();

  // The array needs to be of length 5
  void play_notes(int notes[]);


  private:
  DY::Player player;
  int i = 0;
  int* notes;
  bool playing = false;

};