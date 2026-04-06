#include <WiFi.h>
#include <HTTPClient.h>

#define LDR_DO    26
#define RELAY_PIN 27

const char* ssid     = "Redmi Note 13 5G";
const char* password = "12345678";

String apiKey    = "JVQ00L4X1GG92OEC";
unsigned long lastUpload = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LDR_DO, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void sendToThingSpeak(int lightStatus, int relayState) {
  HTTPClient http;
  String url = "http://api.thingspeak.com/update?api_key="
               + apiKey
               + "&field1=" + String(lightStatus)
               + "&field2=" + String(relayState);  // added field2
  http.begin(url);
  http.GET();
  http.end();
  Serial.println("ThingSpeak updated!");
}

void loop() {
  int dark = digitalRead(LDR_DO);
  int relayState = 0;

  if (dark == LOW) {
    digitalWrite(RELAY_PIN, LOW);   // light ON
    relayState = 1;
    Serial.println("DARK — Light ON!");
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // light OFF
    relayState = 0;
    Serial.println("BRIGHT — Light OFF!");
  }

  if (millis() - lastUpload > 15000) {
    sendToThingSpeak(dark, relayState);
    lastUpload = millis();
  }

  delay(500);
}