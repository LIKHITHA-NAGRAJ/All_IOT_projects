#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TILT_PIN  11
#define VIB_PIN   3
#define BUZZER    4
#define RED_LED   6
#define GREEN_LED 8

LiquidCrystal_I2C lcd(0x27, 16, 2);

int tiltCount = 0;
int vibCount  = 0;
unsigned long lastReset = 0;
String alertLevel = "NONE";

void setup() {
  Serial.begin(9600);
  pinMode(TILT_PIN, INPUT);
  pinMode(VIB_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Earthquake Det ");
  lcd.setCursor(0, 1);
  lcd.print("  Monitoring... ");
  delay(2000);
  lcd.clear();

  digitalWrite(GREEN_LED, HIGH);
  Serial.println("Earthquake Detector Ready!");
}

void noAlert() {
  alertLevel = "NONE";
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  noTone(BUZZER);              // ✅ stop tone first
  delay(10);

  lcd.clear();                 // ✅ clear before print
  lcd.setCursor(0, 0);
  lcd.print("Status: SAFE    ");
  lcd.setCursor(0, 1);
  lcd.print("No Activity     ");
}

void lowAlert() {
  alertLevel = "LOW";
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  noTone(BUZZER);              // ✅ stop first
  delay(10);

  lcd.clear();                 // ✅ clear before print
  lcd.setCursor(0, 0);
  lcd.print("** LOW ALERT ** ");
  lcd.setCursor(0, 1);
  lcd.print("Minor Tremor!   ");
  Serial.println("LOW ALERT!");

  tone(BUZZER, 500);           // ✅ tone AFTER lcd
}

void highAlert() {
  alertLevel = "HIGH";
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  noTone(BUZZER);              // ✅ stop first
  delay(10);

  lcd.clear();                 // ✅ clear before print
  lcd.setCursor(0, 0);
  lcd.print("!! EARTHQUAKE !!");
  lcd.setCursor(0, 1);
  lcd.print("EVACUATE NOW!!! ");
  Serial.println("EARTHQUAKE DETECTED!!");

  // ✅ beep AFTER lcd update
  for (int i = 0; i < 3; i++) {
    tone(BUZZER, 2000);
    delay(200);
    noTone(BUZZER);
    delay(100);
  }
}

void loop() {
  int tilt = digitalRead(TILT_PIN);
  int vib  = digitalRead(VIB_PIN);
  unsigned long now = millis();

  if (tilt == LOW) {
    tiltCount++;
    Serial.println("Tilt detected!");
  }
  if (vib == LOW) {
    vibCount++;
    Serial.println("Vibration detected!");
  }

  if (now - lastReset > 2000) {
    Serial.print("Tilt: "); Serial.print(tiltCount);
    Serial.print(" Vib: "); Serial.println(vibCount);

    if (tiltCount >= 3 && vibCount >= 3) {
      highAlert();
      delay(5000);             // high alert stays 5 seconds
    } else if (tiltCount >= 1 || vibCount >= 2) {
      lowAlert();
      delay(2000);             // low alert stays 2 seconds
    } else {
      noAlert();
    }

    tiltCount = 0;
    vibCount  = 0;
    lastReset = now;
  }

  delay(50);
}