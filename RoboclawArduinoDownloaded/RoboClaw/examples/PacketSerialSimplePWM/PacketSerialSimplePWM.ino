 //See BareMinimum example for a list of library functions

//Includes required to use Roboclaw library
#include <SoftwareSerial.h>
#include "RoboClaw.h"

//See limitations of Arduino SoftwareSerial
SoftwareSerial serial(28, 29);	
RoboClaw roboclaw(&serial,10000);

#define address 0x80

int power = 104;

void setup() {
  //Open roboclaw serial ports
  roboclaw.begin(38400);
}

void loop() {
  roboclaw.ForwardBackwardM1(address, 104); //start Motor1 forward at half speed
  roboclaw.ForwardBackwardM2(address, 64); 
  delay(2000);

  roboclaw.ForwardBackwardM1(address, 64); //start Motor1 forward at half speed
  roboclaw.ForwardBackwardM2(address, 104); 
  delay(2000);

 }
