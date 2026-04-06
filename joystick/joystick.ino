#define JOY_X   A0
#define JOY_Y   A1
#define UP      2
#define DOWN    3
#define LEFT    4
#define RIGHT   5

void setup() {
  Serial.begin(9600);
  pinMode(UP, OUTPUT);
  pinMode(DOWN, OUTPUT);
  pinMode(LEFT, OUTPUT);
  pinMode(RIGHT, OUTPUT);
}

void allOff() {
  digitalWrite(UP, LOW);
  digitalWrite(DOWN, LOW);
  digitalWrite(LEFT, LOW);
  digitalWrite(RIGHT, LOW);
}

void loop() {
  int x = analogRead(JOY_X);  // 0-1023
  int y = analogRead(JOY_Y);  // 0-1023

  Serial.print("X: "); Serial.print(x);
  Serial.print(" Y: "); Serial.println(y);

  allOff();  // turn all off first

  if (x < 300) {
    digitalWrite(LEFT, HIGH);
    Serial.println("LEFT");
  }
  else if (x > 700) {
    digitalWrite(RIGHT, HIGH);
    Serial.println("RIGHT");
  }
  else if (y < 300) {
    digitalWrite(UP, HIGH);
    Serial.println("UP");
  }
  else if (y > 700) {
    digitalWrite(DOWN, HIGH);
    Serial.println("DOWN");
  }
  // center = all off

  delay(100);
}