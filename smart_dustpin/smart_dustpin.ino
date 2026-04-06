#include <Servo.h>

#define TRIGGER_PIN  9
#define ECHO_PIN     10
#define SERVO_PIN    6
#define MAX_DISTANCE 20

Servo servo;

void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  servo.attach(SERVO_PIN);
  servo.write(180);      // start CLOSED
  delay(500);
}

void loop() {
  long duration, distance;

  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance <= MAX_DISTANCE && distance > 0) {
    Serial.println("Opening lid!");
    servo.write(90);     // ✅ OPEN
    delay(2500);
    servo.write(0);    // ✅ CLOSED
    Serial.println("Closing lid!");
  } else {
    servo.write(0);    // stay CLOSED
  }

  delay(500);
}