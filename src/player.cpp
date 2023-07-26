#include <Arduino.h>
#include "player.h"

char RT_sound[3] = "RT";

void Player::setup() {
  player.begin();
  player.setVolume(20); // 100% Volume
}

// `song_notes` is an array of `len` two-character strings, corresponding to
// file names in the DY directory.
void Player::putNotes(char *song_notes[], uint8_t len) {
  notes[0] = RT_sound;
  for (int i = 0; i < len; i++) {
    notes[i+1] = song_notes[i];
  }
  n_notes = len + 1;
}

void Player::putNotes(char song_notes[][3], uint8_t len) {
  notes[0] = RT_sound;
  for (int i = 0; i < len; i++) {
    notes[i+1] = song_notes[i];
  }
  n_notes = len + 1;
}

void Player::playNotes(bool skip_ring) {
  // Debugging output.
  Serial.print("Isak will sing: ");
  int i = skip_ring ? 1 : 0;
  for (; i < n_notes; i++) {
    Serial.print(notes[i]);
    if (i == n_notes-1) {
      Serial.println();
    } else {
      Serial.print(", ");
    }
  }

  if (skip_ring) {
    player.combinationPlay(notes + 1, n_notes - 1);
  } else {
    player.combinationPlay(notes, n_notes);
  }
  playing = true;
  lastSound = 0;
}

void Player::playSound(char *path) {
  Serial.print("Requesting to play ");
  Serial.println(path);
  player.playSpecifiedDevicePath(DY::Device::Flash, path);
  playing = true;
  lastSound = 0;
}

// Returns true if still playing
bool Player::loop() {
  if (!playing) {
    return false;
  }

  if (!(player.checkPlayState() == DY::PlayState::Playing)) {
    playing = false;
    return false;
  }

  uint16_t currentSound = player.getPlayingSound();
  if (lastSound != currentSound) {
    Serial.print("Playing sound: ");
    Serial.println(currentSound);
    lastSound = currentSound;
  }

  // Delay 100 ms to avoid querying play state too often.
  delay(100);
  return true;
}

void Player::stop() {
  if (playing && player.checkPlayState() == DY::PlayState::Playing) {
    player.stop();
  }
  playing = false;
}

bool Player::hasNotes() {
  return n_notes > 0;
}
