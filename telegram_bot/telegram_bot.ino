#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define WIFI_SSID     "Your_wifi"
#define WIFI_PASSWORD "password"
#define BOT_TOKEN     "Bot_token"
#define CHAT_ID       "chat_id"

#define RELAY_PIN D1
#define LED_PIN   D2

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long lastTimeBotRan = 0;
const int botRequestDelay = 2000;  
bool relayState = false;

void handleMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text    = bot.messages[i].text;

    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user!", "");
      continue;
    }

    if (text == "/on") {
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_PIN, HIGH);   // ✅ HIGH = ON for LED
      relayState = true;
      bot.sendMessage(CHAT_ID, "✅ Light ON!", "");
      Serial.println("LED + Relay → ON");
    }
    else if (text == "/off") {
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(LED_PIN, LOW);    // ✅ LOW = OFF for LED
      relayState = false;
      bot.sendMessage(CHAT_ID, "❌ Light OFF!", "");
      Serial.println("LED + Relay → OFF");
    }
    else if (text == "/status") {
      String status = relayState ? "💡 Light is ON" : "🌑 Light is OFF";
      bot.sendMessage(CHAT_ID, status, "");
    }
    else if (text == "/start") {
      String welcome = "👋 Hey Likhitha!!\n"
                       "I am your Home Bot 🤖\n\n"
                       "Commands:\n"
                       "/on → Turn Light ON\n"
                       "/off → Turn Light OFF\n"
                       "/status → Check status\n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }
    else {
      bot.sendMessage(CHAT_ID, "❓ Unknown command! Send /start for help", "");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // relay OFF at start
  digitalWrite(LED_PIN, LOW);     // LED OFF at start

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  client.setInsecure();

  // ✅ Clear old messages on startup
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numNewMessages) {
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

  bot.sendMessage(CHAT_ID, "🚀 Bot is Online! Send /start", "");
  Serial.println("Bot ready!");
}

void loop() {
  if (millis() - lastTimeBotRan > botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages) {
      handleMessages(numNewMessages);
    }
    lastTimeBotRan = millis();
  }
}