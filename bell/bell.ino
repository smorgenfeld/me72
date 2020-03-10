// wireless libraries
#include <PS4BT.h>
#include <Usb.h>
#include <usbhub.h>

// servo motor library
#include <Servo.h>

// roboclaw libraries
#include <RoboClaw.h>

// define actuator pins
#define CAM_PIN 3
#define SCOOP_PIN 4
#define FAN_PIN 5
#define FAN_REVERSE_PIN 6
#define RUDDER_PIN 7

// define the rudder min, max, and center position (on 0 to 180 scale)
#define RUDDER_MIN 20
#define RUDDER_MAX 150
#define RUDDER_CENTER 85

// define min and max positions for the scooper
/* original values
  #define MIN_SCOOP 0
  #define MAX_SCOOP 130*/
#define MIN_SCOOP 3
#define MAX_SCOOP 170
#define BUTTON_BAP 80

// define the min and max speeds for the fan
#define MIN_FAN 0
#define MAX_FAN 100

// controller max
#define MAX_CONTROLLER 255

// shooter variables
#define SHOOTER_MAX 48
#define SHOOTER_STOP 0
#define SHOOTER_START 14

// cam controls
#define CAM_STOP 92
#define CAM_FULL 180
#define CAM_REVERSE 40

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
//
RoboClaw roboclaw(&Serial1, 10000);
//
//
//

#define address 0x80

// set thresholds above and below the controller rest position
int Upper_thres = 137;
int Lower_thres = 117;

int left_power;
int right_power;
int leftjoystick_reading;
int rightjoystick_reading;
int ljX_reading;

// define shooter speed
int shooter_speed = SHOOTER_START;
bool shooter_active = false;

// button bapping mode
bool button_bap_mode = false;

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
  cam.attach(CAM_PIN, 1000, 2000);

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
      roboclaw.ForwardM1(address, SHOOTER_STOP);
      roboclaw.ForwardM2(address, SHOOTER_STOP);

      // stop the fan
      fan.write(0);

      // stop the cam
      cam.write(CAM_STOP);

      // THANOS SNAPS...
      endgame();

      // stop the tank treads
      roboclaw.ForwardM1(address, SHOOTER_STOP);
      roboclaw.ForwardM2(address, SHOOTER_STOP);

      // stop the fan
      fan.write(0);

      // stop the cam
      cam.write(CAM_STOP);

    }

    // lift the scooper accordingly
//    if (PS4.getAnalogButton(L2)) {
//
//      // get the additional offset contributing from the L2 button (for the extra push)
//      int cam_val = map(PS4.getAnalogButton(L2), 0, 255, CAM_STOP, CAM_FULL);
//
//      // add the contribution to the scoop value
//      cam.write(cam_val);
//
//    }

    // move the came in reverse
    if (PS4.getAnalogButton(L2) > 30) {

      // get the additional offset contributing from the L2 button (for the extra push)
      //      int cam_val = map(PS4.getAnalogButton(L2), 0, 255, CAM_STOP, CAM_FULL);

      PS4.setLed(Red);

      // add the contribution to the scoop value
      cam.write(CAM_REVERSE);

    }

    if (PS4.getButtonClick(L1)) {

      cam.write(CAM_STOP);
      PS4.setLed(Blue);

    } else if (PS4.getButtonClick(R1)) {
      cam.write(CAM_FULL);
      PS4.setLed(Blue);

    }

    // FAN_MODE: check the right joystick setting (make sure it's being set)
    if (rightjoystick_reading > Upper_thres) {

      // make sure fan is moving forwards
      fan_reverse.write(0);

      // map the fan values from 0 to 180 (in the lower stick area)
      fan_val = map(rightjoystick_reading - Upper_thres, 0, 255 - Upper_thres, MIN_FAN, MAX_FAN);

      // write speed to fan
      fan.write(fan_val);

    }
    else if (rightjoystick_reading < Lower_thres) {

      // reverse the damn fan
      fan_reverse.write(180);

      // map the magnitude of the fan (in the upper region of stick)
      fan_val = map(Lower_thres - rightjoystick_reading, 0, Lower_thres, MIN_FAN, MAX_FAN);

      // write this value to the fan
      fan.write(fan_val);

    }

    // if we're between ranges, stop the ESC motor
    else {

      // stop the fan
      fan.write(0);

    }

    // FAN_MODE: check the right joystick setting (make sure it's being set), for rudder
    if (ljX_reading < Lower_thres || ljX_reading > Upper_thres) {

      // map the rudder values from min to max values
      rudder_val = map(ljX_reading, 0, 255, RUDDER_MIN, RUDDER_MAX);

      // get the offset of the rudder command from the servo center command
      int offset_val = rudder_val - RUDDER_CENTER;

      // command the rudder, with the offset in the other direction
      rudder.write(RUDDER_CENTER + offset_val);

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

    if (PS4.getAnalogButton(R2) && !button_bap_mode) {

      // start at max position (equilibrium position)
      //      int scoop_write = MAX_SCOOP;


      // get the scoop command value by mapping R2 input to min and max ranges specified from testing
      //        int scoop_val2 = map(PS4.getAnalogButton(R2), 0, 255, MIN_SCOOP, MAX_SCOOP);

      // offset vals
      //      scoop_write = scoop_write - scoop_val2;

      int scoop_write = map(PS4.getAnalogButton(R2), 0, 255, MIN_SCOOP, MAX_SCOOP);

      // tell the scooper who's boss
      scoop.write(scoop_write);

    }

    if (PS4.getButtonClick(RIGHT) && shooter_active) {

      // increment shooter speed
      if (shooter_speed < SHOOTER_MAX) {

        // increment the shooter speeds
        shooter_speed += 1;

      }

      // set power of the flywheels
      roboclaw.ForwardM1(address, shooter_speed);
      roboclaw.ForwardM2(address, shooter_speed);

    }
    if (PS4.getButtonClick(LEFT) && shooter_active) {

      // check shooter speed is above minimum value
      if (shooter_speed > SHOOTER_STOP) {

        // decrease the shooter speed by 1
        shooter_speed -= 1;

      }

      // set power of the flywheels
      roboclaw.ForwardM1(address, shooter_speed);
      roboclaw.ForwardM2(address, shooter_speed);

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
        scoop.write(MIN_SCOOP);

      }

    }

    // to turn the shooter off and on
    if (PS4.getButtonClick(TRIANGLE)) {

      // invert active shooter condition
      shooter_active = !shooter_active;

      if (shooter_active) {

        PS4.setLed(Yellow);

        shooter_speed = SHOOTER_START;

        // set power of the flywheels
        roboclaw.ForwardM1(address, shooter_speed);
        roboclaw.ForwardM2(address, shooter_speed);

      }
      else {

        PS4.setLed(Blue);

        // set the new motor speed (turn it off)
        shooter_speed = 0;

        // set power of the flywheels
        roboclaw.ForwardM1(address, shooter_speed);
        roboclaw.ForwardM2(address, shooter_speed);


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
