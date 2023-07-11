#include <Arduino.h>

#include "config.h"
#include "handle.h"
#include "input.h"
#include "player.h"
#include "ringer.h"
#include "server.h"
#include "songs.h"
#include "pins.h"

Handle handle;
Input input;
Player player;
Ringer ringer;

void setup() {
  Serial.begin(115200);
  Serial.println("Setting up...");
  // put your setup code here, to run once:
  delay(2000);

  handle.setup();
  input.setup();
  player.setup();
  ringer.setup();

#if ENABLE_WIFI
  setup_server();
#endif

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
  handle_state_t handleState = handle.loop();

  switch (mainState) {
    // Idle state - listen for handle up and network requests.
    case IDLE: {
      if (handleState == HandleState::UP) {
        // The phone handle was lifted, start waiting for dial input.
        mainState = WAITING_FOR_INPUT;
      }
      break;
    }

    // Riiing!!   Riiing!!   Riiing!!
    case RINGING: {
      if (handleState == HandleState::UP) {
        mainState = WAITING_FOR_INPUT; // TODO implement server input
      }

      // If ringing timeout reached, go back to idle
      bool ringer_timeout = ringer.loop();
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
      }

      if (handleState == HandleState::DOWN) {
        // The handle was put down again, return to IDLE.
        mainState = IDLE;
      }
      break;
    }
    
    // Playing notes. Wait until it's done or the handle is put down.
    case PLAYING: {
      if (handleState == HandleState::DOWN) {
        // Handle is down, stop playing sounds.
        player.stop();

        // Back to IDLE
        mainState = IDLE;
      } else if (!player.loop()) {
        // Playing done. Go back to listening for input.
        mainState = WAITING_FOR_INPUT;
      }
      break;
    }
  }
}
