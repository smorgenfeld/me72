/* Constants */
#define PS4_STICK_NEG 0
#define PS4_STICK_POS 255
#define PS4_STICK_ZERO 127

#define lowBound 117
#define upBound 137


/* System Variables */
USB Usb;
BTD Btd(&Usb);
PS4BT PS4(&Btd);

double PS4_LHatY; double PS4_RHatY;
double PS4_LHatX; double PS4_RHatX;

double PS4_R1; double PS4_L1;
double PS4_R3; double PS4_L3;

double PS4_UP; double PS4_DOWN;
double PS4_RIGHT; double PS4_LEFT;

double PS4_Circle; double PS4_Square;
double PS4_Triangle; double PS4_Cross;

//Need to add all the parameters added into the operate function

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

	Usb.Task();

	if(PS4.connected()){
		PS4_getSignal();
	}
}


/* Functions */
void PS4_getSignal(){
	PS4_R1 = PS4.getButtonClick(R1);
	PS4_L1 = PS4.getButtonClick(L1);

	PS4_R3 = PS4.getButtonClick(R3);
	PS4_L3 = PS4.getButtonClick(L3);

	PS4_Circle = PS4.getButtonClick(CIRCLE);
	PS4_Square = PS4.getButtonClick(SQUARE);
	PS4_Triangle = PS4.getButtonClick(TRIANGLE);
	PS4_Cross = PS4.getButtonClick(CROSS);

	PS4_UP = PS4.getButtonClick(UP);
	PS4_DOWN = PS4.getButtonClick(DOWN);
	PS4_LEFT = PS4.getButtonClick(LEFT);
	PS4_RIGHT = PS4.getButtonClick(RIGHT);


	PS4_LHatX = PS4.getAnalogHat(LeftHatX);
	PS4_RHatX = PS4.getAnalogHat(RightHatX);
	PS4_LHatY = PS4.getAnalogHat(LeftHatY);
	PS4_RHatY = PS4.getAnalogHat(RightHatY);
}

void PS4_processSignal(){
	if (!(PS4_LHatY < lowBound || PS4_LHatY > upBound)){
		PS4_LHatY =  PS4_STICK_ZERO;
	if (!(PS4_RHatY < lowBound || PS4_RHatY > upBound)){
		PS4_RHatY = PS4_STICK_ZERO;
	}
 }
}

void PS4_reset(){
	PS4_LHatX = PS4_STICK_ZERO;
	PS4_RHatX = PS4_STICK_ZERO;
	PS4_LHatY = PS4_STICK_ZERO;
	PS4_RHatY = PS4_STICK_ZERO;
}

