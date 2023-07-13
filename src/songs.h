#pragma once

// We need to convert string constants to char* for the static initialization
// of the song list. But the strings will never be modified, so this is fine.
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <Arduino.h>

typedef struct {
  int page;
  char* notes[5];
  uint8_t len;
} song_t;

song_t* find_song(int page);

