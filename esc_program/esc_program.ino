#include <Servo.h>

int potpin = A15;

int fanpin = 5;

Servo fan;

void setup() {
  // put your setup code here, to run once:
  pinMode(potpin, INPUT);

  fan.attach(fanpin, 1000,2000);
  
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:

  int out = analogRead(potpin);

  out = map(out, 0, 1023, 0, 180);

  Serial.println(out);
  fan.write(out);

}
