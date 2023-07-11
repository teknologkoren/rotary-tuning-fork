#include "player.h"

void Player::setup() {
  player.begin();
  player.setVolume(5); // 100% Volume
}

// `song_notes` is an array of `len` two-character strings, corresponding to
// file names in the DY directory.
void Player::playNotes(char *song_notes[], uint8_t len) {
  // Debugging output.
  Serial.print("Isak will sing: ");
  for (int i = 0; i < len; i++) {
    Serial.print(song_notes[i]);
    if (i != len-1) {
      Serial.print(", ");
    }
  }

  player.combinationPlay(song_notes, len);
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
