#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define DHTPIN D5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long lastUpdate = 0;
int screen = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (true);
  }

  // Startup screen
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(20, 10);
  display.println("OLED Weather");
  display.setCursor(30, 25);
  display.println("Station!");
  display.setCursor(25, 45);
  display.println("by Likhitha");
  display.display();
  delay(3000);

  Serial.println("OLED Weather Station Ready!");
}

String getStatus(float temp, float hum) {
  if (temp > 35) return "HOT!";
  else if (temp < 18) return "COLD!";
  else if (hum > 80) return "HUMID";
  else if (hum < 30) return "DRY";
  else return "NORMAL";
}

void showScreen1(float temp, float hum) {
  display.clearDisplay();

  // Title
  display.setTextSize(1);
  display.setCursor(25, 0);
  display.println("WEATHER STATION");

  // Divider line
  display.drawLine(0, 10, 128, 10, WHITE);

  // Temperature big
  display.setTextSize(2);
  display.setCursor(0, 16);
  display.print(temp, 1);
  display.print(" C");

  // Humidity
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print("Humidity: ");
  display.print(hum, 0);
  display.print("%");

  // Status
  display.setCursor(0, 52);
  display.print("Status: ");
  display.print(getStatus(temp, hum));

  display.display();
}

void showScreen2(float temp, float hum) {
  display.clearDisplay();

  // Title
  display.setTextSize(1);
  display.setCursor(30, 0);
  display.println("COMFORT LEVEL");
  display.drawLine(0, 10, 128, 10, WHITE);

  // Heat index
  float heatIndex = temp + 0.33 * hum - 4;
  display.setTextSize(1);
  display.setCursor(0, 16);
  display.print("Feels like: ");
  display.setTextSize(2);
  display.setCursor(0, 28);
  display.print(heatIndex, 1);
  display.print("C");

  // Comfort
  display.setTextSize(1);
  display.setCursor(0, 52);
  if (heatIndex > 35) display.print("UNCOMFORTABLE!");
  else if (heatIndex < 18) display.print("TOO COLD!");
  else display.print("COMFORTABLE!");

  display.display();
}

void showScreen3(float temp, float hum) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(35, 0);
  display.println("ADVICE");
  display.drawLine(0, 10, 128, 10, WHITE);

  display.setTextSize(1);
  display.setCursor(0, 18);

  if (temp > 35) {
    display.println("Stay hydrated!");
    display.setCursor(0, 32);
    display.println("Use sunscreen!");
    display.setCursor(0, 46);
    display.println("Stay indoors!");
  } else if (hum > 75) {
    display.println("Feels stuffy!");
    display.setCursor(0, 32);
    display.println("Open windows!");
    display.setCursor(0, 46);
    display.println("Use fan!");
  } else if (temp < 18) {
    display.println("Wear a jacket!");
    display.setCursor(0, 32);
    display.println("Stay warm!");
    display.setCursor(0, 46);
    display.println("Hot drinks!");
  } else {
    display.println("Weather is good!");
    display.setCursor(0, 32);
    display.println("Great day to");
    display.setCursor(0, 46);
    display.println("go outside!");
  }

  display.display();
}

void loop() {
  unsigned long now = millis();

  if (now - lastUpdate > 3000) {
    lastUpdate = now;

    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
      Serial.println("DHT11 error!");
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 25);
      display.println("Sensor Error!");
      display.display();
      return;
    }

    Serial.print("Temp: "); Serial.print(temp);
    Serial.print("C  Hum: "); Serial.println(hum);

    if (screen == 0) showScreen1(temp, hum);
    else if (screen == 1) showScreen2(temp, hum);
    else if (screen == 2) showScreen3(temp, hum);

    screen++;
    if (screen > 2) screen = 0;
  }
}