#define TOUCH_PIN 2
#define LED 13
#define BUZZER 3
#define RED_LED 4

int tapCount = 0;
unsigned long lastTapTime = 0;
unsigned long firstTapTime = 0;
bool lastState = LOW;

void setup() {
  Serial.begin(9600);
  pinMode(TOUCH_PIN, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, HIGH);
  Serial.println("Touch Lock Ready!");
  Serial.println("Tap 3 times to unlock!");
}

void unlock() {
  digitalWrite(LED, HIGH);
  digitalWrite(RED_LED, LOW);
  tone(BUZZER, 1000, 200);
  delay(250);
  tone(BUZZER, 1500, 200);
  delay(250);
  tone(BUZZER, 2000, 200);
  Serial.println("UNLOCKED!!");
  delay(8000);
  lockSystem();
}

void lockSystem() {
  digitalWrite(LED, LOW);
  digitalWrite(RED_LED, HIGH);
  tapCount = 0;
  noTone(BUZZER);
  Serial.println("Locked!");
}

void wrongPattern() {
  Serial.println("Wrong! Try again!");
  tone(BUZZER, 400, 800);
  delay(900);
  noTone(BUZZER);
  tapCount = 0;
}

void loop() {
  bool current = digitalRead(TOUCH_PIN);
  unsigned long now = millis();

  if (lastState == LOW && current == HIGH) {
    // ✅ only count if 400ms passed since last tap
    if (now - lastTapTime > 400) {
      if (tapCount == 0) firstTapTime = now;
      tapCount++;
      lastTapTime = now;
      Serial.print("Tap ");
      Serial.print(tapCount);
      Serial.println(" detected!");
    }
    delay(500);  // ✅ bigger debounce
  }

  // wait 2 seconds after last tap
  if (tapCount > 0 && (now - lastTapTime > 2000)) {
    Serial.print("Total taps: ");
    Serial.println(tapCount);
    if (tapCount == 3) {
      unlock();
    } else {
      wrongPattern();
    }
    tapCount = 0;
  }

  lastState = current;
}