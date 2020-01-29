//Define Thresholds
#define Upper_thres 137
#define Lower_thres 117

//Define Thresholds and initialize power readings 
int left_power;
int right_power;
int leftjoystick_reading;
int rightjoystick_reading;

//Roboclaw Setup
void rc_setup(void)
{
  //Serial for RoboClaw
  roboclaw.begin(38400); 
  delay(2000);

    //Set RoboClaw Limits
  roboclaw.SetMainVoltages(address, 130, 190); //(address, minV, maxV) based on 0.1V, so 130 -> 13V
}

void drive_screws(void)
{
    leftjoystick_reading = PS4.getAnalogHat(LeftHatY);
    rightjoystick_reading = PS4.getAnalogHat(RightHatY);

    left_power = map(leftjoystick_reading, 0, 255, 96, 32u); //Maps analog output of joystick to ForwardsBackwards
    right_power = map(rightjoystick_reading, 0, 255, 96, 32); //Maps analog output to ForwardsBackwards levels

    if(leftjoystick_reading < Lower_thres || leftjoystick_reading > Upper_thres){
      roboclaw.ForwardBackwardM1(address, left_power);
      Serial.print("Left Power: ");
      Serial.println(left_power);
    }
    
    else{
      roboclaw.ForwardBackwardM1(address, 64);
    }

    if (rightjoystick_reading < Lower_thres || rightjoystick_reading > Upper_thres){
      roboclaw.ForwardBackwardM2(address, right_power);
      Serial.print("Right Power: ");
      Serial.print(right_power);
      Serial.print(" Right Joystick Reading: ");
      Serial.println(rightjoystick_reading);
    } 

    else{
      roboclaw.ForwardBackwardM2(address, 64);
    } 
}

