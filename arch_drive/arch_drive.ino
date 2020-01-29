#include "PS4BT.h"
#include "usbhub.h"
#include "RoboClaw.h"
#include "SPI.h"


SoftwareSerial serial(28,29); 
RoboClaw roboclaw(&serial,10000);

#define address 0x80

bool ledON;
bool motor1ON = false;
bool motor2ON = false;

bool shooter_on = false;

USB Usb;
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
PS4BT PS4(&Btd);

void setup(){
  rc_setup();
  PS4_setup();
  shooter_setup();
}

void loop(){
  Usb.Task();
    
  if (PS4.connected())
  {

    PS4.setLed(Red);
    drive_screws();
    if(PS4.getButtonClick(TRIANGLE)){
      if(!shooter_on){
        PS4.setRumbleOn(RumbleLow);
        turn_R(16, 16);
      }
      else{
        PS4.setRumbleOff();
        stop();
      }

      shooter_on = !shooter_on;
    }
  }
}

