#include "DHT.h"
DHT dht(3,DHT11);
void setup() {
Serial.begin(9600);
dht.begin();
}

void loop() {
  float h=dht.readHumidity();
  float t=dht.readTemperature();
  float f=dht.readTemperature(true);
  String output="Temperature "+String(t) +"C or "+String(f)+"F and Humidity"+String(h)+"%";
Serial.println(output);
delay(2000);
}
