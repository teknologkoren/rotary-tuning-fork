#include <Arduino.h>
#include "serial_input.h"

bool hasDigitInput() {
  if (Serial.available() > 0) {
    int next = Serial.peek();
    if (next >= '0' && next <= '9') {
      return true;
    }
  }
  return false;
}

int popDigitInput() {
  int res = Serial.read();
  Serial.print(res - '0');
  return res - '0';
}

bool hasCharInput() {
  if (Serial.available() > 0) {
    int next = Serial.peek();
    if (next >= '0' && next <= '9') {
      return false;
    }
    return true;
  }
  return false;
}

char popCharInput() {
  char res = Serial.read();
  Serial.print(res);
  return res;
}