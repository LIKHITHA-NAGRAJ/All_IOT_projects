#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ThingSpeak.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClient client;

const char* ssid       = "Your_wifi";
const char* password   = "password";
unsigned long channelID      = channel_id;
const char* writeAPIKey      = "write_api_key";//thingspeak

#define PULSE_PIN 34
#define LED_PIN   15   

int bpm = 0;
unsigned long lastBeat   = 0;
unsigned long lastUpload = 0;
bool beatDetected = false;

int bpmHistory[5] = {0};
int bpmIndex = 0;

// ✅ auto threshold tuning variables
int signalMin = 4095;
int signalMax = 0;
int threshold = 2100;
unsigned long calibStart = 0;
bool calibrated = false;

int getAverageBPM() {
  int sum = 0;
  for (int i = 0; i < 5; i++) sum += bpmHistory[i];
  return sum / 5;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  // ✅ WiFi connect with timeout
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi ");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...  ");

  WiFi.begin(ssid, password);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    ThingSpeak.begin(client);
    Serial.println("\nWiFi Connected!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected! ");
    delay(1000);
  } else {
    Serial.println("\nWiFi FAILED - running offline");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed!    ");
    lcd.setCursor(0, 1);
    lcd.print("Offline mode    ");
    delay(1000);
  }

  // ✅ calibration prompt
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...  ");
  lcd.setCursor(0, 1);
  lcd.print("Place finger now");
  calibStart = millis();
}

void loop() {
  int signal = analogRead(PULSE_PIN);
  unsigned long now = millis();

  Serial.println(signal);  // open Serial Plotter to watch wave

  // ✅ auto calibrate for first 5 seconds
  if (!calibrated) {
    if (signal < signalMin) signalMin = signal;
    if (signal > signalMax) signalMax = signal;

    if (now - calibStart > 5000) {
      threshold = (signalMin + signalMax) / 2;
      calibrated = true;

      Serial.print("Calibrated! Min:");
      Serial.print(signalMin);
      Serial.print(" Max:");
      Serial.print(signalMax);
      Serial.print(" Threshold:");
      Serial.println(threshold);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ready!          ");
      lcd.setCursor(0, 1);
      lcd.print("Threshold:");
      lcd.print(threshold);
      delay(1500);
      lcd.clear();
    }
    delay(10);
    return;  // skip beat detection during calibration
  }

  // beat detection
  if (signal > threshold && !beatDetected) {
    beatDetected = true;
    digitalWrite(LED_PIN, HIGH);  // ✅ LED on beat peak

    if (lastBeat > 0) {
      unsigned long interval = now - lastBeat;
      int rawBPM = 60000 / interval;

      if (rawBPM > 40 && rawBPM < 200) {
        bpmHistory[bpmIndex] = rawBPM;
        bpmIndex = (bpmIndex + 1) % 5;
        bpm = getAverageBPM();

        // ✅ no lcd.clear() — overwrite with spaces instead
        lcd.setCursor(0, 0);
        lcd.print("Heart Rate:     ");
        lcd.setCursor(0, 1);
        lcd.print("BPM: ");
        lcd.print(bpm);

        String status;
        if (bpm < 60)        status = " LOW    ";
        else if (bpm <= 100) status = " NORMAL ";
        else                 status = " HIGH   ";
        lcd.print(status);

        Serial.print("BPM: ");
        Serial.print(bpm);
        Serial.println(status);
      }
    }
    lastBeat = now;
  }

  if (signal < threshold - 100) {
    beatDetected = false;
    digitalWrite(LED_PIN, LOW);  // ✅ LED off when signal drops
  }

  // ThingSpeak upload every 20 seconds
  if (now - lastUpload > 20000 && bpm > 40) {
    if (WiFi.status() == WL_CONNECTED) {
      ThingSpeak.setField(1, bpm);
      int result = ThingSpeak.writeFields(channelID, writeAPIKey);

      if (result == 200) {
        Serial.println("ThingSpeak OK!");
        lcd.setCursor(0, 0);
        lcd.print("Uploaded!       ");
        delay(800);
        lcd.setCursor(0, 0);
        lcd.print("Heart Rate:     ");
      } else {
        Serial.print("Upload failed: ");
        Serial.println(result);
      }
    }
    lastUpload = now;
  }

  delay(10);
}