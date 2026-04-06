#include <Servo.h>

Servo servo;
int trigPin = 12;
int echoPin = 11;
long duration;
int distance;

void setup() {
  Serial.begin(9600);
  servo.attach(13);
  servo.write(180);          // start CLOSED
  delay(1000);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance <= 25 && distance > 0) {
    Serial.println("Vehicle — Opening!");
    servo.write(90);          // rod UP
    delay(3000);
    servo.write(0);        // rod DOWN
    delay(1000);
  } else {
    servo.write(0);        // stay closed
  }
}