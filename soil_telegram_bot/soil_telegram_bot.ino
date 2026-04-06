#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define WIFI_SSID     "Your_Wifi"
#define WIFI_PASSWORD "password"
#define BOT_TOKEN     "Bot_token"
#define CHAT_ID       "chat_id"

#define SOIL_PIN  D5
#define RELAY_PIN D1
#define LED_PIN   D2

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long lastCheck    = 0;
unsigned long lastBotCheck = 0;
bool pumpRunning = false;
bool alreadyNotified = false;
String lastStatus = "";          // ✅ track last status

void pumpON() {
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
  pumpRunning = true;
  Serial.println("Pump ON!");
}

void pumpOFF() {
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
  pumpRunning = false;
  Serial.println("Pump OFF!");
}

void handleMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text    = bot.messages[i].text;

    if (chat_id != CHAT_ID) continue;

    if (text == "/status") {
      int soil = digitalRead(SOIL_PIN);
      String soilStatus = (soil == HIGH) ? "🏜️ Soil is DRY" : "🌱 Soil is WET";
      String pumpStatus = pumpRunning ? "💧 Pump is ON" : "⏹️ Pump is OFF";
      bot.sendMessage(CHAT_ID, soilStatus + "\n" + pumpStatus, "");
    }
    else if (text == "/pump_on") {
      pumpON();
      bot.sendMessage(CHAT_ID, "💧 Pump turned ON manually!", "");
    }
    else if (text == "/pump_off") {
      pumpOFF();
      bot.sendMessage(CHAT_ID, "⏹️ Pump turned OFF manually!", "");
    }
    else if (text == "/start") {
      String welcome = "🌱 Plant Watering Bot\n\n"
                       "/status → Check soil + pump\n"
                       "/pump_on → Force pump ON\n"
                       "/pump_off → Force pump OFF\n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(SOIL_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pumpOFF();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected!");

  client.setInsecure();

  // Clear old messages
  int n = bot.getUpdates(bot.last_message_received + 1);
  while (n) n = bot.getUpdates(bot.last_message_received + 1);

  bot.sendMessage(CHAT_ID, "🌱 Plant Watering Bot Online!", "");
  Serial.println("Bot ready!");
}

void loop() {
  unsigned long now = millis();

  // Check soil every 30 seconds ✅
  if (now - lastCheck > 30000) {
    lastCheck = now;
    int soil = digitalRead(SOIL_PIN);
    String currentStatus = (soil == HIGH) ? "DRY" : "WET";

    // ✅ Only print if status changed
    if (currentStatus != lastStatus) {
      Serial.println("Soil: " + currentStatus);
      lastStatus = currentStatus;
    }

    if (soil == HIGH) {
      if (!pumpRunning) {
        pumpON();
        if (!alreadyNotified) {
          bot.sendMessage(CHAT_ID, "🏜️ Soil is DRY!\n💧 Auto watering started!", "");
          alreadyNotified = true;
        }
      }
    } else {
      if (pumpRunning) {
        pumpOFF();
        bot.sendMessage(CHAT_ID, "✅ Soil is WET!\n⏹️ Pump stopped!", "");
        alreadyNotified = false;
        lastStatus = "";
      }
    }
  }

  // Check Telegram every 2 seconds
  if (now - lastBotCheck > 2000) {
    lastBotCheck = now;
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages) handleMessages(numNewMessages);
  }
}