//-------------------------------------
//-------------DEFINITIONS-------------
//-------------------------------------

/*Define Motor Pins for Flywheel
  #Steps:
 1.Connect the M1_PWM & M2_PWM to UNO digital 3 & 4
 2.Connect the M1_EN & M2_EN to UNO digital 2 & 5
 3.Connect +5V & GND to UNO 5V & GND
 */
//Flywheel Motor Pins
#define E1 3 //M1 Speed Control
#define E2 5 //M2 Speed Control
#define M1 22 //M1 Direction Control
#define M2 23 //M2 Direction Control


//Shooter Definitions
Servo belt;

#define BELT_PIN 35
#define BELT_SPEED 180 //Check what this speed it (0 max on direction, 180 max oter direction)
#define BELT_STOP 95 
int WHEEL_SPEED = 17;


//Scooper Definitions
Servo scooper;

#define SCOOP_PIN 7
#define SCOOP_DOWN 60
#define SCOOP_UP 160
#define SCOOP_ANGLE 5

int scooper_pos = 90;
int grabber_pos = 90;
bool detached;

//Grabber Definitions
Servo grabber;

#define GRAB_PIN 2
#define GRAB_DOWN 80
#define GRAB_UP 160
#define GRAB_ANGLE -5

bool grabber_on;

//-------------------------------------
//--------------SETUP------------------
//-------------------------------------

void ball_setup(void)
{
  //Writes the scooper in the up position to start.
  scooper.attach(SCOOP_PIN);
  scooper.write(SCOOP_UP - 10);

  //Sets the grabber in the UP position to start.
  grabber.attach(GRAB_PIN);
  grabber.write(GRAB_UP);

  //Sets the motor pins as OUTPUTS.
  int i;
  for(i=2;i<=5;i++)
    pinMode(i, OUTPUT);  
  digitalWrite(E1,LOW);   
  digitalWrite(E2,LOW);
}

//-------------------------------------
//-------------SHOOTER-----------------
//-------------------------------------

//Flywheels
void shooter_stop(void)                    //Stop
{
  //Stops the flywheels
  digitalWrite(E1,0); 
  digitalWrite(M1,LOW);    
  digitalWrite(E2,0);   
  digitalWrite(M2,LOW);    
}   

void shoot_ball ()             
{
  //Turns on the flywheel to the defined WHEEL_SPEED
  analogWrite (E1,WHEEL_SPEED);
  digitalWrite(M1,HIGH);    
  analogWrite (E2,WHEEL_SPEED);    
  digitalWrite(M2,LOW);

  if (PS4.getButtonClick(R1)){
    WHEEL_SPEED += 1;
    Serial.print("Wheel speed: ");
    Serial.println(WHEEL_SPEED);
  }

  if (PS4.getButtonClick(L1)){
    WHEEL_SPEED -= 1;
    Serial.print("Wheel speed: ");
    Serial.println(WHEEL_SPEED);
  }
}

//Loading belt
void belt_stop(){
  //Stops the belt
  belt.attach(BELT_PIN);
  belt.write(BELT_STOP);
  belt.detach();
}

void belt_run(){
  //Runs the belt
  belt.attach(BELT_PIN);
  belt.write(BELT_SPEED);
}

//-------------------------------------
//--------------SERVOS-----------------
//-------------------------------------

void servodpad(bool grab_attach)
{

  if (grab_attach){
    //If the grabber is supposed to be on, the grabber is reattached and controlled by the Dpad
    grabber.attach(GRAB_PIN);
    grabber_pos = dpad_pos(grabber_pos, GRAB_DOWN, GRAB_UP, GRAB_ANGLE); //Sets the position based on the Dpad
    grabber.write(grabber_pos);
    //PS4.setLed(Green);
    detached = false; //Declares that the grabber is in fact detached.
  }

  else{
    if (!detached){
      //If the first time going through the loop, detaches the grabber.
      grabber.detach();
    }
    scooper_pos = dpad_pos(scooper_pos, SCOOP_DOWN, SCOOP_UP, SCOOP_ANGLE); //Sets the position based on the Dpad
    scooper.write(scooper_pos);
    //PS4.setLed(Red);
  }
}

int dpad_pos(int pos, int minval, int maxval, int angleval){  
  int oldpos = pos;
  
  if (pos >= 0 && pos <= 180){
    if(PS4.getButtonClick(UP))
    {
      //If pressing up button, raise the servo
      pos += angleval;
    }

    else if (PS4.getButtonClick(DOWN))
    {
      //DOWN button lowers the servo arm
      pos -= angleval;
    }

    else if (PS4.getButtonClick(LEFT)){
      //LEFT sets the servo arm to the lowest position.
      pos = minval;
    }

    else if (PS4.getButtonClick(RIGHT)){
      //RIGHT sets the servo arm to the highest position.
      pos = maxval;
    }
  }

  if (pos > 180){
    //Doesn't allow pos to be above 180
    pos = 180;
  }

  else if (pos < 0){
    //Doesn't allow pos to be below 0
    pos = 0;
  }
  
  return pos;
}


//ANALOG CONTROLS: TRIGGERS.

int analog_scoop(int buttonread){
  int pos = map(buttonread, 0, 255, SCOOP_UP, SCOOP_DOWN);
  scooper.write(pos);
}

void analog_grabber(int buttonread){
  int pos = map(buttonread, 0, 255, GRAB_UP, GRAB_DOWN);
  grabber.write(pos);
}




