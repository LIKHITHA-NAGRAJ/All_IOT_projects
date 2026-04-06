int potpin=A0;
int potvalue;
void setup() {
pinMode(potpin,INPUT);
Serial.begin(9600);
pinMode(3,OUTPUT);
}

void loop() {
potvalue=analogRead(potpin);
int brightness=map(potvalue,0,1023,0,255);
analogWrite(3,brightness);
Serial.println(potvalue);
delay(300);
}
