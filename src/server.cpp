#include <Arduino.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "config.h"
#include "server.h"

void WebServer::setup() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setHostname(HOSTNAME);

  // Initialize webserver URLs
  server.on("/api/wifi-info", HTTP_GET, [](AsyncWebServerRequest *request) {
      AsyncResponseStream *response = request->beginResponseStream("application/json");
      DynamicJsonDocument json(1024);
      json["status"] = "ok";
      json["ssid"] = WiFi.SSID();
      json["ip"] = WiFi.localIP().toString();
      serializeJson(json, *response);
      request->send(response);
  });

  server.on("/api/play-notes", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncResponseStream *response =
        request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);

    if (!listening) {
      json["status"] = "busy";
      Serial.println("Got a call, but the phone was in use.");
    }
    else if (request->hasParam("notes")) {
      AsyncWebParameter *p = request->getParam("notes");
      Serial.print("Got notes from call: ");
      Serial.println(p->value());
      bool success = true;

      unsigned int str_len = p->value().length();
      for (int i = 0; i < str_len; i++) {
        char ch1 = p->value()[i];

        if (ch1 == ',') {
          continue;
        }

        if (notes_len >= 20) {
          // Too long
          success = false;
          break;
        }

        if (ch1 < 'A' || ch1 > 'G') {
          // Invalid note.
          success = false;
          break;
        }

        char ch2;
        if (i+1 == str_len || p->value()[i+1] == ',') {
          ch2 = ch1;
        } else {
          ch2 = p->value()[++i];
          if (ch2 == '#') {
            ch2 = 'S';
          } else if (ch2 == 'b') {
            ch2 = 'F';
          } else {
            // Invalid accidental
            success = false;
            break;
          }
        }

        notes[notes_len][0] = ch1;
        notes[notes_len][1] = ch2;
        notes[notes_len][2] = 0;
        notes_len++;
      }

      if (success) {
        json["status"] = "ok";
      } else {
        json["status"] = "malformed";
      }
    } else {
      json["status"] = "malformed";
      Serial.println("Did not get notes to play.");
    }

    serializeJson(json, *response);
    request->send(response);
  });

  // Start webserver
  server.begin();
}

void WebServer::startListening() {
  listening = true;
}

void WebServer::stopListening() {
  listening = false;
}

void WebServer::loop() {
  // Only check if we're connected once a second.
  if (millis() - lastConnectionCheck < 1000) {
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (lastConnected == 0 || notConnectedCounter > 0) {
      Serial.print("Wifi connected, IP address: ");
      Serial.println(WiFi.localIP());
    }
    notConnectedCounter = 0;
    lastConnected = millis();
  } else {
    Serial.println("Wifi connecting...");
    notConnectedCounter++;
    if (notConnectedCounter > 50) {  // Reset board if not connected after 50s
      Serial.println("Resetting due to Wifi not connecting...");
      ESP.restart();
    }
  }

  lastConnectionCheck = millis();
}

void WebServer::resetNotes() {
  notes_len = 0;
}
