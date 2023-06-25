#include <Arduino.h>
#include "DYPlayerArduino.h"

#include "dial.h"
#include "server.h"
#include "songs.h"


#define RECIEVER_SWITCH_PIN 21

#define LED_PIN 17

DY::Player player(&Serial2);
Dial dial;

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
  pinMode(RECIEVER_SWITCH_PIN, INPUT_PULLUP);
  player.begin();
  player.setVolume(15); // 100% Volume
  delay(2000);
  // setup_server();
  dial.setup();
  Serial.println("Setup finished.");
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
  int song = dial.loop();
  if (song != -1) {
    play_song(song);
  }
}
