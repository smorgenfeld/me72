// wireless libraries
#include <PS4BT.h>
#include <Usb.h>
#include <usbhub.h>

// servo motor library
#include <Servo.h>

// roboclaw libraries
#include <RoboClaw.h>

// define actuator pins
#define SCOOP_PIN 4
#define FAN_PIN 5
#define FAN_REVERSE_PIN 6
#define RUDDER_PIN 7

// define the rudder min, max, and center position (on 0 to 180 scale)
#define RUDDER_MIN 30
#define RUDDER_MAX 150
#define RUDDER_CENTER 85

// define min and max positions for the scooper
#define MIN_SCOOP 0
#define MAX_SCOOP 130
#define MAX_MAX_SCOOP 160

// define the min and max speeds for the fan
#define MIN_FAN 0
#define MAX_FAN 100

// controller max
#define MAX_CONTROLLER 255

// cam controls
#define CAM_PIN 3
#define CAM_STOP 90
#define CAM_FULL 180

// servo objects for our motors
Servo rudder;
Servo fan;
Servo fan_reverse;
Servo scoop;
Servo cam;

// Roboclaw serial communication using software serial
//SoftwareSerial serial(24, 25);
//RoboClaw roboclaw(&serial, 10000);

// Trying to use Serial port to avoid softwareserial
//
//**** uncomment
RoboClaw roboclaw(&Serial,10000);
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

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

//*
//* DO NOT UNCOMMENT THE NEXT LINE, WE NO LONGER WANT TO PAIR
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
//  Serial.begin(115200);
#if !defined(__MIPSEL__)
//    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
//        Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
//    Serial.print(F("\r\nPS4 Bluetooth Library Started"));

  //Serial for RoboClaw
  roboclaw.begin(38400);

  delay(2000);

  //Set RoboClaw Limits
  roboclaw.SetMainVoltages(address, 130, 190); //(address, minV, maxV) based on 0.1V, so 130 -> 13V

  // attach motors to their pins
  rudder.attach(RUDDER_PIN);
  fan.attach(FAN_PIN, 1000, 2000);
  fan_reverse.attach(FAN_REVERSE_PIN, 1000, 2000);
  scoop.attach(SCOOP_PIN);
  cam.attach(CAM_PIN, 1000,2000);

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

      // THANOS SNAPS...
      endgame();

    }

    // lift the scooper accordingly
      if (PS4.getAnalogButton(L2)) {

        // get the additional offset contributing from the L2 button (for the extra push)
        int cam_val = map(PS4.getAnalogButton(L2), 0, 255, CAM_STOP, CAM_FULL);

        // add the contribution to the scoop value
        cam.write(cam_val);

      }
   
    // FAN_MODE: check the right joystick setting (make sure it's being set)
    if (rightjoystick_reading > Upper_thres) {

      // make sure fan is moving forwards
      fan_reverse.write(0);

      // map the fan values from 0 to 180 (in the lower stick area)
      fan_val = map(rightjoystick_reading - Upper_thres, 0, 255 - Upper_thres, MIN_FAN, MAX_FAN);

      // write speed to fan
      fan.write(fan_val);

      // set the LED color 
      PS4.setLed(Yellow);

    }
    else if (rightjoystick_reading < Lower_thres) {

      // reverse the damn fan
      fan_reverse.write(180);

      // map the magnitude of the fan (in the upper region of stick)
      fan_val = map(Lower_thres - rightjoystick_reading, 0, Lower_thres, MIN_FAN, MAX_FAN);

      // write this value to the fan
      fan.write(fan_val);
      
      PS4.setLed(Yellow);

    }

    // if we're between ranges, stop the ESC motor
    else {

      // stop the fan
      fan.write(0);

      //PS4.setLed(Yellow);

    }

    // FAN_MODE: check the right joystick setting (make sure it's being set), for rudder
    if (ljX_reading < Lower_thres || ljX_reading > Upper_thres) {

      // map the rudder values from min to max values
      rudder_val = map(ljX_reading, 0, 255, RUDDER_MIN, RUDDER_MAX);

      // get the offset of the rudder command from the servo center command
      int offset_val = rudder_val - RUDDER_CENTER;

      // command the rudder, with the offset in the other direction
      rudder.write(RUDDER_CENTER - offset_val);

    }

    // if it's not getting the signal, set rudder to center
    else {

      rudder.write(RUDDER_CENTER);

    }

    // to get the battery voltage
    if (PS4.getButtonClick(UP)) {
      //Pressing Up checks the battery's voltage.
      int batt_vol = roboclaw.ReadMainBatteryVoltage(address);

      if (batt_vol > 150) {
        //Voltage is fine, no charge needed.
        PS4.setLed(Green);
      }

      else if (batt_vol > 130) {
        //Voltage is low but not critically low.
        PS4.setLed(Blue);
        PS4.setLedFlash(100, 100);
      }

      else {
        //Voltage is critically low and the system needs to be shut off.
        PS4.setLed(Red);
      }

    }

    if (PS4.getAnalogButton(R2)) {

      // start at max position (equilibrium position)
      int scoop_write = MAX_SCOOP;

//      // lift the scooper accordingly taking out, because bump functionality isn't necessary
//      if (PS4.getAnalogButton(L2)) {
//
//        // get the additional offset contributing from the L2 button (for the extra push)
//        int scoop_val1 = map(PS4.getAnalogButton(L2), 0, 255, 0, MAX_MAX_SCOOP - MAX_SCOOP);
//
//        // add the contribution to the scoop value
//        scoop_write += scoop_val1;
//
//      }

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


  }

}

void endgame() {

  // stay in this for some damn reason...safety? what?
  while (true) {
    // idk what this does
    Usb.Task();
    //Serial.println("We're in the Endgame now");

    if (PS4.connected()) {

      // set LED to purple
      PS4.setLed(255, 0, 255);

    }
  }

}

// YOU THINK THIS IS A JOKE?!!?!??!?!
/*int cubic_mapv2(int input) {

  // idk, ask spencer what this means 
  return 4 * TANK_MAX_F * pow(input - MAX_CONTROLLER / 2, 3) / pow (MAX_CONTROLLER, 3) +  TANK_MAX_F / 2;

}*/
