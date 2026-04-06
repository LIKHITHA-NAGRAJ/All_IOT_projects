#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "DHT.h"

#define WIFI_SSID     "your_wifi"
#define WIFI_PASSWORD "password"

#define DHTPIN  D4
#define DHTTYPE DHT11

const char* scriptURL = "https://script.google.com/macros/s/AKfycbzzpMzpSazkcnFN58ezn3JisLkufhc4fyanQaCqAMjfs-IlZJLZsX0o3qcO8DVBn_3Jsw/exec";

DHT dht(DHTPIN, DHTTYPE);
unsigned long lastLog = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
  Serial.println("Google Sheets Logger Ready!");
}

void logToSheets(float temp, float hum, float heatIndex) {
  if (WiFi.status() == WL_CONNECTED) {
    BearSSL::WiFiClientSecure client;
    client.setInsecure();  // skip SSL verification

    HTTPClient http;
    String url = String(scriptURL)
                 + "?temp="      + String(temp, 1)
                 + "&hum="       + String(hum, 1)
                 + "&heatindex=" + String(heatIndex, 1);

    Serial.println("Sending to Sheets: " + url);

    http.begin(client, url);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
      Serial.println("✅ Data logged to Google Sheets!");
    } else {
      Serial.println("❌ Failed: " + http.errorToString(httpCode));
    }
    http.end();
  }
}

void loop() {
  unsigned long now = millis();

  // Log every 30 seconds
  if (now - lastLog > 30000) {
    lastLog = now;

    float temp      = dht.readTemperature();
    float hum       = dht.readHumidity();
    float heatIndex = temp + 0.33 * hum - 4;

    if (isnan(temp) || isnan(hum)) {
      Serial.println("DHT11 error!");
      return;
    }

    Serial.print("Temp: "); Serial.print(temp);
    Serial.print("°C  Hum: "); Serial.print(hum);
    Serial.print("%  HeatIndex: "); Serial.println(heatIndex);

    logToSheets(temp, hum, heatIndex);
  }
}