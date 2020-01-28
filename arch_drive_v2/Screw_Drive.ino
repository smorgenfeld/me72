/*Drive System*/

#define speed_MAX_for 127
#define speed_zero 64
#define speed_MAX_back 0

double M1_Signal = 0;
double M2_Signal = 0;

void drive_setup(){
	//Open roboclaw serial ports
	roboclaw.begin(38400);

	//Stop Motors
	drive_reset();
	drive_signaltoscrews();

	Serial.println("Screw drive setup complete.");
}

//Run the actual screws
void drive_operate(){
	drive_reset();

	drive_ps4signal();

	drive_signaltoscrews();
}

//Print statements for debugging
void drive_print(){
	Serial.print("M1 Signal: ");
	Serial.print((uint8_t) round(M1_Signal));
	Serial.print(" | ");
	Serial.print("M2 Signal: ");
	Serial.print((uint8_t) round(M2_Signal));
	Serial.print(" || ");
}

//Reset signal to stop
//TODO: checkk this
void drive_reset(){
	M1_Signal = speed_zero;
	M2_Signal = speed_zero;
}

void drive_ps4signal(){
	M1_Signal = PS4_LHatY;
	M2_Signal = PS4_RHatY;
}

void drive_signaltoscrews(){
	M1_Signal = map(M1_Signal, 0, 255, speed_MAX_for, speed_MAX_back);
	M2_Signal = map(M2_Signal, 0, 255, speed_MAX_for, speed_MAX_back);

	roboclaw.ForwardBackwardM1(address, (uint8_t) round(M1_Signal));
	roboclaw.ForwardBackwardM2(address, (uint8_t) round(M2_Signal));
}
