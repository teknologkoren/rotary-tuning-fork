#include <Arduino.h>
#include "DYPlayerArduino.h"

// Blue wire on rotary thingy
#define PULSES_PIN 2

// White wire on rotary thingy
#define ENABLE_PIN 3

#define LED_PIN 17

#define DEBOUNCE_THRESHOLD 3

typedef struct {
  int page;
  int notes[5];
} song_t;

DY::Player player(&Serial1);

song_t songs[] = {
  {10, {9, 4, 14, 4, -1}},
  {100, {8, -1, -1, -1, -1}},
  {106, {15, 15, 4, 15, -1}},
  {108, {6, 18, 6, 6, -1}},
  {11, {15, -1, -1, -1, -1}},
  {110, {15, -1, -1, -1, -1}},
  {113, {11, -1, -1, -1, -1}},
  {114, {18, 7, 18, -1, -1}},
  {116, {11, -1, -1, -1, -1}},
  {118, {4, 9, -1, -1, -1}},
  {12, {18, -1, -1, -1, -1}},
  {120, {18, 11, 7, 18, -1}},
  {122, {4, 15, 8, 15, -1}},
  {124, {11, -1, -1, -1, -1}},
  {127, {3, 13, 8, 3, -1}},
  {128, {18, -1, -1, -1, -1}},
  {132, {14, -1, -1, -1, -1}},
  {134, {18, 18, 7, 11, 18}},
  {136, {15, 8, 3, 15, -1}},
  {138, {18, 14, 8, 8, -1}},
  {14, {8, 3, 8, 13, 3}},
  {140, {16, 9, 9, 9, -1}},
  {145, {15, -1, -1, -1, -1}},
  {148, {18, -1, -1, -1, -1}},
  {152, {8, 14, 8, 18, 8}},
  {156, {15, -1, -1, -1, -1}},
  {160, {6, 15, 11, 6, -1}},
  {162, {13, 8, 3, 3, -1}},
  {165, {7, 2, 7, 14, -1}},
  {168, {11, 7, 18, 18, -1}},
  {17, {15, 8, 4, 15, -1}},
  {170, {8, -1, -1, -1, -1}},
  {176, {4, -1, -1, -1, -1}},
  {177, {4, -1, -1, -1, -1}},
  {178, {18, 11, 6, 18, -1}},
  {179, {16, 9, 5, 16, -1}},
  {18, {8, -1, -1, -1, -1}},
  {180, {18, -1, -1, -1, -1}},
  {183, {3, 15, 8, -1, -1}},
  {186, {11, -1, -1, -1, -1}},
  {188, {8, -1, -1, -1, -1}},
  {21, {8, -1, -1, -1, -1}},
  {22, {14, -1, -1, -1, -1}},
  {24, {9, 4, -1, -1, -1}},
  {26, {16, 11, -1, -1, -1}},
  {27, {13, -1, -1, -1, -1}},
  {30, {11, -1, -1, -1, -1}},
  {32, {15, -1, -1, -1, -1}},
  {34, {8, -1, -1, -1, -1}},
  {40, {8, 18, 14, 8, -1}},
  {42, {8, -1, -1, -1, -1}},
  {43, {14, -1, -1, -1, -1}},
  {44, {11, 7, 18, 18, -1}},
  {46, {14, -1, -1, -1, -1}},
  {48, {14, 4, -1, -1, -1}},
  {49, {18, -1, -1, -1, -1}},
  {54, {18, -1, -1, -1, -1}},
  {56, {11, -1, -1, -1, -1}},
  {62, {14, -1, -1, -1, -1}},
  {63, {14, 7, 18, 14, -1}},
  {64, {4, -1, -1, -1, -1}},
  {66, {8, 13, 3, 3, -1}},
  {68, {18, -1, -1, -1, -1}},
  {69, {15, 10, 3, 10, -1}},
  {70, {8, 18, 14, 8, 8}},
  {72, {8, -1, -1, -1, -1}},
  {73, {13, 13, 18, 13, -1}},
  {74, {8, -1, -1, -1, -1}},
  {76, {8, 4, 15, 15, -1}},
  {78, {7, 2, 14, 7, -1}},
  {79, {11, -1, -1, -1, -1}},
  {8, {14, -1, -1, -1, -1}},
  {80, {11, -1, -1, -1, -1}},
  {81, {14, 8, 4, 4, -1}},
  {82, {16, -1, -1, -1, -1}},
  {83, {10, 3, 15, 10, -1}},
  {84, {8, -1, -1, -1, -1}},
  {85, {8, 8, 13, 3, -1}},
  {86, {4, 8, 4, 15, -1}},
  {87, {14, 8, 14, 4, -1}},
  {9, {14, -1, -1, -1, -1}},
  {90, {4, 15, 8, 15, -1}},
  {92, {6, -1, -1, -1, -1}},
  {94, {11, -1, -1, -1, -1}},
  {96, {8, -1, -1, -1, -1}},
  {98, {8, -1, -1, -1, -1}},
  {-1, {-1, -1, -1, -1, -1}}
};

void play_song(int index) {
  for(int i = 0; i < 5; i++) {
    if (songs[index].notes[i] == -1) {
      break;
    }
    player.playSpecified(songs[index].notes[i]);
    delay(2200);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.print("Setting up...");
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  pinMode(PULSES_PIN, INPUT_PULLUP);
  pinMode(ENABLE_PIN, INPUT_PULLUP);
  player.begin();
  player.setVolume(6); // 100% Volume
  delay(2000);
  Serial.print("Setup finished.");
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
