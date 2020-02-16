#include "PS4BT.h"
#include "usbhub.h"
#include "RoboClaw.h"
#include "SPI.h"
#include <Servo.h>


//SoftwareSerial serial(29,28); 
RoboClaw roboclaw(&Serial1,10000);

#define address 0x80

bool ledON;
bool motor1ON = false;
bool motor2ON = false;

bool shooter_on = false;
bool belt_on = false;

bool grab_attached = false;
bool cubicmap = false;

uint16_t batt_vol;

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
   
  if (PS4.connected())
  {
    Usb.Task();

    if (PS4.getButtonClick(L3)){
       cubicmap = true; 
    }

    if (PS4.getButtonClick(R3)){
      cubicmap = false;
    }
    
    if(PS4.getButtonClick(TRIANGLE)){
      //Pressing Triangle turns on the flywheels, pressing it again turns them off.
      shooter_on = !shooter_on;
    }

   if (PS4.getButtonClick(CIRCLE)){
    //Pressing Circle turns the belt on, pressing it again turns the belt off. 
    if (belt_on){
      belt_stop();
    }
    else{
      belt_run();
    }

    belt_on = !belt_on;
   }
    
   if (PS4.getButtonClick(SQUARE)){
      //Pressing Square checks the battery's voltage.
       batt_vol = roboclaw.ReadMainBatteryVoltage(address);
       Serial.print("LiPo Voltage: ");
       Serial.println(batt_vol);
       if (batt_vol > 150){
        //Voltage is fine, no charge needed. 
        PS4.setLed(Green);
       }
       
       else if (batt_vol > 130){
        //Voltage is low but not critically low.
        PS4.setLed(Yellow);
        PS4.setLedFlash(100, 100);
       }
       
       else{
        //Voltage is critically low and the system needs to be shut off.
        PS4.setLed(Red);
//        safe_mode();
       }
   }

   if (PS4.getButtonClick(CROSS)){
      //Pressing the X button changes between the grabber and scooper for Dpad Control. 
      grab_attached = !grab_attached;
   }
   servodpad(grab_attached);

   //ps4_operate();
   drive_screws(cubicmap);


    if(shooter_on){
      PS4.setRumbleOn(RumbleLow);
      shoot_ball();
    }
    else{
      PS4.setRumbleOff();
      shooter_stop();
    }
  
  
  }
}

