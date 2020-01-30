/*Define Motor Pins for Flywheel
  #Steps:
 1.Connect the M1_PWM & M2_PWM to UNO digital 3 & 4
 2.Connect the M1_EN & M2_EN to UNO digital 2 & 5
 3.Connect +5V & GND to UNO 5V & GND
 */
//Flywheel Motor Pins
#define E1 3 //M1 Speed Control
#define E2 5 //M2 Speed Control
#define M1 23 //M1 Direction Control
#define M2 22 //M2 Direction Control


//Shooter Definitions
Servo belt;

#define BELT_PIN 2 
#define BELT_SPEED 180 //Check what this speed it (0 max on direction, 180 max oter direction)
#define BELT_STOP 85 
#define WHEEL_SPEED 16


//Scooper Definitions
Servo scooper;

#define SCOOP_PIN 51
#define SCOOP_UP 0
#define SCOOP_DOWN 180

bool scoop_up;


void ball_setup(void)
{
  belt.attach(BELT_PIN);
  belt.write(BELT_STOP);
  belt.detach();

  scooper.attach(SCOOP_PIN);
  scooper.write(SCOOP_UP);
  scooper.detach();
  
  int i;
  for(i=2;i<=5;i++)
    pinMode(i, OUTPUT);  
  digitalWrite(E1,LOW);   
  digitalWrite(E2,LOW);
}


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
  digitalWrite(M1,LOW);    
  analogWrite (E2,WHEEL_SPEED);    
  digitalWrite(M2,HIGH);

  //belt.detach();
}

void scoop_ball (void){
  if(!scoop_up)
  {
    scooper.attach(SCOOP_PIN);
    scooper.write(SCOOP_UP);
    scooper.detach();
  }

  else
  {
    scooper.attach(SCOOP_PIN);
    scooper.write(SCOOP_DOWN);
    scooper.detach();
  }

  scoop_up = !scoop_up;
}

