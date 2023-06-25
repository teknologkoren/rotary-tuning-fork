#include <Arduino.h>
#include "DYPlayerArduino.h"

#include "server.h"
#include "songs.h"

// Blue wire on rotary thingy
// #define PULSES_PIN 2
#define PULSES_PIN 18

// White wire on rotary thingy
// #define ENABLE_PIN 3
#define ENABLE_PIN 19
#define RECIEVER_SWITCH_PIN 21

#define LED_PIN 17

#define DEBOUNCE_THRESHOLD 3

DY::Player player(&Serial2);

void play_song(int index) {
  for(int i = 0; i < 5; i++) {
    if (songs[index].notes[i] == -1) {
      break;
    }

    player.playSpecified(songs[index].notes[i]);
    while (player.checkPlayState() == DY::PlayState::Playing) {
      delay(100);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Setting up...");
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  pinMode(PULSES_PIN, INPUT_PULLUP);
  pinMode(ENABLE_PIN, INPUT_PULLUP);
  pinMode(RECIEVER_SWITCH_PIN, INPUT_PULLUP);
  player.begin();
  player.setVolume(15); // 100% Volume
  delay(2000);
  // setup_server();
  Serial.println("Setup finished.");
}

int state = 0;
int n_of_pulses = 0;
int i_printed = 1;
int d_var = 0;
int d_var_2 = 0;

bool debounce(int pin, int state, int &d_var) {
  if (digitalRead(pin) == state) {
    d_var += 1;
    if (d_var > DEBOUNCE_THRESHOLD) {
      d_var = 0;
      d_var_2 = 0;
      return true;
    }
  } else {
    d_var = 0;
  }
  return false;
}

int accumulated_page;
void process_number(int number) {
  accumulated_page = accumulated_page * 10 + number;
}

void loop() {
  // Serial.println(digitalRead(RECIEVER_SWITCH_PIN));
  // while (true) {
    // Serial.println((int) player.checkPlayState());
  //   delay(1000);
  // }

  switch(state) {
    case 0:
      if (i_printed == 0) {
        // Serial.print(n_of_pulses - 1);
        i_printed = 1;
        play_song(n_of_pulses - 1);
      }
      digitalWrite(LED_PIN, HIGH); // Turn LED off
      n_of_pulses = 0;
      if (debounce(ENABLE_PIN, LOW, d_var)) {
        state = 1;
      }
    break;
    case 1:
      if (debounce(PULSES_PIN, HIGH, d_var)) {
        digitalWrite(LED_PIN, LOW);
        n_of_pulses += 1;
        state = 2;
      }

      if (debounce(ENABLE_PIN, HIGH, d_var_2)) { // Dial came back
        i_printed = 0;
        state = 0;
      }
    break;
    case 2:
      if (debounce(PULSES_PIN, LOW, d_var)) {
        digitalWrite(LED_PIN, HIGH);
        state = 1;
      }

      if (debounce(ENABLE_PIN, HIGH, d_var_2)) {
        i_printed = 0;
        state = 0;
      }
    break;
  }

  delay(1);
}
