#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <ESP8266HTTPClient.h>

#define WIFI_SSID     "Your_wifi"
#define WIFI_PASSWORD "password"
#define BOT_TOKEN     "Bot_token"
#define CHAT_ID       "chat_id"

String apiKey = "Write_api_key";

#define RAIN_PIN   D5
#define SERVO_PIN  D4
#define BUZZER_PIN D6
#define LED_PIN    D7

#define COVER_ANGLE 0      // ✅ keeping your angles
#define OPEN_ANGLE  180    // ✅ keeping your angles

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);
Servo servo;

unsigned long lastBotCheck   = 0;
unsigned long lastCheck      = 0;
unsigned long lastThingSpeak = 0;
bool alreadyNotified = false;
bool clothesCovered  = false;
int lastRainVal = -1;

void coverClothes() {
  servo.write(COVER_ANGLE);
  clothesCovered = true;
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000, 500);
  Serial.println("Clothes COVERED!");
}

void openClothes() {
  servo.write(OPEN_ANGLE);
  clothesCovered = false;
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
  Serial.println("Clothes OPEN!");
}

void sendToThingSpeak(int rainStatus) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient tsClient;
    HTTPClient http;
    String url = "http://api.thingspeak.com/update?api_key="
                 + apiKey + "&field1=" + String(rainStatus);
    if (http.begin(tsClient, url)) {
      int httpCode = http.GET();
      Serial.println(httpCode > 0 ? "ThingSpeak updated!" : "ThingSpeak failed!");
      http.end();
    }
  }
}

void handleMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text    = bot.messages[i].text;

    if (chat_id != CHAT_ID) continue;

    if (text == "/status") {
      int rain = digitalRead(RAIN_PIN);
      String rainStatus  = (rain == LOW) ? "🌧️ RAINING" : "☀️ No Rain";  // ✅
      String clothStatus = clothesCovered ? "🧺 Clothes COVERED" : "👕 Clothes OPEN";
      bot.sendMessage(CHAT_ID, rainStatus + "\n" + clothStatus, "");
    }
    else if (text == "/cover") {
      coverClothes();
      bot.sendMessage(CHAT_ID, "🧺 Clothes covered manually!", "");
    }
    else if (text == "/open") {
      openClothes();
      bot.sendMessage(CHAT_ID, "👕 Clothes opened manually!", "");
    }
    else if (text == "/start") {
      bot.sendMessage(CHAT_ID,
        "🌧️ Smart Cloth Protector Bot\n\n"
        "/status → Rain + cloth status\n"
        "/cover  → Cover clothes manually\n"
        "/open   → Open clothes manually\n"
        "Auto covers when rain detected!", "");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RAIN_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  servo.attach(SERVO_PIN);
  servo.write(OPEN_ANGLE);
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected!");

  client.setInsecure();

  int n = bot.getUpdates(bot.last_message_received + 1);
  while (n) n = bot.getUpdates(bot.last_message_received + 1);

  bot.sendMessage(CHAT_ID, "🌧️ Smart Cloth Protector Online!\nSend /start for commands", "");
  Serial.println("Ready!");

  lastRainVal = digitalRead(RAIN_PIN);
  Serial.print("Initial rain value: ");
  Serial.println(lastRainVal);
}
void loop() {
  unsigned long now = millis();

  if (now - lastCheck > 3000) {
    lastCheck = now;
    int rain = digitalRead(RAIN_PIN);

    Serial.print("Rain pin: ");
    Serial.println(rain);

    // ✅ Remove lastRainVal check — always act on current state!
    if (rain == LOW) {
      Serial.println("RAINING!");
      if (!clothesCovered) {
        coverClothes();
        if (!alreadyNotified) {
          bot.sendMessage(CHAT_ID,
            "🌧️ RAIN DETECTED!\n"
            "🧺 Clothes being covered!\n"
            "☂️ Carry umbrella!", "");
          alreadyNotified = true;
        }
      }
    } else {
      Serial.println("CLEAR!");
      if (clothesCovered) {        // ✅ only open if currently covered
        openClothes();
        bot.sendMessage(CHAT_ID,
          "☀️ Rain stopped!\n"
          "👕 Clothes uncovered!\n"
          "🌈 All clear!", "");
        alreadyNotified = false;
      }
    }
  }

  // ThingSpeak every 20 seconds
  if (now - lastThingSpeak > 20000) {
    lastThingSpeak = now;
    int rain = digitalRead(RAIN_PIN);
    sendToThingSpeak(rain == LOW ? 1 : 0);
  }

  // Telegram every 2 seconds
  if (now - lastBotCheck > 2000) {
    lastBotCheck = now;
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages) handleMessages(numNewMessages);
  }
}
