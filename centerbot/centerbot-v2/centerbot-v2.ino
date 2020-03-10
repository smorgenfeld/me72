// wireless libraries
#include <PS4BT.h>
#include <Usb.h>
#include <usbhub.h>

// servo motor library
#include <Servo.h>

// roboclaw libraries
#include <RoboClaw.h>

// define actuator pins
#define GATE_PIN 8
#define SCOOP_PIN 4
#define FAN_PIN 5
#define FAN_REVERSE_PIN 6
#define RUDDER_PIN 7

// define the rudder min, max, and center position (on 0 to 180 scale)
#define RUDDER_MIN 30
#define RUDDER_MAX 150
#define RUDDER_CENTER 85

// define the ZERO, MAX Forward, and MAX Backward speed for RS550
#define TANK_MAX_F 127
#define TANK_MAX_B 0
#define TANK_ZERO 64

// define open and close positions for the gate
#define GATE_OPEN 90
#define GATE_CLOSE 180

// define min and max positions for the scooper
#define MIN_SCOOP 0
#define MAX_SCOOP 150
#define MAX_MAX_SCOOP 170
#define BUTTON_BAP 95

// define the min and max speeds for the fan
#define MIN_FAN 0
#define MAX_FAN 100
#define FAN_INC 5

// controller max
#define MAX_CONTROLLER 255

// arcade tank
# define ARCADE true
int leftVal = 0;
int rightVal = 0;
# define RIGHT_OFFSET 1
# define LEFT_OFFSET 1
# define THROTTLE_BIAS 1.5

// servo objects for our motors
Servo rudder;
Servo fan;
Servo fan_reverse;
Servo gate;
Servo scoop;

// tank mode automatically activated
boolean tank_mode = false;

// Roboclaw serial communication using software serial
//SoftwareSerial serial(24, 25);
//RoboClaw roboclaw(&serial, 10000);

// Trying to use Serial port to avoid softwareserial
//
//**** uncomment
RoboClaw roboclaw(&Serial1,10000);
//
//
//

#define address 0x80
 
int Upper_thres = 137;
int Lower_thres = 117;

int left_power;
int right_power;
int leftjoystick_reading;
int rightjoystick_reading;
int ljX_reading;

// boolean for when the fan is moving forward
bool fan_forward = true;

// button bapping mode
bool button_bap_mode = false;

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

//*
//* DO NOT UNCOMMENT THE NEXT LINE, WE NO LONGER WANT TO PAIR ok boomer
//PS4BT PS4(&Btd, PAIR);
//*
//*

