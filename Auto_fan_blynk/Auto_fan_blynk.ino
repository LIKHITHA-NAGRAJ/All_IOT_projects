#define BLYNK_TEMPLATE_ID   "Blynk_template_id"
#define BLYNK_TEMPLATE_NAME "template_name"
#define BLYNK_AUTH_TOKEN    "blynk_token"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

#define DHTPIN    26
#define DHTTYPE   DHT11
#define RELAY_PIN 27

char ssid[] = "Your_wifi";
char pass[] = "password";

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

float threshold = 30.0;
bool manualOverride = false;
bool manualState = false;

// Manual override from Blynk app
BLYNK_WRITE(V4) {
  int val = param.asInt();
  if (val == 1) {
    manualOverride = true;
    manualState = true;
    digitalWrite(RELAY_PIN, LOW);   // force fan ON
    Serial.println("Manual ON!");
  } else {
    manualOverride = false;
    manualState = false;
    digitalWrite(RELAY_PIN, HIGH);  // force fan OFF
    Serial.println("Manual OFF!");
  }
}

void readAndControl() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT11 error!");
    return;
  }

  Serial.print("Temp: "); Serial.print(temp);
  Serial.print("°C  Hum: "); Serial.print(hum);
  Serial.println("%");

  // Send to Blynk
  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, hum);

  // Auto control only if no manual override
  if (!manualOverride) {
    if (temp > threshold) {
      digitalWrite(RELAY_PIN, LOW);   // fan ON
      Blynk.virtualWrite(V3, 1);
      Serial.println("🌀 Fan AUTO ON!");
    } else {
      digitalWrite(RELAY_PIN, HIGH);  // fan OFF
      Blynk.virtualWrite(V3, 0);
      Serial.println("Fan AUTO OFF");
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // start OFF

  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, readAndControl);
  Serial.println("Auto Fan System Ready!");
}

void loop() {
  Blynk.run();
  timer.run();
}