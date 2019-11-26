#include "arch_drive.h"
#include "PS4BT.h"
#include "usbhub.h"
#include "RoboClaw.h"
#include "SPI.h"

void motor_setup(){
	SoftwareSerial serial(10,11);	
	RoboClaw roboclaw(&serial,10000);

	#define address 0x80
}
void bt_setup(){

	USB Usb;
	//USBHub Hub1(&Usb); // Some dongles have a hub inside

	BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

	/* You can create the instance of the PS4BT class in two ways */
	// This will start an inquiry and then pair with the PS4 controller - you only have to do this once
	// You will need to hold down the PS and Share button at the same time, the PS4 controller will then start to blink rapidly indicating that it is in pairing mode
	PS4BT PS4(&Btd, PAIR);

	// After that you can simply create the instance like so and then press the PS button on the device
	//PS4BT PS4(&Btd);

	bool printAngle, printTouch;
	uint8_t oldL2Value, oldR2Value;

	int Upper_thres = 137;
	int Lower_thres = 117;
}
void arch_drive_setup(){
	//Serial for USB Host over main serial
	Serial.begin(115200);
#if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
	 if (Usb.Init() == -1) {
	   Serial.print(F("\r\nOSC did not start"));
	   while (1); // Halt
	 }
  	Serial.print(F("\r\nPS4 Bluetooth Library Started"));

  	//Serial for Roboclaw
	Serial1.begin(38400); 
}

void screw_drive(){
	if (PS4.connected())
	{
		leftjoystick_reading = PS4.getAnalogHat(LHatX);
		rightjoystick_reading = PS4.getAnalogHat(RHatX);

		/*TODO: Fix Map levels. Best guess is 157 is the middle of the controller. To be tested
		when controller connection working*/
		left_power = map(leftjoystick_reading, 0, 255, 84, 44); //Maps analog output of joystick to ForwardsBackwards
		right_power = map(rightjoystick_reading, 0, 255, 84, 44); //Maps analog output to ForwardsBackwards levels

		if(leftjoystick_reading < Lower_thres || leftjoystick_reading > Upper_thres){
			roboclaw.ForwardsBackwardsM1(address, left_power);
			Serial.print("Left Power: ")
			Serial.print(left_power);
		}

		if (rightjoystick_reading < Lower_thres || rightjoystick_reading > Upper_thres)
		{
			roboclaw.ForwardsBackwardsM2(address, right_power);
		}
	}
}