// After that you can simply create the instance like so and then press the PS button on the device
PS4BT PS4(&Btd);

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
  //  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    //    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  //  Serial.print(F("\r\nPS4 Bluetooth Library Started"));

  //Serial for RoboClaw
  roboclaw.begin(38400);

  delay(2000);

  //Set RoboClaw Limits
  roboclaw.SetMainVoltages(address, 130, 190); //(address, minV, maxV) based on 0.1V, so 130 -> 13V

  // attach motors to their pins
  rudder.attach(RUDDER_PIN);
  fan.attach(FAN_PIN, 1000, 2000);
  fan_reverse.attach(FAN_REVERSE_PIN, 1000, 2000);
  gate.attach(GATE_PIN);
  scoop.attach(SCOOP_PIN);

}
void loop() {
  // idk what this does
  Usb.Task();

  if (PS4.connected()) {

    // get the readings from the PS4 controller's left and right knobs
    leftjoystick_reading = PS4.getAnalogHat(LeftHatY);
    rightjoystick_reading = PS4.getAnalogHat(RightHatY);
    ljX_reading = PS4.getAnalogHat(LeftHatX);

    // check for safety kill switch
    if (PS4.getButtonClick(CIRCLE)) {

      // we're Thanos' color now
      PS4.setLed(255, 0, 255);

      // stop the tank treads
      roboclaw.ForwardBackwardM1(address, TANK_ZERO);
      roboclaw.ForwardBackwardM2(address, TANK_ZERO);

      // stop the fan
      fan.write(0);

      // THANOS SNAPS...
      endgame();

    }

    if (PS4.getButtonClick(TRIANGLE)) {

      if (tank_mode) {

        // if we're leaving tank mode, adjust the tank motors to no speed
        roboclaw.ForwardBackwardM1(address, TANK_ZERO);
        roboclaw.ForwardBackwardM2(address, TANK_ZERO);

        // reset the fan when switching to fan mode (since fan could be on from slight modification)
        fan.write(0);

        // reset the fan value
        fan_val = 0;

        // change PS4 LED to Blue (BLUE = FAN MODE)
        PS4.setLed(Blue);

      }
      else {

        // change PS4 LED to green (Green = TANK MODE)
        PS4.setLed(Green);

        // if we're leaving fan mode, center the rudder and turn off fan
        rudder.write(RUDDER_CENTER);
        fan.write(0);

        // reset the fan value
        fan_val = 0;

      }

      // adjust the robot's state
      tank_mode = !tank_mode;

    }

    if (tank_mode && !ARCADE) {
      // TANK_MODE: check the right joystick setting (make sure it's being set)
      if ((leftjoystick_reading < Lower_thres || leftjoystick_reading > Upper_thres) && tank_mode) {
  
        // left_power using the cubic function
        left_power = cubic_mapv2(MAX_CONTROLLER - leftjoystick_reading);
  
        // set power of left tank motor
        roboclaw.ForwardBackwardM1(address, left_power);
  
      }
  
      // if it's not getting the signal, set left tank power to zero
      else if (tank_mode) {
  
        roboclaw.ForwardBackwardM1(address, TANK_ZERO);
  
      }
  
      // TANK_MODE: check the right joystick setting (make sure it's being set)
      if ((rightjoystick_reading < Lower_thres || rightjoystick_reading > Upper_thres) && tank_mode) {
  
        // right_power using the cubic function
        right_power = cubic_mapv2(MAX_CONTROLLER - rightjoystick_reading);
  
        // set power of right tank motor
        roboclaw.ForwardBackwardM2(address, right_power);
  
      }
  
      // if it's not getting the signal, set right tank power to zero
      else if (tank_mode) {
  
        roboclaw.ForwardBackwardM2(address, TANK_ZERO);
  
      }
    }
    else if (tank_mode) { // ARCADE drive scheme
      left_power = TANK_ZERO;
      right_power = TANK_ZERO;
      rightVal = MAX_CONTROLLER / 2;
      leftVal = MAX_CONTROLLER / 2;

      // aquire controller input
      if (ljX_reading < Lower_thres || ljX_reading > Upper_thres) {
        leftVal = ljX_reading;
      }
      if (rightjoystick_reading < Lower_thres || rightjoystick_reading > Upper_thres) {
        rightVal = ((rightjoystick_reading - MAX_CONTROLLER / 2) * THROTTLE_BIAS) + MAX_CONTROLLER / 2;
      }
      
      // blend controller inputs; do the arcade drive math (cubic)
      // right_power = cubic_mapv2(MAX_CONTROLLER - (rightjoystick_reading - ljX_reading + MAX_CONTROLLER) / 2) * RIGHT_OFFSET;
      // left_power = cubic_mapv2(MAX_CONTROLLER - (rightjoystick_reading + ljX_reading) / 2) * LEFT_OFFSET;

      // blend controller inputs; do the arcade drive math (linear)
      right_power = map(MAX_CONTROLLER - (rightjoystick_reading - ljX_reading + MAX_CONTROLLER) / 2,0,MAX_CONTROLLER, 0, TANK_MAX_F) * RIGHT_OFFSET;
      left_power = map(MAX_CONTROLLER - (rightjoystick_reading + ljX_reading) / 2,0,MAX_CONTROLLER, 0, TANK_MAX_F) * LEFT_OFFSET;
      
      // limit the power to reasonable ranges (just in case...)
      right_power = max(TANK_MAX_B, min(TANK_MAX_F, right_power));
      left_power = max(TANK_MAX_B, min(TANK_MAX_F, left_power));

      // set motor powers
      roboclaw.ForwardBackwardM2(address, left_power);
      roboclaw.ForwardBackwardM1(address, right_power);
    }

    // fan control when the tank treads are on
    if (PS4.getButtonClick(RIGHT) && tank_mode) {

      // check the new fan value when incrementing
      if(fan_val + FAN_INC < MAX_FAN) {

        fan_val = fan_val + FAN_INC;
        
      } 

      // write the new value
      fan.write(fan_val);
      
    }
    else if (PS4.getButtonClick(LEFT) && tank_mode) {
      
      // check the new fan value when incrementing
      if(fan_val - FAN_INC >= MIN_FAN) {

        fan_val = fan_val - FAN_INC;
        
      }

      // write the new value
      fan.write(fan_val);
      
    }

    // FAN_MODE: check the right joystick setting (make sure it's being set)
    if (rightjoystick_reading > Upper_thres && !tank_mode) {

      // make sure fan is moving forwards
      fan_reverse.write(0);

      // map the fan values from 0 to 180 (in the lower stick area)
      fan_val = map(rightjoystick_reading - Upper_thres, 0, 255 - Upper_thres, MIN_FAN, MAX_FAN);

      // write speed to fan
      fan.write(fan_val);

    }
    else if (rightjoystick_reading < Lower_thres && !tank_mode) {

      // reverse the damn fan
      fan_reverse.write(180);

      // map the magnitude of the fan (in the upper region of stick)
      fan_val = map(Lower_thres - rightjoystick_reading, 0, Lower_thres, MIN_FAN, MAX_FAN);

      // write this value to the fan
      fan.write(fan_val);

    }

    // if we're between ranges, stop the ESC motor
    else if (!tank_mode) {

      // stop the fan
      fan.write(0);

    }

    // FAN_MODE: check the right joystick setting (make sure it's being set), for rudder
    if ((ljX_reading < Lower_thres || ljX_reading > Upper_thres) && !tank_mode) {

      // map the rudder values from min to max values
      rudder_val = map(ljX_reading, 0, 255, RUDDER_MIN, RUDDER_MAX);

      // get the offset of the rudder command from the servo center command
      int offset_val = rudder_val - RUDDER_CENTER;

      // command the rudder, with the offset in the other direction
      rudder.write(RUDDER_CENTER - offset_val);

    }

    // if it's not getting the signal, set rudder to center
    else if (!tank_mode) {

      rudder.write(RUDDER_CENTER);

    }

    // set the gate position to close
    if (PS4.getButtonClick(R1)) {

      gate.write(GATE_OPEN);

    }

    // set the gate position to open
    if (PS4.getButtonClick(L1)) {

      gate.write(GATE_CLOSE);

    }

    if ((PS4.getAnalogButton(L2) || PS4.getAnalogButton(R2) ) && !button_bap_mode) {

      // start at max position (equilibrium position)
      int scoop_write = MAX_SCOOP;

      // lift the scooper accordingly
      if (PS4.getAnalogButton(L2)) {

        // get the additional offset contributing from the L2 button (for the extra push)
        int scoop_val1 = map(PS4.getAnalogButton(L2), 0, 255, 0, MAX_MAX_SCOOP - MAX_SCOOP);

        // add the contribution to the scoop value
        scoop_write += scoop_val1;

      }

      // lift the scooper accordingly
      if (PS4.getAnalogButton(R2)) {

        // get the scoop command value by mapping R2 input to min and max ranges specified from testing
        int scoop_val2 = map(PS4.getAnalogButton(R2), 0, 255, MIN_SCOOP, MAX_SCOOP);

        // offset vals
        scoop_write = scoop_write - scoop_val2;

      }

      // tell the scooper who's boss
      scoop.write(scoop_write);

    }
    if (PS4.getButtonClick(UP)) {
      //Pressing Up checks the battery's voltage.
      int batt_vol = roboclaw.ReadMainBatteryVoltage(address);

      if (batt_vol > 150) {
        //Voltage is fine, no charge needed.
        PS4.setLed(Green);
      }

      else if (batt_vol > 130) {
        //Voltage is low but not critically low.
        PS4.setLed(Yellow);
        PS4.setLedFlash(100, 100);
      }

      else {
        //Voltage is critically low and the system needs to be shut off.
        PS4.setLed(Red);
      }

    }

    // if we click the down button and are not in button bap mode, we want to enter it
    if (PS4.getButtonClick(DOWN)) {

      if (!button_bap_mode) {

        // change to button bap mode
        button_bap_mode = true;

        // write the button bap position to the scooper
        scoop.write(BUTTON_BAP);

      }

      else {

        // change out of button bap mode
        button_bap_mode = false;

        // change back to the original scooper position
        scoop.write(MAX_SCOOP);

      }

    }

  }

}

void endgame() {

  // check integer for breaking from while loop
  int check = 0;

  // stay in this for some damn reason...
  while (check == 0) {

    // idk what this does
    Usb.Task();

    //Serial.println("We're in the Endgame now");

    if (PS4.connected()) {

      // set LED to purple cuz thanos
      PS4.setLed(255, 0, 255);

      // SHARE button is the unsnap
      if (PS4.getButtonClick(SHARE)) {

        // check value goes to 1 to break from the while loop
        check = 1;

        // reset the LED to blue, because we are back in our previous state
        PS4.setLed(Blue);

      }

    }
  }

}

// YOU THINK THIS IS A JOKE?!!?!??!?!
int cubic_mapv2(int input) {

  // idk, ask spencer what this means 
  return 4 * TANK_MAX_F * pow(input - MAX_CONTROLLER / 2, 3) / pow (MAX_CONTROLLER, 3) +  TANK_MAX_F / 2;

}
