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
  0 - phone handle down, idle
  1 - phone handle down, ringing
  2 - phone handle up, waiting for input
  3 - phone handle up, playing notes
*/
int main_state = 0;

// HIGH = handle up
// LOW = handle down
int reciever_switch_debounce = 0;

void loop() {
  switch (main_state) {
    // Idle state - listen for handle up and network requests.
    case 0: {
      if (debounce(RECIEVER_SWITCH_PIN, HIGH, reciever_switch_debounce)) {
        main_state = 2;
        reciever_switch_debounce = 0;
      }
      break;
    }

    // Riiing!!   Riiing!!   Riiing!!
    case 1: {
      if (debounce(RECIEVER_SWITCH_PIN, HIGH, reciever_switch_debounce)) {
        main_state = 2;
        reciever_switch_debounce = 0;
      }
      bool ringer_timeout = ringer.loop();

      // If ringing timeout reached, go back to idle
      if (!ringer_timeout) {
        main_state = 2;
        reciever_switch_debounce = 0;
      }
      break;
    }

    // Wait for dial input or the handle to be put down again.
    case 2: {
      int song = input.loop();
      if (song != -1) {
        Serial.println();
        Serial.println(song);

        // Start playin'!
        player.play_notes(songs[song].notes);
        main_state = 3;
        reciever_switch_debounce = 0;
      }

      if (debounce(RECIEVER_SWITCH_PIN, LOW, reciever_switch_debounce)) {
        main_state = 0;
        reciever_switch_debounce = 0;
      }
      break;
    }
    
    // Playing notes. Wait until it's done or the handle is put down.
    case 3: {
      if (debounce(RECIEVER_SWITCH_PIN, LOW, reciever_switch_debounce)) {
        // Handle is down, stop player NOW.
        player.stop();

        // Back to IDLE
        main_state = 0;
        reciever_switch_debounce = 0;
        break;
      }

      if (player.loop()) {
        // Still playing. Delay 100 ms to avoid querying play state too often.
        delay(100);
      }
      else {
        // Playing done. Go back to listening for input.
        main_state = 2;
        reciever_switch_debounce = 0;
      }
      break;
    }
  }
}
