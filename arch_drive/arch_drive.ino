#include "PS4BT.h"
#include "usbhub.h"
#include "RoboClaw.h"
#include "SPI.h"


SoftwareSerial serial(10,11); 
RoboClaw roboclaw(&serial,10000);

#define address 0x80

//bool printAngle, printTouch;
//uint8_t oldL2Value, oldR2Value;

int Upper_thres = 137;
int Lower_thres = 117;

int left_power;
int right_power;
int leftjoystick_reading;
int rightjoystick_reading;

int shooter_power = 20;

bool ledON;
bool motor1ON = false;
bool motor2ON = false;

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

/* You can create the instance of the PS4BT class in two ways */
// This will start an inquiry and then pair with the PS4 controller - you only have to do this once
// You will need to hold down the PS and Share button at the same time, the PS4 controller will then start to blink rapidly indicating that it is in pairing mode
//PS4BT PS4(&Btd, PAIR);

// After that you can simply create the instance like so and then press the PS button on the device
PS4BT PS4(&Btd);

void setup(){

  Serial.begin(115200);
  Serial.print("Main startup...")
   
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
  roboclaw.SetMainVoltages(address, 140, 190); //(address, minV, maxV) based on 0.1V, so 140 -> 14V
  
}

void loop(){
  Usb.Task();
  
  roboclaw.ForwardBackwardM1(address, 104); //start Motor1 forward at half speed
  roboclaw.ForwardBackwardM2(address, 64); 
  delay(2000);

  roboclaw.ForwardBackwardM1(address, 64); //start Motor1 forward at half speed
  roboclaw.ForwardBackwardM2(address, 104); 
  delay(2000);

  digitalWrite(6, HIGH);

  /*if (PS4.connected())
  {

    digitalWrite(7, HIGH);
    
    leftjoystick_reading = PS4.getAnalogHat(LeftHatY);
    rightjoystick_reading = PS4.getAnalogHat(RightHatY);

    left_power = map(leftjoystick_reading, 0, 255, 84, 44); //Maps analog output of joystick to ForwardsBackwards
    right_power = map(rightjoystick_reading, 0, 255, 84, 44); //Maps analog output to ForwardsBackwards levels

<<<<<<< HEAD
    //if(leftjoystick_reading < Lower_thres || leftjoystick_reading > Upper_thres)

    if (PS4.getButtonClick(SQUARE))
    {
      if (motor1ON){
      roboclaw.ForwardBackwardM1(address, 64);
      PS4.setRumbleOff();
      motor1ON = false;
      }
    
      else{
      roboclaw.ForwardBackwardM1(address, 75);
      PS4.setRumbleOn(RumbleLow);
      motor1ON = true;
      }
=======
    if(leftjoystick_reading < Lower_thres || leftjoystick_reading > Upper_thres){
      roboclaw.ForwardBackwardM2(address, left_power);
      Serial.print("Left Power: ");
      Serial.println(left_power);
>>>>>>> parent of 5fc97d3... Reverted arch_drive.ino
    }

    //if (rightjoystick_reading < Lower_thres || rightjoystick_reading > Upper_thres)

<<<<<<< HEAD
    if (PS4.getButtonClick(CIRCLE))
    {
      if (motor2ON){
      roboclaw.ForwardBackwardM2(address, 64);
      PS4.setLed(Yellow);
      motor2ON = false;
      }

      else{
        roboclaw.ForwardBackwardM2(address, 75);
        PS4.setLed(Blue);
        motor2ON = true;
=======
    if (rightjoystick_reading > Lower_thres && rightjoystick_reading < Upper_thres && leftjoystick_reading > Lower_thres && leftjoystick_reading < Upper_thres){
      roboclaw.ForwardBackwardM1(address, 64); //Stop the motor
      
      if (PS4.getButtonClick(TRIANGLE)){
      //Shooter mode
        roboclaw.ForwardM1(address, shooter_power);
        roboclaw.ForwardM2(address, shooter_power);
      }

      else{
        roboclaw.ForwardBackwardM1(address, 64);
        roboclaw.ForwardBackwardM2(address, 64);
>>>>>>> parent of 5fc97d3... Reverted arch_drive.ino
      }
    }
  }*/
}

