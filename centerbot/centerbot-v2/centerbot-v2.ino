/*
 Example sketch for the PS4 Bluetooth library - developed by Kristian Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */

#include <PS4BT.h>
#include <Usb.h>
#include <usbhub.h>
#include <Servo.h>

// define actuator pins
#define RUDDER_PIN 3
#define FAN_PIN 5

// servo objects for our motors
Servo rudder;
Servo fan;

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

/* You can create the instance of the PS4BT class in two ways */
// This will start an inquiry and then pair with the PS4 controller - you only have to do this once
// You will need to hold down the PS and Share button at the same time, the PS4 controller will then start to blink rapidly indicating that it is in pairing mode

//*
//* DO NOT UNCOMMENT THE NEXT LINE, WE NO LONGER WANT TO PAIR
//PS4BT PS4(&Btd, PAIR);
//*
//*

// After that you can simply create the instance like so and then press the PS button on the device
PS4BT PS4(&Btd);


// values from the remote that we want to capture/store
int L2_val = 0;

// speed values for each motor
int servo_val = 0;
int fan_val = 0;


void setup() {

  // Serial communication
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));

  // attach motors to their pins
  rudder.attach(RUDDER_PIN);
  fan.attach(FAN_PIN, 1000,2000);
  
}
void loop() {
  Usb.Task();

  // write speeds to the motors
  //rudder.write(servo_val);
  fan.write(fan_val);
  
  if (PS4.connected()) {
      PS4.setLed(Green);
    
      //PS4.setRumbleOn(RumbleHigh);
     
     if(PS4.getAnalogButton(L2)) {

      // get the L2 values from the controller
      L2_val = PS4.getAnalogButton(L2);

      // map this input from the controller to the motor's input range
      //servo_val = map(L2_val, 0, 255, 0, 100);
      fan_val = map(L2_val, 0, 255, 0, 180);
      
     }
    
  }
}
