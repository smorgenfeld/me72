#include <RoboClaw.h>
#include "PS4BT.h"
#include "usbhub.h"

#define address 0x80
#define M1_PIN 10
#define M2_PIN 11

// Satisfy the IDE, which needs to see the include statement in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

SoftwareSerial serial(M1_PIN, M2_PIN);
RoboClaw roboclaw(&serial, 10000);

void setup(){
	Serial.begin(115200);
	Serial.println("Main startup...")

	PS4_setup();
	drive_setup();

}

void loop(){
	PS4_operate();
	drive_operate();
}
