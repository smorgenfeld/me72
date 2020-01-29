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
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));
}


