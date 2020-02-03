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
  #define FAN_REVERSE_PIN 9
//  #define GATE_PIN 11
//  #define GATE_POS_F 180

//  Servo gate; 
  
  // define the rudder min, max, and center position (on 0 to 180 scale)
  #define RUDDER_MIN 0
  #define RUDDER_MAX 100
  #define RUDDER_CENTER 50
  
  // define the ZERO, MAX Forward, and MAX Backward speed for RS550
#define TANK_MAX_F 127
#define TANK_MAX_B 0
#define TANK_ZERO 64

// servo objects for our motors
Servo rudder;
Servo fan;
Servo fan_reverse;

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


// boolean for when the fan is moving forward
bool fan_forward = true;

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
  fan_reverse.attach(FAN_REVERSE_PIN, 1000, 2000);
//  gate.attach(GATE_PIN);

//  pinMode(GATE_PIN, INPUT);

  // initially start the fan in the forward direction
  fan_reverse.write(0);

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

        rudder.attach(RUDDER_PIN);
        pinMode(FAN_PIN, OUTPUT);
        //fan.attach(FAN_PIN);

        //pinMode(RUDDER_PIN, OUTPUT);

        // if we're leaving tank mode, adjust the tank motors to no speed
        roboclaw.ForwardBackwardM1(address, TANK_ZERO);
        roboclaw.ForwardBackwardM2(address, TANK_ZERO);

        // change PS4 LED to green (GREEN = FAN MODE)
        PS4.setLed(Blue);

      }
      else {
        // change PS4 LED to blue (BLUE = TANK MODE)
        PS4.setLed(Green);

        //pinMode(RUDDER_PIN, INPUT);

        // if we're leaving fan mode, center the rudder and turn off fan
        rudder.write(RUDDER_CENTER);
        fan.write(0);

        pinMode(FAN_PIN, INPUT);
        rudder.detach();
        //fan.detach();

      }

      // adjust the robot's state
      tank_mode = !tank_mode;
    }

//
//if (PS4.getButtonClick(SQUARE)) {
//      Serial.println(F("\r\\nSquare"));
//      pinMode(GATE_PIN, OUTPUT);
//
//
//      // set the gate movement
//      gate.write(GATE_POS_F);
//      
//
//    }









    

    // TANK_MODE: check the right joystick setting (make sure it's being set)
    if ((leftjoystick_reading < Lower_thres || leftjoystick_reading > Upper_thres) && tank_mode) {

      // get the correct power setting for the left and right motors
      left_power = map(leftjoystick_reading, 0, 255, TANK_MAX_F, TANK_MAX_B); //Maps analog output of joystick to ForwardsBackwards

      // set power of left tank motor
      roboclaw.ForwardBackwardM1(address, left_power);
      Serial.print("Left tank Power: ");
      Serial.println(left_power);
      //fan.write(0);
    }

    // if it's not getting the signal, set left tank power to zero
    else if (tank_mode) {
      roboclaw.ForwardBackwardM1(address, TANK_ZERO);
      //fan.write(0);
    }

    // TANK_MODE: check the right joystick setting (make sure it's being set)
    if ((rightjoystick_reading < Lower_thres || rightjoystick_reading > Upper_thres) && tank_mode) {
      // get the correct power setting for the left and right motors
      right_power = map(rightjoystick_reading, 0, 255, TANK_MAX_F, TANK_MAX_B); //Maps analog output to ForwardsBackwards levels
      
      // set power of right tank motor
      roboclaw.ForwardBackwardM2(address, right_power);
      Serial.print("Right tank Power: ");
      Serial.println(right_power);
      //fan.write(0);
    }

    // if it's not getting the signal, set right tank power to zero
    else if (tank_mode) {
      roboclaw.ForwardBackwardM2(address, TANK_ZERO);
      //fan.write(0);
    }

    // FAN_MODE: check the right joystick setting (make sure it's being set)
    if (rightjoystick_reading > Upper_thres && !tank_mode) {

//      // if the fan was going forward and we're commanding to reverse, we need to change direction state
//      if(fan_forward) {
//
//         // stop the fan
//         fan.write(0);
//
//         // send command to reverse the direction of the ESC
//         fan_reverse.write(180);
//
//         // reverse fan direction state boolean
//         fan_forward = !fan_forward;
//        
//      }
      fan_reverse.write(180);

      Serial.print(fan_reverse.read());
 
      // map the fan values from 0 to 180 (in the lower stick area)
      fan_val = map(rightjoystick_reading - Upper_thres, 0, 255 - Upper_thres, 0, 180);

      // write speed to fan
      fan.write(fan_val);
      Serial.print("Fan power (reverse): ");
      Serial.println(fan_val);
    }
    else if(rightjoystick_reading < Lower_thres && !tank_mode) {

      // if the fan was going reverse and we're commanding to forward, we need to change direction state
//      if(!fan_forward) {
//
//         // stop the fan
//         fan.write(0);
//
//         // send command to reverse the direction of the ESC
//         fan_reverse.write(0);
//
//         // reverse fan direction state boolean
//         fan_forward = !fan_forward;
//        
//      }

      fan_reverse.write(0);

      Serial.print(fan_reverse.read());
      // map the magnitude of the fan (in the upper region of stick)
      fan_val = map(Lower_thres - rightjoystick_reading, 0, Lower_thres, 0, 180);

      // write this value to the fan
      fan.write(fan_val);
      
      Serial.print("Fan power (forward): ");
      Serial.println(fan_val);
        
    }
    // if we're between ranges, stop the ESC motor
    else if (!tank_mode) {

      // stop the fan
      fan.write(0);  

      Serial.print("Stopping fan"); 
      
    }

    // FAN_MODE: check the right joystick setting (make sure it's being set), for rudder
    if ((ljX_reading < Lower_thres || ljX_reading > Upper_thres) && !tank_mode) {

      // map the rudder values from min to max values
      rudder_val = map(ljX_reading, 0, 255, RUDDER_MIN, RUDDER_MAX);

      rudder.write(RUDDER_MAX - rudder_val);
      Serial.print("Rudder pos: ");
      Serial.println(rudder_val);
    }

    // if it's not getting the signal, set rudder to center
    else if(!tank_mode){
      rudder.write(RUDDER_CENTER);
      Serial.print("Rudder pos: ");
      Serial.println(RUDDER_CENTER);
    }

  }
//  Serial.print("Fan speed: ");
//  Serial.print(fan.read());
//
//  Serial.print("    Fan direction: ");
//  Serial.println(fan_reverse.read());

}


void endgame() {
  
  // stay in this for some damn reason...safety? what?
  while(true) {
    
    Serial.println("We're in the Endgame now");   
    
    
  }  
  
  
}
