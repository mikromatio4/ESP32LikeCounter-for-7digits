#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <LedController.hpp>

#define LED_CONTROL_CLK_PIN D5
#define LED_CONTROL_DATA_PIN D6
#define LED_CONTROL_CS_PIN D7
#define LED_CONTROL_NUM_SEGMENTS 2

LedController ledController(LED_CONTROL_DATA_PIN, LED_CONTROL_CLK_PIN, LED_CONTROL_CS_PIN, LED_CONTROL_NUM_SEGMENTS);


const char* ssid = "SSID";
const char* password = "PASSWORD";

int followy;
int liki;
int jasn = 15;

void display(const char* value, unsigned int segment = 0) {
  ledController.clearSegment(segment);

  uint8_t dotsNumber = 0;
  uint8_t length = (uint8_t)strlen(value);

  for (uint8_t i = 0; i < length; i++) {
    if (value[i] == '.') {
      dotsNumber++;
    }
  }

  length = length - dotsNumber > 8 ? 8 + dotsNumber : length;

  uint8_t i = 0;
  bool withDot = false;
  uint8_t dotShift = 0;

  while (i < length) {
    uint8_t key = length - i - 1;
    uint8_t pos = i - dotShift;

    if (value[key] == '.') {
      withDot = true;
      dotShift++;
    } else {
      ledController.setChar(segment, pos, value[key], withDot);
      if (withDot) {
        withDot = false;
      }
    }

    i++;
  }
}

void setup() {

  pinMode(D1, INPUT);
  attachInterrupt(5, jasnoscplus, RISING);


  ledController.activateAllSegments();
  ledController.setIntensity(15);
  ledController.clearMatrix();

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    display("0.0.0.0.0.0.0.0.");
    delay(200);
    ledController.clearMatrix();
    display("0.0.0.0.0.0.0.0.", 1);
    delay(200);
    ledController.clearMatrix();
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}




void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient https;
    String data = "FB API";
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

        int liki = doc["fan_count"];  // 1229
        int followy = doc["followers_count"];
        // 1255
        char likes[8];
        sprintf(likes, "%-8d", liki);
        char follows[8];
        sprintf(follows, "%-8d", followy);
        display(likes);
        display(follows, 1);
        Serial.print("obserwujacy: ");
        Serial.println(followy);
        Serial.print("liki: ");
        Serial.println(liki);
        Serial.print("jasnosc: ");
        Serial.println(jasn);
        https.end();
        delay(10000);
      }
    }

    else {
      Serial.printf("[HTTPS] Unable to connect\n");
      ledController.setIntensity(15);
      display("0.0.0.0.0.0.0.0.");
      delay(200);
      ledController.clearMatrix();
      display("0.0.0.0.0.0.0.0.", 1);
      delay(200);
      ledController.clearMatrix();
    }
  }
}
IRAM_ATTR void jasnoscplus() {
  {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 200) {
      jasn++;
      jasn = jasn + 2;
      if (jasn > 15) {
        jasn = 0;
      }
      ledController.setIntensity(jasn);
    }
    last_interrupt_time = interrupt_time;
  }
}
