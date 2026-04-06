#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TRACK_PIN  8
#define RELAY_PIN  3

LiquidCrystal_I2C lcd(0x27, 16, 2);

int objectCount = 0;
bool lastState = HIGH;
bool beltRunning = true;

void setup() {
  Serial.begin(9600);
  pinMode(TRACK_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // ✅ belt RUNNING at start

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Conveyor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Belt: RUNNING");
  lcd.setCursor(0, 1);
  lcd.print("Count: 0");

  Serial.println("Smart Conveyor Ready!");
}

void loop() {
  bool current = digitalRead(TRACK_PIN);

  if (lastState == HIGH && current == LOW) {
    objectCount++;
    Serial.print("Object detected! Count: ");
    Serial.println(objectCount);

    // Stop belt ✅
    digitalWrite(RELAY_PIN, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Belt: STOPPED");
    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.print(objectCount);

    delay(2000);

    // Restart belt ✅
    digitalWrite(RELAY_PIN, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Belt: RUNNING");
    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.print(objectCount);

    Serial.println("Belt restarted!");
  }

  lastState = current;
  delay(50);
}