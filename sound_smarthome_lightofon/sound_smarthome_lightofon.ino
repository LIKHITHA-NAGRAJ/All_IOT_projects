#define SOUND_PIN 2
#define LED 13

bool ledState = false;
int clapCount = 0;
unsigned long lastClapTime = 0;
unsigned long clapWindow = 400; // time window to detect double clap (ms)

void setup() {
  Serial.begin(9600);
  pinMode(SOUND_PIN, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

void loop() {
  bool current = digitalRead(SOUND_PIN);
  unsigned long now = millis();

  // Clap detected
  if (current == LOW) {
    if (now - lastClapTime > 100) { // debounce
      clapCount++;
      lastClapTime = now;
      Serial.print("Clap count: ");
      Serial.println(clapCount);
      delay(100);
    }
  }

  // After clap window expires — decide action
  if (clapCount > 0 && (now - lastClapTime > clapWindow)) {
    if (clapCount == 1) {
      // Single clap = OFF
      ledState = false;
      digitalWrite(LED, LOW);
      Serial.println("Single clap → Light OFF 💡");
    } 
    else if (clapCount >= 2) {
      // Double clap = ON
      ledState = true;
      digitalWrite(LED, HIGH);
      Serial.println("Double clap → Light ON 💡");
    }
    clapCount = 0; // reset
  }
}