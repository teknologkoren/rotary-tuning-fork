#include <Arduino.h>

#include "config.h"
#include "handle.h"
#include "input.h"
#include "player.h"
#include "ringer.h"
#include "server.h"
#include "songs.h"
#include "pins.h"

/*
  Main state variable.
  IDLE - phone handle down
  RINGING - phone handle down, ringing
  WAITING_FOR_DIAL - phone handle up, waiting for dial input
  PLAYING - phone handle up, playing sounds
*/
enum MainState {IDLE, RINGING, WAITING_FOR_INPUT, PLAYING} mainState;

Handle handle;
Input input;
Player player;
Ringer ringer;
WebServer server;

void setup() {
  Serial.begin(115200);
  Serial.println("Setting up...");

  handle.setup();
  input.setup();
  player.setup();
  ringer.setup();

  if (ENABLE_WIFI) {
    server.setup();
  }

  Serial.println("Setup finished.");
}

void loop() {
  if (ENABLE_WIFI) {
    server.loop();
  }

  handle_state_t handleState = handle.loop();

  switch (mainState) {
    // Idle state - listen for handle up and network requests.
    case IDLE: {
      if (handleState == HandleState::UP) {
        // The phone handle was lifted, start waiting for dial input.
        mainState = WAITING_FOR_INPUT;
        server.stopListening();
        break;
      }

      server.startListening();
      if (server.notes_len > 0) {
        server.stopListening();
        mainState = RINGING;
      }

      break;
    }

    // Riiing!!   Riiing!!   Riiing!!
    case RINGING: {
      if (handleState == HandleState::UP) {
        // Play the notes!
        player.putNotes(server.notes, server.notes_len);
        player.playNotes();
        server.resetNotes();
        mainState = PLAYING;
      }

      // If ringing timeout reached, go back to idle
      bool ringer_timeout = ringer.loop();
      if (!ringer_timeout) {
        server.resetNotes();
        mainState = IDLE;
      }

      break;
    }

    // Wait for dial input or the handle to be put down again.
    case WAITING_FOR_INPUT: {
      if (handleState == HandleState::DOWN) {
        // The handle was put down again, return to IDLE.
        mainState = IDLE;
        break;
      }

      // If we got a page number, try to play the starting notes.
      int page = input.loop();
      if (page != -1) {
        Serial.println();
        Serial.println(page);

        song_t* song = find_song(page);

        if (song->page == page) {
          // Start playin'!
          player.putNotes(song->notes, song->len);
          player.playNotes();
        } else if (page == 0 && player.hasNotes()) {
          // Replay the last notes if the user dialed 0 (and notes have been
          // played previously).
          player.playNotes();
        } else {
          // Play error message.
          player.playSound("err.mp3");
        }

        mainState = PLAYING;
      }

      break;
    }
    
    // Playing notes. Wait until it's done or the handle is put down.
    case PLAYING: {
      if (handleState == HandleState::DOWN) {
        // Handle is down, stop playing sounds and go back to IDLE.
        player.stop();
        mainState = IDLE;
        break;
      }

      if (!player.loop()) {
        // Playing done. Go back to listening for input.
        mainState = WAITING_FOR_INPUT;
      }

      break;
    }
  }
}
