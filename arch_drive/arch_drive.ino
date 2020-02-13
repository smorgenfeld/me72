#include "PS4BT.h"
#include "usbhub.h"
#include "RoboClaw.h"
#include "SPI.h"
#include <Servo.h>


SoftwareSerial serial(28,29); 
RoboClaw roboclaw(&serial,10000);

#define address 0x80

bool ledON;
bool motor1ON = false;
bool motor2ON = false;

bool shooter_on = false;

bool grab_attached = false;

unsigned long looptime;
unsigned long newtime;
unsigned long oldtime = 0;

USB Usb;
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
PS4BT PS4(&Btd);

void setup(){
  rc_setup();
  PS4_setup();
  ball_setup();
}

void loop(){
  Usb.Task();

  /*newtime = millis();
  looptime = newtime - oldtime;
  oldtime = newtime;

  Serial.print("Loop time: ");
  Serial.println(looptime);*/
    
  if (PS4.connected())
  {

//    PS4.setLed(Red);
    drive_screws();

    Usb.Task();
    if(PS4.getButtonClick(TRIANGLE)){
      if(!shooter_on){
        PS4.setRumbleOn(RumbleLow);
        shoot_ball();
      }
      else{
        PS4.setRumbleOff();
        stop();
      }

      shooter_on = !shooter_on;
    }

   if (PS4.getButtonClick(CROSS)){
      grab_attached =  !grab_attached;
   }
   
   servocontrol(grab_attached);
  }
}

