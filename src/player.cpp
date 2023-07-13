#include "player.h"

void Player::setup() {
  player.begin();
  player.setVolume(5); // 100% Volume
}

// `song_notes` is an array of `len` two-character strings, corresponding to
// file names in the DY directory.
void Player::putNotes(char *song_notes[], uint8_t len) {
  for (int i = 0; i < len; i++) {
    notes[i] = song_notes[i];
  }
  n_notes = len;
}

void Player::putNotes(char song_notes[][3], uint8_t len) {
  for (int i = 0; i < len; i++) {
    notes[i] = song_notes[i];
  }
  n_notes = len;
}

void Player::playNotes() {
  // Debugging output.
  Serial.print("Isak will sing: ");
  for (int i = 0; i < n_notes; i++) {
    Serial.print(notes[i]);
    if (i == n_notes-1) {
      Serial.println();
    } else {
      Serial.print(", ");
    }
  }

  player.combinationPlay(notes, n_notes);
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
  if (player.checkPlayState() == DY::PlayState::Playing) {
    player.stop();
  }
  playing = false;
}

bool Player::hasNotes() {
  return n_notes > 0;
}
