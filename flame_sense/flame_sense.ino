#define BLYNK_TEMPLATE_ID      "Blynk_template_id"
#define BLYNK_TEMPLATE_NAME    "Template_name"
#define BLYNK_AUTH_TOKEN       "Blynk_token"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define FLAME_PIN 26
#define BUZZER    27
#define LED       25

char ssid[] = "Your_wifi";
char pass[] = "password";

bool alreadyNotified = false;

void setup() {
  Serial.begin(9600);
  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Fire Alarm System Ready!");
}

void loop() {
  Blynk.run();

  int flame = digitalRead(FLAME_PIN);

  if (flame == HIGH) {
    // Fire detected!
    digitalWrite(LED, HIGH);
    tone(BUZZER, 2000);

    // Send Blynk notification once
    if (!alreadyNotified) {
      Blynk.logEvent("fire_alarm", "🔥 FIRE DETECTED! Check immediately!");
      alreadyNotified = true;
    }

    Serial.println("FIRE DETECTED!");

  } else {
    // All clear
    digitalWrite(LED, LOW);
    noTone(BUZZER);
    alreadyNotified = false;   // reset so next fire triggers again
    Serial.println("All clear.");
  }

  delay(500);
}


