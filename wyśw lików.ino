#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DigitLedDisplay.h>

DigitLedDisplay ld = DigitLedDisplay(7, 6, 5);

const char* ssid = "";
const char* password = "";

int followy;
int liki;

void setup() {

  ld.setBright(10);
  ld.setDigitLimit(8);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {
    ld.printDigit(12345678);
  delay(500);
  ld.clear();
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient https;
    String data = "FBGraphApi";
    String fullUrl = data;
    if (https.begin(client, fullUrl)) {
      int httpCode = https.GET();
      if (httpCode > 0) {
        String input = https.getString();
        Serial.println(input);

        StaticJsonDocument<128> doc;

        DeserializationError error = deserializeJson(doc, input);

          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
          }

          int liki = doc["fan_count"]; // 1229
          int followy = doc["followers_count"]; // 1255

        Serial.print("obserwujacy: ");
        Serial.println(followy);
        Serial.print("liki: ");
        Serial.println(liki);  
        https.end();
      }
    } 
    
    else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }


    delay(1000);
  }
}
