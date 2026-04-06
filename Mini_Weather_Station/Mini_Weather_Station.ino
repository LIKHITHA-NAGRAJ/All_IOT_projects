#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN    2
#define DHTTYPE   DHT11
#define LDR_PIN   3
#define BUZZER    4
#define LED       13

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lastUpdate = 0;
int screen = 0; // rotate between screens

void setup() {
  Serial.begin(9600);
  pinMode(LDR_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Mini Weather  ");
  lcd.setCursor(0, 1);
  lcd.print("   Station!    ");
  delay(3000);
  lcd.clear();

  Serial.println("Mini Weather Station Ready!");
}

String getWeatherStatus(float temp, float hum) {
  if (temp > 35) return "HOT!";
  else if (temp < 18) return "COLD!";
  else if (hum > 80) return "HUMID";
  else if (hum < 30) return "DRY";
  else return "NORMAL";
}

String getComfort(float temp, float hum) {
  float heatIndex = temp + 0.33 * hum - 4;
  if (heatIndex > 35) return "UNCOMFORTABLE";
  else if (heatIndex < 18) return "TOO COLD!";
  else return "COMFORTABLE!";
}

void handleAlerts(float temp, float hum) {
  if (temp > 38) {
    // Extreme heat alert
    digitalWrite(LED, HIGH);
    tone(BUZZER, 2000, 500);
    Serial.println("EXTREME HEAT ALERT!");
  } else if (temp < 15) {
    // Extreme cold alert
    digitalWrite(LED, HIGH);
    tone(BUZZER, 500, 500);
    Serial.println("EXTREME COLD ALERT!");
  } else {
    digitalWrite(LED, LOW);
    noTone(BUZZER);
  }
}

void loop() {
  unsigned long now = millis();

  // Update every 3 seconds
  if (now - lastUpdate > 3000) {
    lastUpdate = now;

    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();
    int light  = digitalRead(LDR_PIN);

    if (isnan(temp) || isnan(hum)) {
      Serial.println("DHT11 error!");
      return;
    }

    // Auto LCD backlight
    if (light == LOW) {
      lcd.backlight();    // day = backlight ON
    } else {
      lcd.noBacklight();  // night = backlight OFF
    }

    handleAlerts(temp, hum);

    // Rotate screens every 3 seconds
    lcd.clear();
    if (screen == 0) {
      // Screen 1 — Temp + Humidity
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print(temp, 1);
      lcd.print("C H:");
      lcd.print(hum, 0);
      lcd.print("%");
      lcd.setCursor(0, 1);
      lcd.print("Status:");
      lcd.print(getWeatherStatus(temp, hum));

    } else if (screen == 1) {
      // Screen 2 — Comfort level
      lcd.setCursor(0, 0);
      lcd.print("Feels:");
      float heatIndex = temp + 0.33 * hum - 4;
      lcd.print(heatIndex, 1);
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print(getComfort(temp, hum));

    } else if (screen == 2) {
      // Screen 3 — Light + advice
      lcd.setCursor(0, 0);
      lcd.print("Light:");
      lcd.print(light == LOW ? "DAY" : "NIGHT");
      lcd.setCursor(0, 1);
      if (temp > 33) {
        lcd.print("Drink water!");
      } else if (hum > 75) {
        lcd.print("Feels stuffy!");
      } else {
        lcd.print("Weather good!");
      }
    }

    screen++;
    if (screen > 2) screen = 0;

    // Serial Monitor log
    Serial.print("Temp: "); Serial.print(temp);
    Serial.print("C  Hum: "); Serial.print(hum);
    Serial.print("%  Light: "); Serial.println(light == LOW ? "DAY" : "NIGHT");
  }
}
