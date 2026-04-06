#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IR_PIN 2

LiquidCrystal_I2C lcd(0x27, 16, 2);

int count = 0;
bool lastState = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(IR_PIN, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Object Counter ");
  lcd.setCursor(0, 1);
  lcd.print("  Starting...   ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Count:");
  lcd.setCursor(0, 1);
  lcd.print("Place object...");
}

void loop() {
  bool currentState = digitalRead(IR_PIN);

  // Count only when object passes (HIGH to LOW transition)
  if (lastState == HIGH && currentState == LOW) {
    count++;
    Serial.print("Count: ");
    Serial.println(count);

    lcd.setCursor(0, 0);
    lcd.print("Count: ");
    lcd.print(count);
    lcd.print("        ");

    lcd.setCursor(0, 1);
    lcd.print("Object detected!");
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print("Place object... ");
  }

  lastState = currentState;
  delay(50);
}