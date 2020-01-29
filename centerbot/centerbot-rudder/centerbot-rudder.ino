#include <Servo.h>

#define RUDDER_PIN 6

Servo rudder;

void setup() {
  // put your setup code here, to run once:

  rudder.attach(RUDDER_PIN);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(2000);

  rudder.write(100);

  delay(2000);

  rudder.write(0);

}
