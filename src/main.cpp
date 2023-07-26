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
bool already_played = false;

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
  if (ENABLE_WIFI && mainState != MainState::RINGING) {
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
        player.playSound("/ZH/DT.mp3");
        break;
      }

      server.startListening();
      if (server.notes_len > 0) {
        server.stopListening();
        player.putNotes(server.notes, server.notes_len);
        mainState = RINGING;
      }

      break;
    }

    // Riiing!!   Riiing!!   Riiing!!
    case RINGING: {
      if (handleState == HandleState::UP) {
        // Play the notes!
        delay(1100);
        player.playNotes(/* skip_ring= */ true);
        ringer.reset();
        server.resetNotes();
        mainState = PLAYING;
        break;
      }

      // If ringing timeout reached, go back to idle
      bool ringer_timeout = ringer.loop();
      if (!ringer_timeout) {
        ringer.reset();
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
        already_played = false;
        player.stop();
        break;
      }

      // If we are inputting something, stop playing dial tone.
      if (input.get_dial_state() != 0) {
        player.stop();
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
          player.playNotes(/* skip_ring= */ false);
          already_played = true;
        } else if (page == 0 && player.hasNotes()) {
          // Replay the last notes if the user dialed 0 (and notes have been
          // played previously).
          player.playNotes(/* skip_ring= */ already_played);
          already_played = true;
        } else {
          // Play error message.
          player.playSound("/ZH/LW.mp3");
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
        already_played = false;
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
