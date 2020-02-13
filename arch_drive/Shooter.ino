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
#define WHEEL_SPEED 16


//Scooper Definitions
Servo scooper;

#define SCOOP_PIN 7
#define SCOOP_UP 100
#define SCOOP_DOWN 170
#define SCOOP_ANGLE 10

int scooper_pos = 90;
int grabber_pos = 90;
bool detached;

//Grabber Definitions
Servo grabber;

#define GRAB_PIN 2
#define GRAB_DOWN 100
#define GRAB_UP 170
#define GRAB_ANGLE -5

bool grabber_on;

//-------------------------------------
//--------------SETUP------------------
//-------------------------------------

void ball_setup(void)
{
  scooper.attach(SCOOP_PIN);
  scooper.write(SCOOP_DOWN);
//  scooper.detach();
//  scoop_up = false;

  grabber.attach(GRAB_PIN);
  grabber.write(GRAB_UP);
//  grabber.detach();
  grabber_on = false;
  
  int i;
  for(i=2;i<=5;i++)
    pinMode(i, OUTPUT);  
  digitalWrite(E1,LOW);   
  digitalWrite(E2,LOW);
}

//-------------------------------------
//-------------SHOOTER-----------------
//-------------------------------------


void stop(void)                    //Stop
{
  belt.attach(BELT_PIN);
  belt.write(BELT_STOP);
  belt.detach();
  
  digitalWrite(E1,0); 
  digitalWrite(M1,LOW);    
  digitalWrite(E2,0);   
  digitalWrite(M2,LOW);    
}   

void shoot_ball ()             //Turn Right: Shooter Direction Forward
{
  belt.attach(BELT_PIN);
  belt.write(BELT_SPEED);
  
  analogWrite (E1,WHEEL_SPEED);
  digitalWrite(M1,HIGH);    
  analogWrite (E2,WHEEL_SPEED);    
  digitalWrite(M2,LOW);

  //belt.detach();
}

//-------------------------------------
//-------------SCOOPER-----------------
//-------------------------------------

/*void scoop_ball (void){
  if(!scoop_up)
  {
//    scooper.attach(SCOOP_PIN);
    scooper.write(SCOOP_UP);
    //scooper.detach();
    PS4.setLedFlash(10,10);
  }

  else
  {
//    scooper.attach(SCOOP_PIN);
    scooper.write(SCOOP_DOWN);
//    Serial.println("Scooper Down");
    //scooper.detach();
    PS4.setLedFlash(0, 0);
  }

  scoop_up = !scoop_up;
}*/

//-------------------------------------
//-------------GRABBER-----------------
//-------------------------------------

void grab_tower (void){
  if(!grabber_on)
  {
//    grabber.attach(GRAB_PIN);
    grabber.write(GRAB_DOWN);
//    Serial.println(" Grabber Down ");
//    grabber.detach();
  }

  else
  {
//    grabber.attach(GRAB_PIN);
    grabber.write(GRAB_UP);
    Serial.println(" Grabber Up ");
//    grabber.detach();
  }

  grabber_on = !grabber_on;
}

//-------------------------------------
//-------------TEST SCRIPTS------------
//-------------------------------------


void servocontrol(bool grab_attach)
{

  if (grab_attach){
    grabber.attach(GRAB_PIN);
    grabber_pos = dpad_pos(grabber_pos, GRAB_DOWN, GRAB_UP, GRAB_ANGLE);
    grabber.write(grabber_pos);
    //Serial.println("Grabber Attached");
    PS4.setLed(Green);
    detached = false;
  }

  else{
    if (!detached){
      grabber.detach();
    }
    scooper_pos = dpad_pos(scooper_pos, SCOOP_DOWN, SCOOP_UP, SCOOP_ANGLE);
    scooper.write(scooper_pos);
    //Serial.println("Scooper Control");
    PS4.setLed(Red);
  }
}

int dpad_pos(int pos, int minval, int maxval, int angleval){  
  int oldpos = pos;
  
  if (pos >= 0 && pos <= 180){
    if(PS4.getButtonClick(UP))
    {
      pos += angleval;
    }

    else if (PS4.getButtonClick(DOWN))
    {
      pos -= angleval;
    }

    else if (PS4.getButtonClick(LEFT)){
      pos = minval;
    }

    else if (PS4.getButtonClick(RIGHT)){
      pos = maxval;
    }
  }

  if (pos > 180){
    pos = 180;
  }

  else if (pos < 0){
    pos = 0;
  }
  
  if (pos != oldpos)
  {
    Serial.print("Current Position: ");
    Serial.println(pos);
  }

  if (pos == 0 || pos == 180){
    PS4.setLedFlash(100, 100);
  }

  else{
    PS4.setLedFlash(0, 0);
  }

  return pos;
}



