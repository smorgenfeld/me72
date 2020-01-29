/*
  Example sketch for the PS4 Bluetooth library - developed by Kristian Lauszus
  For more information visit my blog: http://blog.tkjelectronics.dk/ or
  send me an e-mail:  kristianl@tkjelectronics.com
*/

// wireless libraries
#include <PS4BT.h>
#include <Usb.h>
#include <usbhub.h>

// servo motor library
#include <Servo.h>

// roboclaw libraries
#include <RoboClaw.h>

// define actuator pins
#define RUDDER_PIN 3
#define FAN_PIN 5

// define the rudder min, max, and center position (on 0 to 180 scale)
#define RUDDER_MIN 0
#define RUDDER_MAX 100
#define RUDDER_CENTER 50

// servo objects for our motors
Servo rudder;
Servo fan;

// tank mode automatically activated
boolean tank_mode = false;

// Roboclaw serial communication
SoftwareSerial serial(7, 6);
RoboClaw roboclaw(&serial, 10000);

#define address 0x80

int Upper_thres = 137;
int Lower_thres = 117;

int left_power;
int right_power;
int leftjoystick_reading;
int rightjoystick_reading;
int ljX_reading;

bool ledON;
bool motor1ON = false;
bool motor2ON = false;

bool shooter_on = false;

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
PS4BT PS4(&Btd, PAIR);
//*
//*

// After that you can simply create the instance like so and then press the PS button on the device
//PS4BT PS4(&Btd);


// values from the remote that we want to capture/store
int L2_val = 0;
int LY_HAT = 0;

// speed values for each motor
int rudder_val = 0;
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

  //Serial for RoboClaw
  roboclaw.begin(38400);


  delay(2000);

  //Set RoboClaw Limits
  roboclaw.SetMainVoltages(address, 130, 190); //(address, minV, maxV) based on 0.1V, so 130 -> 13V

  // attach motors to their pins
  rudder.attach(RUDDER_PIN);
  fan.attach(FAN_PIN, 1000, 2000);

  // set tank LED color on the PS4 controller
  //PS4.setLed(Green);

}
void loop() {
  // idk what this does
  Usb.Task();

  // reset the speed and servo values in case connection issues occur :(
  //rudder_val = 0;
  //fan_val = 0;

  if (PS4.connected()) {

    //PS4.setRumbleOn(RumbleHigh);

    // get the readings from the PS4 controller's left and right knobs
    leftjoystick_reading = PS4.getAnalogHat(LeftHatY);
    rightjoystick_reading = PS4.getAnalogHat(RightHatY);
    ljX_reading = PS4.getAnalogHat(LeftHatX);

    // check for safety kill switch
    if (PS4.getButtonClick(CIRCLE)) { 

      // we're Thanos' color now
      PS4.setLed(Purple);
      
      // THANOS SNAPS...
      endgame();
      
    }
    
    if (PS4.getButtonClick(TRIANGLE)) {
      Serial.println(F("\r\nTriangle"));

      if (tank_mode) {

        // if we're leaving tank mode, adjust the tank motors to no speed
        roboclaw.ForwardBackwardM1(address, 64);
        roboclaw.ForwardBackwardM2(address, 64);

        // change PS4 LED to green (GREEN = FAN MODE)
        PS4.setLed(Green);

      }
      else {
        // change PS4 LED to blue (BLUE = TANK MODE)
        PS4.setLed(Blue);

        // if we're leaving fan mode, center the rudder and turn off fan
        rudder.write(RUDDER_CENTER);
        fan.write(0);

      }

      // adjust the robot's state
      tank_mode = !tank_mode;
    }

    // TANK_MODE: check the right joystick setting (make sure it's being set)
    if ((leftjoystick_reading < Lower_thres || leftjoystick_reading > Upper_thres) && tank_mode) {

      // get the correct power setting for the left and right motors
      left_power = map(leftjoystick_reading, 0, 255, 128, 0); //Maps analog output of joystick to ForwardsBackwards

      // set power of left tank motor
      roboclaw.ForwardBackwardM1(address, left_power);
      Serial.print("Left tank Power: ");
      Serial.println(left_power);
    }

    // if it's not getting the signal, set left tank power to zero
    else {
      roboclaw.ForwardBackwardM1(address, 64);
    }

    // TANK_MODE: check the right joystick setting (make sure it's being set)
    if ((rightjoystick_reading < Lower_thres || rightjoystick_reading > Upper_thres) && tank_mode) {
      // get the correct power setting for the left and right motors
      right_power = map(rightjoystick_reading, 0, 255, 128, 0); //Maps analog output to ForwardsBackwards levels

      // set power of right tank motor
      roboclaw.ForwardBackwardM2(address, right_power);
      Serial.print("Right tank Power: ");
      Serial.println(right_power);
    }

    // if it's not getting the signal, set right tank power to zero
    else {
      roboclaw.ForwardBackwardM2(address, 64);
    }

    // FAN_MODE: check the right joystick setting (make sure it's being set)
    if ((rightjoystick_reading < Lower_thres || rightjoystick_reading > Upper_thres) && !tank_mode) {

      // map the fan values from 0 to 180
      fan_val = 180 - map(rightjoystick_reading, 0, 100, 0, 180);

      // check to make sure fan is non-negative
      if (fan_val < 0) {

        fan_val = 0;

      }

      // write speed to fan
      fan.write(fan_val);
      Serial.print("Right fan power: ");
      Serial.println(fan_val);
    }

    // FAN_MODE: check the right joystick setting (make sure it's being set), for rudder
    if ((ljX_reading < Lower_thres || ljX_reading > Upper_thres) && !tank_mode) {

      // map the rudder values from min to max values
      rudder_val = map(ljX_reading, 0, 255, RUDDER_MIN, RUDDER_MAX);

      rudder.write(rudder_val);
      Serial.print("Rudder pos: ");
      Serial.println(rudder_val);
    }

    // if it's not getting the signal, set rudder to center
    //else {
    //  rudder.write(RUDDER_CENTER);
    //  Serial.print("Rudder pos: ");
    //  Serial.println(RUDDER_CENTER);
    //}

  }

}


void endgame() {
  
  // stay in this for some damn reason...safety? what?
  while(true) {
    
    Serial.println("We're in the Endgame now");   
    
    
  }  
  
  
}
