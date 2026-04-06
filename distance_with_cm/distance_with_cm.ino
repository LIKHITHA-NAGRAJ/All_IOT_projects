#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define TRIG_PIN 9
#define ECHO_PIN 10
#define GREEN_LED 2
#define RED_LED 3
#define BUZZER 4
#define SERVO_PIN 6

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

int pos = 0;
int direction = 1;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Sonar System ");
  lcd.setCursor(0, 1);
  lcd.print("  Starting...  ");
  delay(2000);
  lcd.clear();
}

long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

void loop() {
  long distance = readDistance();
  Serial.print("Distance: ");
  Serial.println(distance);   // watch this in Serial Monitor!

  // ✅ FIXED CONDITION
  if (distance < 20 && distance > 0 && distance != 999) {
    // Object detected!
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 1000);

    lcd.setCursor(0, 0);
    lcd.print("  ** WARNING ** ");
    lcd.setCursor(0, 1);
    lcd.print("Obj: ");
    lcd.print(distance);
    lcd.print(" cm        ");

    myServo.write(pos);
    delay(200);

  } else {
    // Clear zone
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);

    lcd.setCursor(0, 0);
    lcd.print(" Area is Clear  ");
    lcd.setCursor(0, 1);
    if (distance == 999) {
      lcd.print("Dist: --  cm    ");  // shows -- when no echo
    } else {
      lcd.print("Dist: ");
      lcd.print(distance);
      lcd.print(" cm        ");
    }

    myServo.write(pos);
    delay(10);
    pos += direction;
    if (pos >= 180 || pos <= 0) direction = -direction;
  }
}