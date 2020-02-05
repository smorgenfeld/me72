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

#define BELT_PIN 2 
#define BELT_SPEED 180 //Check what this speed it (0 max on direction, 180 max oter direction)
#define BELT_STOP 85 
#define WHEEL_SPEED 16


//Scooper Definitions
Servo scooper;

#define SCOOP_PIN 7
#define SCOOP_UP 0
#define SCOOP_DOWN 70

int scoop_pos = 90;

bool scoop_up;

//Grabber Definitions
Servo grabber;

#define GRAB_PIN 31
#define GRAB_DOWN 0
#define GRAB_UP 135

bool grabber_on;

//-------------------------------------
//--------------SETUP------------------
//-------------------------------------

void ball_setup(void)
{
  belt.attach(BELT_PIN);
  belt.write(BELT_STOP);
  belt.detach();

  scooper.attach(SCOOP_PIN);
  scooper.write(SCOOP_DOWN);
  scooper.detach();
  scoop_up = false;

  grabber.attach(GRAB_PIN);
  grabber.write(GRAB_UP);
  grabber.detach();
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
  digitalWrite(M2,HIGH);    
  analogWrite (E2,WHEEL_SPEED);    
  digitalWrite(M1,LOW);

  //belt.detach();
}

//-------------------------------------
//-------------SCOOPER-----------------
//-------------------------------------

void scoop_ball (void){
  if(!scoop_up)
  {
    scooper.attach(SCOOP_PIN);
    scooper.write(SCOOP_UP);
    //scooper.detach();
    Serial.println("Scooper Up");
    PS4.setLedFlash(10,10);
  }

  else
  {
    scooper.attach(SCOOP_PIN);
    scooper.write(SCOOP_DOWN);
    Serial.println("Scooper Down");
    //scooper.detach();
    PS4.setLedFlash(0, 0);
  }

  scoop_up = !scoop_up;
}

//-------------------------------------
//-------------GRABBER-----------------
//-------------------------------------

void grab_tower (void){
  if(!grabber_on)
  {
    grabber.attach(GRAB_PIN);
    grabber.write(GRAB_DOWN);
    Serial.println(" Grabber Down ");
//    grabber.detach();
  }

  else
  {
    grabber.attach(GRAB_PIN);
    grabber.write(GRAB_UP);
    Serial.println(" Grabber Up ");
//    grabber.detach();
  }

  grabber_on = !grabber_on;
}

//-------------------------------------
//-------------TEST SCRIPTS------------
//-------------------------------------

void servo_r2(void)
{
  int r2_val = PS4.getAnalogButton(R2);
  int serv_pos = map(r2_val, 0, 180, 0, 255);
  scooper.attach(SCOOP_PIN);
  scooper.write(serv_pos);
  PS4.setLedFlash(5, 5);
}

void servo_dpad()
{
  int oldpos = scoop_pos;
  if(PS4.getButtonClick(UP))
  {
    scoop_pos += 10;
  }

  else if (PS4.getButtonClick(DOWN))
  {
    scoop_pos -= 10;
  }
  
  scooper.attach(SCOOP_PIN);
  scooper.write(scoop_pos);
  
//  if (scoop_pos != oldpos)
//  {
//    Serial.print("Current position: ");
//    Serial.println(scoop_pos);
//  }
}

