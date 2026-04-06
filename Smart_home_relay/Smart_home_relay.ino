#define BLYNK_TEMPLATE_ID   "TMPL3we_zrRi2"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN    "BQapvIigC-wJRJDjhe02wnH2FlBK7-0m"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define RELAY_PIN 26
#define LED_PIN   27

char ssid[] = "Your_Wifi";
char pass[] = "password";

BlynkTimer timer;

BLYNK_CONNECTED() {
  Serial.println("Blynk connected!");  // ✅ confirms connection
  Blynk.syncVirtual(V1);              // ✅ sync button state on startup
}

BLYNK_WRITE(V1) {
  int val = param.asInt();
  Serial.print("V1 received: ");      // ✅ debug
  Serial.println(val);
  if (val == 1) {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Appliance ON!");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
    Serial.println("Appliance OFF!");
  }
}

void checkSchedule() {
  Blynk.virtualWrite(V2, digitalRead(LED_PIN));
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Starting...");

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);

  Serial.println("Connecting to WiFi...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, checkSchedule);
  Serial.println("Smart Home Ready!");
}

void loop() {
  Blynk.run();
  timer.run();
}