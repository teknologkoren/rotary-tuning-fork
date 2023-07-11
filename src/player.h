#pragma once

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

  // `song_notes` is an array of `len` two-character strings, corresponding to
  // file names in the DY directory.
  void playNotes(char *song_notes[], uint8_t len);
  void playSound(char *path);


  private:
  DY::Player player;
  bool playing = false;
  uint16_t lastSound = 0;
};