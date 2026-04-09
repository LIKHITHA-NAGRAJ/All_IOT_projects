#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

// ── WiFi ──────────────────────────────────────────
const char* ssid     = "Your_Wifi_Name";
const char* password = "password";

// ── Flask Server ───────────────────────────────────
const char* flask_url = "Your_flask_url";

// ── Azure IoT Hub MQTT ─────────────────────────────
const char* mqtt_server   = "my-sensor-hub.azure-devices.net";
const int   mqtt_port     = 8883;
const char* device_id     = "esp32-sensor-01";
const char* mqtt_username = "my-sensor-hub.azure-devices.net/esp32-sensor-01/"
                            "?api-version=2021-04-12";
const char* sas_token     = "Your_sas_tokenSharedAccessSignature sr=..."; // your token
const char* pub_topic     = "devices/esp32-sensor-01/messages/events/";

// ── Sensor ─────────────────────────────────────────
#define DHTPIN  4             // D4 on NodeMCU = GPIO2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiClientSecure espClient;
PubSubClient     client(espClient);

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println(" Connected! IP: " + WiFi.localIP().toString());
}

void connectMQTT() {
  espClient.setInsecure();     // skip cert verify for dev
  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()) {
    Serial.print("Connecting to IoT Hub...");
    if (client.connect(device_id, mqtt_username, sas_token)) {
      Serial.println(" Connected!");
    } else {
      Serial.print(" Failed rc=");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

void sendToFlask(float temp, float hum) {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClient wifiClient;        // plain HTTP — no SSL needed for Flask
  HTTPClient http;
  http.begin(wifiClient, flask_url);
  http.addHeader("Content-Type", "application/json");

  String body = "{\"temperature\":" + String(temp, 1) +
                ",\"humidity\":"    + String(hum, 1)  +
                ",\"deviceId\":\"esp8266-sensor-01\"}";

  int code = http.POST(body);
  Serial.println("Flask POST → HTTP " + String(code));
  http.end();
}

void sendToAzure(float temp, float hum) {
  if (!client.connected()) connectMQTT();
  client.loop();

  StaticJsonDocument<200> doc;
  doc["deviceId"]    = device_id;
  doc["temperature"] = temp;
  doc["humidity"]    = hum;

  char payload[200];
  serializeJson(doc, payload);
  client.publish(pub_topic, payload);
  Serial.println("Azure MQTT → " + String(payload));
}

void setup() {
  Serial.begin(115200);
  delay(100);
  dht.begin();
  connectWiFi();
  connectMQTT();
}

void loop() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (!isnan(temp) && !isnan(hum)) {
    sendToFlask(temp, hum);
    sendToAzure(temp, hum);
    Serial.println("Temp: " + String(temp) + "C  Hum: " + String(hum) + "%");
  } else {
    Serial.println("DHT11 read failed — check wiring!");
  }

  delay(5000);
}