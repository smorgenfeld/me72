/* Constants */
#include "usbhub.h"
#include "PS4BT.h"

#define PS4_STICK_NEG 0
#define PS4_STICK_POS 255


/* System Variables */
USB usb;
BTD Btd(&Usb);
PS4BT PS4(&Btd);

double PS4_LHatY; double PS4_RHatY;
double PS4_LHatX; double PS4_RHatX;
//There's more but add as needed?

/* Main Functions */

void PS4_setup(){
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));
}

void PS4_operate(){
	PS4_reset();

	usb.Task();

	if(PS4.connected()){
		PS4_getSignal();
	}
}


/* Functions */
void PS4_getSignal(){
	PS4_LHatX = PS4.getAnalogHat(LeftHatX, 127);
	PS4_RHatX = PS4.getAnalogHat(RightHatX, 127);
	PS4_LHatY = PS4.getAnalogHat(LeftHatY, 127);
	PS4_RHatY = PS4.getAnalogHat(RightHatY, 127);
}

void PS4_reset(){
	PS4_LHatX = 127;
	PS4_RHatX = 127;
	PS4_LHatY = 127;
	PS4_RHatY = 127;
}
