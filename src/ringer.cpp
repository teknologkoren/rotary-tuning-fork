#include <Arduino.h>
#include "config.h"
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
#if HAS_HARDWARE
  // If the microcontroller is connected to the rotary phone hardware,
  // ring the ringer for real!
  digitalWrite(RINGER_PIN_A, HIGH);
  digitalWrite(RINGER_PIN_B, LOW);
  delay(25);

  digitalWrite(RINGER_PIN_A, LOW);
  digitalWrite(RINGER_PIN_B, HIGH);
  delay(25);
#else
  // If it is not connected to the rotary phone hardware, mock the ringer
  // with serial output.
  Serial.println("R");
  delay(25);
  Serial.println("R");
  delay(25);
#endif
}
