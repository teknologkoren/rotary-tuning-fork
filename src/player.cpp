#include "player.h"

void Player::setup() {
  player.begin();
  player.setVolume(5); // 100% Volume
}

// The array needs to be of length 5
void Player::play_notes(int song_notes[]) {
  notes = song_notes;
  i = -1;
  playing = true;
}

// Returns true if still playing
bool Player::loop() {
  if (!playing) {
    return false;
  }

  // If no longer playing, start playing the next note
  if (!(player.checkPlayState() == DY::PlayState::Playing)) {
    i++;

    // No more notes, stop playing.
    if (i >= 5 || *(notes + i) == -1) {
      playing = false;
      return false;
    }

    player.playSpecified(*(notes + i));
  }

  return true;
}

void Player::stop() {
  if (player.checkPlayState() == DY::PlayState::Playing) {
    player.stop();
  }
  playing = false;
}
