#include <Arduino.h>

#include "input.h"
#include "player.h"
#include "ringer.h"
#include "server.h"
#include "songs.h"
#include "pins.h"

Player player;
Input input;
Ringer ringer;

bool debounce(int pin, int state, int &d_var) {
  if (digitalRead(pin) == state) {
    d_var += 1;
    if (d_var > DEBOUNCE_THRESHOLD) {
      d_var = 0;
      return true;
    }
  } else {
    d_var = 0;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Setting up...");
  // put your setup code here, to run once:
  pinMode(RECIEVER_SWITCH_PIN, INPUT_PULLUP);
  delay(2000);
  setup_server();
  input.setup();

  // Setup ringing
  ringer.setup();

  Serial.println("Setup finished.");
}

/*
  Main state variable.
  IDLE - phone handle down
  RINGING - phone handle down, ringing
  WAITING_FOR_DIAL - phone handle up, waiting for dial input
  PLAYING - phone handle up, playing sounds
*/
enum MainState {IDLE, RINGING, WAITING_FOR_INPUT, PLAYING} mainState;

// HIGH = handle up
// LOW = handle down
int reciever_switch_debounce = 0;

void loop() {
  switch (mainState) {
    // Idle state - listen for handle up and network requests.
    case IDLE: {
      if (debounce(RECIEVER_SWITCH_PIN, HIGH, reciever_switch_debounce)) {
        mainState = WAITING_FOR_INPUT;
        reciever_switch_debounce = 0;
      }
      break;
    }

    // Riiing!!   Riiing!!   Riiing!!
    case RINGING: {
      if (debounce(RECIEVER_SWITCH_PIN, HIGH, reciever_switch_debounce)) {
        mainState = WAITING_FOR_INPUT; // TODO implement server input
        reciever_switch_debounce = 0;
      }
      bool ringer_timeout = ringer.loop();

      // If ringing timeout reached, go back to idle
      if (!ringer_timeout) {
        mainState = IDLE;
        reciever_switch_debounce = 0;
      }
      break;
    }

    // Wait for dial input or the handle to be put down again.
    case WAITING_FOR_INPUT: {
      int page = input.loop();

      // If we got a page number, try to play the starting notes.
      if (page != -1) {
        Serial.println();
        Serial.println(page);

        song_t song = find_song(page);

        if (song.page == page) {
          // Start playin'!
          player.playNotes(song.notes, song.len);
        } else {
          // Play error message.
          player.playSound("err.mp3");
        }

        mainState = PLAYING;
        reciever_switch_debounce = 0;
      }

      // The handle was put down again, return to IDLE.
      if (debounce(RECIEVER_SWITCH_PIN, LOW, reciever_switch_debounce)) {
        mainState = IDLE;
        reciever_switch_debounce = 0;
      }
      break;
    }
    
    // Playing notes. Wait until it's done or the handle is put down.
    case PLAYING: {
      if (debounce(RECIEVER_SWITCH_PIN, LOW, reciever_switch_debounce)) {
        // Handle is down, stop player NOW.
        player.stop();

        // Back to IDLE
        mainState = IDLE;
        reciever_switch_debounce = 0;
        break;
      }

      if (!player.loop()) {
        // Playing done. Go back to listening for input.
        mainState = WAITING_FOR_INPUT;
        reciever_switch_debounce = 0;
      }
      break;
    }
  }
}
