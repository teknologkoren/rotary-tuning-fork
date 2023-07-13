#pragma once

#include <ESPAsyncWebServer.h>

class WebServer {
  public:
  WebServer() : server(80) {}
  void setup();
  void loop();
  void startListening();
  void stopListening();
  void resetNotes();

  char notes[20][3];
  uint8_t notes_len = 0;

  private:
  unsigned long lastConnectionCheck = 0;
  unsigned long lastConnected = 0;
  uint32_t notConnectedCounter = 0;
  AsyncWebServer server;

  bool listening = true;
};
