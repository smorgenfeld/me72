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

void stop(void)                    //Stop
{
  digitalWrite(E1,0); 
  digitalWrite(M1,LOW);    
  digitalWrite(E2,0);   
  digitalWrite(M2,LOW);    
}   

void turn_R (char a,char b)             //Turn Right: Shooter Direction Forward
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}

void shooter_setup(void)
{
  int i;
  for(i=2;i<=5;i++)
    pinMode(i, OUTPUT);  
  digitalWrite(E1,LOW);   
  digitalWrite(E2,LOW);
}

