#include <Arduino.h>
#include "ringer.h"

void Ringer::setup() {
  pinMode(RINGER_PIN_A, OUTPUT);
  pinMode(RINGER_PIN_B, OUTPUT);

  // ledcSetup(RINGER_PWM_CHANNEL, RINGER_FREQUENCY, 16);
  // ledcAttachPin(RINGER_PIN, RINGER_PWM_CHANNEL);
  // ledcAttachPin
}

// Returns false when it has been making noise for too long
// TODO(fix thing)
bool Ringer::loop() {
  digitalWrite(RINGER_PIN_A, HIGH);
  digitalWrite(RINGER_PIN_B, LOW);
  delay(25);

  digitalWrite(RINGER_PIN_A, LOW);
  digitalWrite(RINGER_PIN_B, HIGH);
  delay(25);
}