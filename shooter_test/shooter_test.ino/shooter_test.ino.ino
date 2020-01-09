#include "PS4BT.h"
#include "usbhub.h"
#include <RoboClaw.h>
#include "SPI.h"
#include <Servo.h>


SoftwareSerial serial(8,7); 
RoboClaw roboclaw(&serial,10000);

#define address 0x80

int shooter_power = 10;

bool ledON;
bool shooter_mode = false;
bool lifter_up = false;


Servo myservo;

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

PS4BT PS4(&Btd);

void setup(){

  myservo.attach(6);

  Serial.begin(115200);
  Serial.print("Main startup... ");
   
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif

  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));
 
  //Serial for RoboClaw
    roboclaw.begin(38400); 
    delay(2000);

  //Set RoboClaw Limits
  roboclaw.SetMainVoltages(address, 130, 190); //(address, minV, maxV) based on 0.1V, so 130 -> 13V
  
}

void loop(){
  Usb.Task();
  
  digitalWrite(4, HIGH);
    
  if (PS4.connected())
  {

    PS4.setLed(Green);
    PS4.setRumbleOff();
    
    if (PS4.getButtonClick(TRIANGLE)){
      if(!shooter_mode){
      //Shooter mode
        roboclaw.ForwardM1(address, shooter_power);
        roboclaw.BackwardM2(address, shooter_power);
        shooter_mode = true;
      }

      else {
        roboclaw.ForwardBackwardM1(address, 64);
        roboclaw.ForwardBackwardM2(address, 64);
        shooter_mode = false;
      }
    }

    if(PS4.getButtonClick(UP)){
      shooter_power += 1;
      shooter_power = min(shooter_power, 127);
      PS4.setRumbleOn(RumbleHigh);
      Serial.println(shooter_power);
      delay(10);
    }

    if(PS4.getButtonClick(DOWN)){
      shooter_power -= 1; 
      shooter_power = max(shooter_power, 1);
      PS4.setRumbleOn(RumbleLow);
      Serial.println(shooter_power);
      delay(10);
    }

    if(PS4.getButtonClick(CIRCLE)){
      if(!lifter_up){
        myservo.write(180);
        lifter_up = true;
      }
      
      else{
        myservo.write(-180);
        lifter_up = false;
      }
    }
  }
}

