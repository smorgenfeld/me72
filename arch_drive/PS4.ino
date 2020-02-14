////Define Variables
//
//USB Usb;
//BTD Btd(&Usb);
//PS4BT PS4(&Btd);

void PS4_setup(void)
{
  Serial.begin(115200);
  Serial.print("Main startup... ");

  #if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  #endif

  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.println(F("\r\nPS4 Bluetooth Library Started"));
}


void ps4_operate(){
  //TODO: Put all PS4 control stuff in here. 
  if (PS4.getAnalogButton(R2)){
    //Controls the scooper by the R2 trigger.
    analog_scoop(PS4.getAnalogButton(R2));
    //PS4.setLed(PS4.getAnalogButton(R2), 0, 255 - PS4.getAnalogButton(R2));
  }

  if (PS4.getAnalogButton(L2)){
    //Controls the scooper by the L2 Trigger. 
    analog_grabber(PS4.getAnalogButton(L2));
  }
}

void safe_mode(){
  //TODO: safety mode such that if the voltage is too low the system stops running to prevent a LiPo kill.
  motor_stop();
}

