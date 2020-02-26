//-------------------------------------
//-------------DEFINITIONS-------------
//-------------------------------------


//Motor Defintions
#define MOTOR_MIN 32
#define MOTOR_MAX 96
#define MOTOR_STOP 64

int left_power;
int right_power;

//Joystick Definitions
#define JOYSTICK_STOP 127
#define JOYSTICK_MIN 0
#define JOYSTICK_MAX 255
#define Upper_thres 137
#define Lower_thres 117

int leftjoystick_reading;
int rightjoystick_reading;

//-------------------------------------
//--------------SETUP------------------
//-------------------------------------

void rc_setup(void)
{
  //Serial for RoboClaw
  roboclaw.begin(38400); 
  delay(2000);

    //Set RoboClaw Limits
  roboclaw.SetMainVoltages(address, 130, 190); //(address, minV, maxV) based on 0.1V, so 130 -> 13V
}

int cubic_map(int x, int motor_min, int joystick_stop, int motor_stop){
  return (motor_min - motor_stop)/pow(-joystick_stop,3) * pow(x - joystick_stop,3) + motor_stop;
}

int cubic_mapv2(int input) {

  // idk, ask spencer what this means 
  return 4 * MOTOR_MAX * pow(input - JOYSTICK_MAX / 2, 3) / pow (JOYSTICK_MAX, 3) +  MOTOR_MAX / 2;
}


//-------------------------------------
//------------LOOP FUNCTION------------
//-------------------------------------
void screw_operate(bool safe, bool mapmode){
  if (safe){
    drive_screws(mapmode);
  }

  else{
    motor_stop();
  }
}
//-------------------------------------
//------------DRIVE MODES--------------
//-------------------------------------
void drive_screws(bool cubic)
{
   //Read the joystick levels. TODO: Integrate into PS4 instead of in ScrewDrive
    leftjoystick_reading = PS4.getAnalogHat(LeftHatY);
    rightjoystick_reading = PS4.getAnalogHat(RightHatY);

    //Check the map mode.
    if(!cubic){
      PS4.setLedFlash(10, 10);
      left_power = map(leftjoystick_reading, JOYSTICK_MIN, JOYSTICK_MAX, MOTOR_MIN, MOTOR_MAX); //Maps analog output of joystick to ForwardsBackwards
      right_power = map(rightjoystick_reading, JOYSTICK_MIN, JOYSTICK_MAX, MOTOR_MIN, MOTOR_MAX); //Maps analog output to ForwardsBackwards levels
    }

    else{
      PS4.setLedFlash(0, 0);
      left_power = cubic_map(leftjoystick_reading, MOTOR_MIN, JOYSTICK_STOP, MOTOR_STOP);
      right_power = cubic_map(rightjoystick_reading, MOTOR_MIN, JOYSTICK_STOP, MOTOR_STOP);

      
    }

    //Power the motors based on the computed power.
    if(leftjoystick_reading < Lower_thres || leftjoystick_reading > Upper_thres){
      roboclaw.ForwardBackwardM2(address, left_power);
    }
    
    else{
      roboclaw.ForwardBackwardM2(address, MOTOR_STOP);
    }

    if (rightjoystick_reading < Lower_thres || rightjoystick_reading > Upper_thres){
      roboclaw.ForwardBackwardM1(address, right_power);
    } 

    else{
      roboclaw.ForwardBackwardM1(address, MOTOR_STOP);
    } 
}

void motor_stop(){
  //Safety function, commands both motors to stop. Should be able to be done by just not using joysticks, but as an added precaution. 
  roboclaw.ForwardBackwardM1(address, MOTOR_STOP);
  roboclaw.ForwardBackwardM2(address, MOTOR_STOP);
}

